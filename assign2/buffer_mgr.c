#include "buffer_mgr.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dberror.h"
#include "storage_mgr.h"
#include "structure.h"
#include "validation.h"

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *bm, const char *pageFileName,
                  int numPages, ReplacementStrategy strategy,
                  void *strategyData)
{

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
    BM_Metadata *metadata = (BM_Metadata *)malloc(sizeof(BM_Metadata));
    metadata->frames = frames;
    metadata->statData = stats;
    metadata->fileHandle = fileHandle;

    // Set metadata in BufferPool.
    bm->pageFile = (char *)malloc(sizeof(char) * (strlen(pageFileName) + 1));
    strcpy(bm->pageFile, pageFileName);
    bm->numPages = numPages;
    bm->strategy = strategy;
    bm->mgmtData = metadata;

    return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const buffer_pool)
{
    BM_Metadata *metadata = buffer_pool->mgmtData;
    RETURN_IF_NULL(metadata, RC_BM_NOT_INITIALIZED,
                   "Buffer pool not initialized.");

    // Flush all dirty pages to disk before shutdown.
    forceFlushPool(buffer_pool);

    // Check if any pages are currently pinned.
    for (int i = 0; i < buffer_pool->numPages; ++i)
    {
        BM_FrameHandle *frame = &metadata->frames[i];
        RETURN_IF_NOT_EQ(frame->fixedCount, 0, RC_BM_PAGE_IN_BUFFER_ERROR,
                         "Buffer pool in use.");
    }

    // Free all page data in buffer.
    for (int i = 0; i < buffer_pool->numPages; ++i)
    {
        BM_FrameHandle *frame = &metadata->frames[i];
        free(frame->pageHandle->data);
        free(frame->pageHandle);
        frame->pageHandle = NULL;
    }

    // Close the page file.
    closePageFile(metadata->fileHandle);

    // Free all specially allocated memory.
    free(metadata->statData->dirtyFlagsArray);
    free(metadata->statData->fixCountsArray);
    free(metadata->statData->pageNumberArray);
    free(metadata->fileHandle);
    free(metadata->frames);
    free(metadata->statData);
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
    BM_Metadata *metadata = bufferPool->mgmtData;
    SM_FileHandle *fileHandle = metadata->fileHandle;
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
    ++metadata->statData->numWrites;

    RETURN_OK("Frame flushed to disk.");
}

// Forces all dirty pages in a buffer pool to be flushed to disk
RC forceFlushPool(BM_BufferPool *const bufferPool)
{
    BM_Metadata *metadata = bufferPool->mgmtData;

    // Check if the buffer pool has been initialized
    RETURN_IF_NULL(metadata, RC_BM_NOT_INITIALIZED, "Buffer pool not initialized.");

    SM_FileHandle *fileHandle = metadata->fileHandle;

    // Iterate over all frames in the buffer pool
    for (int i = 0; i < bufferPool->numPages; ++i)
    {
        BM_FrameHandle *frame = &metadata->frames[i];

        // Flush the frame if it is dirty and not pinned
        if (frame->isDirty && frame->fixedCount == 0)
        {
            RC flushStatus = flushFrame(bufferPool, frame);
            CHECK_OK(flushStatus);
        }
    }

    RETURN_OK("Buffer pool flushed successfully.");
}
