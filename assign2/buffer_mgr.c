#ifndef __DEBUG__
#define __DEBUG__
#endif

#include "buffer_mgr.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dberror.h"
#include "storage_mgr.h"
#include "validation.h"

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *bm, const char *pageFileName,
                  int numPages, ReplacementStrategy strategy,
                  void *strategyData)
{
#ifdef __DEBUG__
    printf("\nbegin initBufferPool\n");
#endif

    // Check for empty file name.
    if (pageFileName == NULL || strlen(pageFileName) == 0)
    {
        return RC_FILE_NOT_FOUND;
    }

    // Check for invalid number of pages.
    if (numPages <= 0)
    {
        return RC_FILE_NOT_FOUND;
    }

#ifdef __DEBUG__
    printf("\n  > before Openfile\n");
#endif
    // Open page file.
    SM_FileHandle *fileHandle = (SM_FileHandle *)malloc(sizeof(SM_FileHandle));
    RC openStatus = openPageFile((char *)pageFileName, fileHandle);
    if (openStatus != RC_OK)
    {
        free(fileHandle);
        return openStatus;
    }

    // Initialize frames.
    BM_FrameHandle *frames = (BM_FrameHandle *)malloc(sizeof(BM_FrameHandle) * numPages);
    for (int i = 0; i < numPages; ++i)
    {
        BM_FrameHandle *frame = &frames[i];
        frame->pageHandle = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
        frame->pageHandle->pageNum = NO_PAGE;
        frame->pageHandle->data = (char *)malloc(sizeof(char) * PAGE_SIZE);
        frame->isDirty = false;
        frame->fixedCount = 0;
    }

    // Initialize stats.
    BM_Stats *stats = (BM_Stats *)malloc(sizeof(BM_Stats));
    stats->numReads = 0;
    stats->numWrites = 0;
    stats->lastWrittenFrame = 0;
    stats->lastWriteTime = 0;
    stats->dirtyFlagsArray = (bool *)malloc(sizeof(bool) * numPages);
    stats->fixCountsArray = (int *)malloc(sizeof(int) * numPages);
    stats->pageNumberArray = (PageNumber *)malloc(sizeof(PageNumber) * numPages);

    // Collect all metadata in a single structure.
    // BM_BufferPool *metadata = (BM_BufferPool *)malloc(sizeof(BM_BufferPool));
    // metadata->frames = frames;
    // metadata->statData = stats;
    // metadata->fileHandle = fileHandle;
    BM_mgmtData *mgmtData = (BM_mgmtData *)malloc(sizeof(BM_mgmtData));
    mgmtData->frames = frames;
    mgmtData->statData = stats;
    mgmtData->fileHandle = fileHandle;

    // Set metadata in BufferPool.
    bm->pageFile = (char *)malloc(sizeof(char) * (strlen(pageFileName) + 1));
    strcpy(bm->pageFile, pageFileName);
    bm->numPages = numPages;
    bm->strategy = strategy;
    //bm->mgmtData = metadata;
    memmove(bm->mgmtData, mgmtData, sizeof(BM_mgmtData));

#ifdef __DEBUG__
    printf("\nend initBufferPool");
#endif
    return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const buffer_pool)
{
    BM_mgmtData *mgmtData = buffer_pool->mgmtData;
    RETURN_IF_NULL(mgmtData, RC_BM_NOT_INITIALIZED,
                   "Buffer pool not initialized.");

    // Flush all dirty pages to disk before shutdown.
    forceFlushPool(buffer_pool);

    // Check if any pages are currently pinned.
    for (int i = 0; i < buffer_pool->numPages; ++i)
    {
        BM_FrameHandle *frame = &mgmtData->frames[i];
        RETURN_IF_NOT_EQ(frame->fixedCount, 0, RC_BM_PAGE_IN_BUFFER_ERROR,
                         "Buffer pool in use.");
    }

    // Free all page data in buffer.
    for (int i = 0; i < buffer_pool->numPages; ++i)
    {
        BM_FrameHandle *frame = &mgmtData->frames[i];
        free(frame->pageHandle->data);
        free(frame->pageHandle);
        frame->pageHandle = NULL;
    }

    // Close the page file.
    closePageFile(mgmtData->fileHandle);

    // Free all specially allocated memory.
    free(mgmtData->statData->dirtyFlagsArray);
    free(mgmtData->statData->fixCountsArray);
    free(mgmtData->statData->pageNumberArray);
    free(mgmtData->fileHandle);
    free(mgmtData->frames);
    free(mgmtData->statData);
    free(buffer_pool->mgmtData);
    free(buffer_pool->pageFile);

    // Reset pointers.
    buffer_pool->mgmtData = NULL;
    buffer_pool->pageFile = NULL;

    RETURN_OK("Buffer pool shutdown successfully.");
}

// Flushes a frame to disk
RC flushFrame(BM_BufferPool *bufferPool, BM_FrameHandle *frameHandle)
{
    BM_mgmtData *mgmtData = bufferPool->mgmtData;
    SM_FileHandle *fileHandle = mgmtData->fileHandle;
    BM_PageHandle *pageHandle = frameHandle->pageHandle;

    // Ensure that the file has enough capacity to store the flushed page
    RC capacityStatus = ensureCapacity(pageHandle->pageNum + 1, fileHandle);
    CHECK_OK(capacityStatus);

    // Write the page to the file
    RC writeStatus = writeBlock(pageHandle->pageNum, fileHandle, pageHandle->data);
    CHECK_OK(writeStatus);

    // Mark the frame as not dirty
    frameHandle->isDirty = false;

    // Increment the number of writes
    ++mgmtData->statData->numWrites;

    RETURN_OK("Frame flushed to disk.");
}

// Forces all dirty pages in a buffer pool to be flushed to disk
RC forceFlushPool(BM_BufferPool *const bufferPool)
{
    BM_mgmtData *mgmtData = bufferPool->mgmtData;

    // Check if the buffer pool has been initialized
    RETURN_IF_NULL(mgmtData, RC_BM_NOT_INITIALIZED, "Buffer pool not initialized.");

    SM_FileHandle *fileHandle = mgmtData->fileHandle;

    // Iterate over all frames in the buffer pool
    for (int i = 0; i < bufferPool->numPages; ++i)
    {
        BM_FrameHandle *frame = &mgmtData->frames[i];

        // Flush the frame if it is dirty and not pinned
        if (frame->isDirty && frame->fixedCount == 0)
        {
            RC flushStatus = flushFrame(bufferPool, frame);
            CHECK_OK(flushStatus);
        }
    }

    RETURN_OK("Buffer pool flushed successfully.");
}

// Define the statistics interface
// Define the statistics interface
typedef struct BM_Statistics {
  PageNumber *pageNumberArray;
  bool *dirtyFlagsArray;
  int *fixCountsArray;
  int numReads;
  int numWrites;
} BM_Statistics;

// Function to initialize the statistics
BM_Statistics *initBMStatistics(int numPages) {
    BM_Statistics *statData = malloc(sizeof(BM_Statistics));
    statData->pageNumberArray = calloc(numPages, sizeof(PageNumber));
    statData->dirtyFlagsArray = calloc(numPages, sizeof(bool));
    statData->fixCountsArray = calloc(numPages, sizeof(int));
    statData->numReads = 0;
    statData->numWrites = 0;
    return statData;
}

// Function to update the statistics
void updateBMStatistics(BM_BufferPool *const bm) {
    for (int i = 0; i < bm->numPages; ++i) {
        BM_PageHandle *pageHandle = bm->mgmtData->frames[i].pageHandle;
        if (pageHandle != NULL) {
            bm->mgmtData->statData->pageNumberArray[i] = pageHandle->pageNum;
            bm->mgmtData->statData->dirtyFlagsArray[i] = bm->mgmtData->frames[i].isDirty;
            bm->mgmtData->statData->fixCountsArray[i] = bm->mgmtData->frames[i].fixedCount;
        }
    }
}


// Implement the statistics interface functions
PageNumber *getFrameContents(BM_BufferPool *const bm) {
    updateBMStatistics(bm);
    return bm->mgmtData->statData->pageNumberArray;
}

bool *getDirtyFlags(BM_BufferPool *const bm) {
    updateBMStatistics(bm);
    return bm->mgmtData->statData->dirtyFlagsArray;
}

int *getFixCounts(BM_BufferPool *const bm) {
    updateBMStatistics(bm);
    return bm->mgmtData->statData->fixCountsArray;
}

int getNumReadIO(BM_BufferPool *const bm) {
    return bm->mgmtData->statData->numReads;
}

int getNumWriteIO(BM_BufferPool *const bm) {
    return bm->mgmtData->statData->numWrites;
}


// +----------------+----------------------------------------------------------*
//    Description    Mark page frame dirty (modified)
//      
//                   page->pageNum: 0 .. N-1
// +----------------+----------------------------------------------------------*
// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    if (bm == NULL || page == NULL) 
    {
        return RC_BM_PAGE_NOT_FOUND;
    }

    // get the pointer to the current frame of the page
    BM_FrameHandle *frame = searchFrame(bm, page->pageNum);

    if (frame == NULL) 
    {
        return RC_BM_PAGE_NOT_FOUND;
    }

    // TODO: Check if markng the dirtyFlagsArray[pageNum] to true is necessary
    // mark dirty
    frame->isDirty = true;

#ifdef __DEBUG__
        printf("\n[markDirty] Frame [%d] marked dirty", page->pageNum);
#endif

    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    Inform the buffer manager that he no longer needs that page
//                   The pageNum field of page should be used to figure out 
//                   which page to unpin
// +----------------+----------------------------------------------------------*
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    if (bm == NULL || page == NULL) 
    {
        return RC_BM_PAGE_NOT_FOUND;
    }

    // get the pointer to the current frame of the page
    BM_FrameHandle *frame = searchFrame(bm, page->pageNum);

    if (frame == NULL) 
    {
        return RC_BM_PAGE_NOT_FOUND;
    }

    // TODO: Check if markng the fixCountsArray[pageNum] to true is necessary
    // decrease fixed count
    frame->fixedCount--;

#ifdef __DEBUG__
        printf("\n[unpinPage] Frame [%d] unpinned. New fixedCount [%d]", page->pageNum, frame->fixedCount);
#endif

    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    Write the current content of the page frame (block?) 
//                   back to the page file on disk.
//                  
//                   Write the current page frame (block) to the page file - 
//                   use writeBlock function of Storage Manager 
// +----------------+----------------------------------------------------------*
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    SM_PageHandle ph;
    RC rc;

    if (bm == NULL || page == NULL) 
    {
        return RC_BM_PAGE_NOT_FOUND;
    }

    // TODO: what is the pre-condition: fixedCount??

    // stuff block contents reading from current page frame 
    ph = (SM_PageHandle) malloc(PAGE_SIZE);
    memmove(ph, &bm->mgmtData->frames[page->pageNum], PAGE_SIZE);

    rc = writeBlock(page->pageNum, bm->mgmtData->fileHandle, ph);
    if (rc != RC_OK) 
    {

#ifdef __DEBUG__
        printf("\n[forcePage] writeBlock failed [RC: %d]", rc);
#endif
        return rc;
    }

#ifdef __DEBUG__
        printf("\n[forcePage] writeBlock succeeded");
#endif

    return rc;
}

// +----------------+----------------------------------------------------------*
//    Description    Request pages identified by their position in the page file
//                   (page number) to be loaded in a page frame. 
//                   Check whether the page requested by the client is already 
//                   cached in a page frame. If this is the case, then the buffer 
//                   simply returns a pointer to this page frame to the client. 
//                   Otherwise, the buffer manager has to read this page from 
//                   disk and decide in which page frame it should be stored 
//                   (this is what the replacement strategy is for). Once an 
//                   appropriate frame is found and the page has been loaded, 
//                   the buffer manager returns a pointer to this frame to the 
//                   client.
//
//   Implementations Here are the attributtes used for each algorithm
//                   [FIFO] firstPinnedTime 
//                   [LRU ] lastUsedTime 
// +----------------+----------------------------------------------------------*
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,
        const PageNumber pageNum)
{
    // search existring frames (BM_FrameHandle -> BM_PageHandle.pageNum)
    // get the pointer to the current frame of the page
    BM_FrameHandle *frame = searchFrame(bm, pageNum);
    RC rc;

    // if page exists in memory (frame), return the pointer
    if (frame != NULL) 
    {
        // update the frame info
        frame->fixedCount++;
        frame->lastUsedTime = (int)time(NULL);

        // update the page info
        page->pageNum = frame->pageHandle->pageNum;
        page->data = frame->pageHandle->data;

        return RC_OK;
    }

    // search for an empty frame
    frame = searchEmptyFrame(bm);

    // if empty frame found, return the frame
    if (frame != NULL)
    {
        // read the specific block into the empty frame
        rc = readBlock(pageNum, bm->mgmtData->fileHandle, frame->pageHandle->data);

        // reading error
        if (rc != RC_OK)
        {
            return rc;
        }

        // update the frame info
        frame->pageHandle->pageNum = pageNum;
        frame->fixedCount = 1;
        frame->lastUsedTime = (int)time(NULL);

        // update the page info
        page->pageNum = frame->pageHandle->pageNum;
        page->data = frame->pageHandle->data;

        return RC_OK; 
    }

    // if frame is full, apply replacement strategy
    frame = searchFrameByStrategy(bm, bm->strategy);
    if (frame == NULL) 
    {
        return RC_UNKNOWN_STRATEGY;
    }

    // if frame is dirty, write to page file
    if (frame->isDirty) {
        rc = writeBlock(pageNum, bm->mgmtData->fileHandle, frame->pageHandle->data);
        if (rc != RC_OK) 
        {

#ifdef __DEBUG__
        printf("\n[pinPage] writeBlock failed [RC: %d]", rc);
#endif
            return rc;
        }        
    }

    // replace the frame with the one read from the page file
    rc = readBlock(pageNum, bm->mgmtData->fileHandle, frame->pageHandle->data);
    // reading error
    if (rc != RC_OK)
    {
        return rc;
    }

    // update the frame info
    frame->pageHandle->pageNum = pageNum;
    frame->fixedCount = 1;
    frame->lastUsedTime = (int)time(NULL);
    frame->firstPinnedTime = (int)time(NULL);

    // update the page info
    page->pageNum = frame->pageHandle->pageNum;
    page->data = frame->pageHandle->data;

    return RC_OK;
}


// +----------------+----------------------------------------------------------*
//    Description    Find a frame return the pointer if found
// +----------------+----------------------------------------------------------*
BM_FrameHandle *searchFrame(BM_BufferPool *const bm, PageNumber const pn)
{
    // validate page number
    if (bm == NULL || pn >= bm->numPages || pn < 0) 
    {
#ifdef __DEBUG__
        printf("\n[searchFrame] Parameter validation failed");
#endif
        return NULL;
    }

    for (int i=0; i<bm->numPages; i++) 
    {
        // page fond in the frame
        if (bm->mgmtData->frames[i].pageHandle->pageNum == pn)
        {
            // return the pointer to the page frame
            return &bm->mgmtData->frames[i];
        };
    }

    return NULL;
}


// +----------------+----------------------------------------------------------*
//    Description    Search an empty frame
// +----------------+----------------------------------------------------------*
BM_FrameHandle *searchEmptyFrame(BM_BufferPool *const bm)
{
    // validate parameters
    if (bm == NULL)
    {
#ifdef __DEBUG__
        printf("\n[searchEmptyFrame] Parameter validation failed");
#endif
        return NULL;
    }

    for (int i=0; i<bm->numPages; i++) 
    {
        // page fond in the frame
        if (bm->mgmtData->frames[i].pageHandle->pageNum == NO_PAGE)
        {
            // return the pointer to the page frame
            return &bm->mgmtData->frames[i];
        };
    }

    return NULL;
}

// +----------------+----------------------------------------------------------*
//    Description    Search a frame based on replacement strategy 
//                   either FIFO or LSU
// +----------------+----------------------------------------------------------*
BM_FrameHandle *searchFrameByStrategy(BM_BufferPool *const bm, ReplacementStrategy strategy)
{
    BM_FrameHandle *frameToEvict = NULL;
    int timeFirstPinned, lastUsedTime;

    switch (strategy) 
    {
        case RS_FIFO:

            // set the initial frame with the first frame
            // assuming that the buffer pool is full --> needs to be enhanced
            timeFirstPinned = bm->mgmtData->frames[0].firstPinnedTime;
            frameToEvict = &bm->mgmtData->frames[0];

            for (int i = 1; i < bm->numPages; ++i) 
            {
                // a frame pinned earlier 
                if (timeFirstPinned > bm->mgmtData->frames[i].firstPinnedTime) 
                {
                    timeFirstPinned = bm->mgmtData->frames[i].firstPinnedTime;
                    frameToEvict = &bm->mgmtData->frames[i];
                }
            }
            return frameToEvict;            
        case RS_LRU: 

            // set the initial frame with the first frame
            // assuming that the buffer pool is full --> needs to be enhanced
            lastUsedTime = bm->mgmtData->frames[0].lastUsedTime;
            frameToEvict = &bm->mgmtData->frames[0];

            for (int i = 1; i < bm->numPages; ++i) 
            {
                // a frame least recently used
                if (lastUsedTime > bm->mgmtData->frames[i].lastUsedTime) 
                {
                    lastUsedTime = bm->mgmtData->frames[i].lastUsedTime;
                    frameToEvict = &bm->mgmtData->frames[i];
                }
            }
            return frameToEvict;             
        default:
            return NULL;
            break;
    }    

    return NULL;
}