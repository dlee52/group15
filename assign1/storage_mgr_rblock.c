/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_rblock.c
 * +----------------+----------------------------------------------------------+
 *    Description    reading blocks from disc
 *                   There are two types of read and write methods that have 
 *                   to be implemented: Methods with absolute addressing 
 *                   (e.g., readBlock) and methods that address relative to 
 *                   the current page of a file (e.g., readNextBlock).
 *    Remark         
 *                   by Daniel
 * +----------------+----------------------------------------------------------+
 */
#include "storage_mgr.h"

// +----------------+----------------------------------------------------------*
//    Description 	 read block
//                   The method reads the block at position pageNum from 
//                   a file and stores its content in the memory pointed to 
//                   by the memPage page handle. If the file has less than 
//                   pageNum pages, the method should return 
//                   RC_READ_NON_EXISTING_PAGE.
// +----------------+----------------------------------------------------------*

// global 
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int len;
    unsigned int offset;

    // sanity checks
    if(fHandle == NULL) {
        printf("\n[Error] File is not open.\n");
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if(fHandle->mgmtInfo.fp == NULL) {
        printf("\n[Error] File Handle is NULL.\n");
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if(pageNum <=0 || pageNum > fHandle->totalNumPages) {
        printf("\n[Error] Invalid page number (requested: %d, allowed: 1 - %d).\n", pageNum, fHandle->totalNumPages);
        return RC_READ_NON_EXISTING_PAGE;
    }

    // calculate offset of the request page. Assume that page number starts from 1.
    offset = sizeof(MGMT_Info) + (pageNum-1) * PAGE_SIZE;

    // move to the start of the requested page
    fseek(fHandle->mgmtInfo.fp, offset, SEEK_SET);

    // load the request page in file to memPage.
    len = fread(memPage, 1, PAGE_SIZE, fHandle->mgmtInfo.fp);
    if (len != PAGE_SIZE) {
        printf("[Error] Page read does not match the PAGE_SIZE [%d:%d] \r\n", len, PAGE_SIZE);
        return RC_READ_NON_EXISTING_PAGE;
    }    

    // update the current page position
    fHandle->curPagePos = pageNum;

    #ifdef __DEBUG__
    printf("\n[readBlock] pageNum: %d, curPagePos: %d, totalNumPages: %d, memPage: %s"
            , pageNum 
            , fHandle->curPagePos
            , fHandle->totalNumPages
            , memPage);    
    #endif
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 Return the current page position in a file
// +----------------+----------------------------------------------------------*
extern int getBlockPos (SM_FileHandle *fHandle)
{
    // sanity checks
    if(fHandle == NULL) {
        printf("\n[Error] File is not open.\n");
        return 0;
    }

    return fHandle->curPagePos;
}


// +----------------+----------------------------------------------------------*
//    Description 	 Read the first respective last page in a file
// +----------------+----------------------------------------------------------*
extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(1, fHandle, memPage);
}

extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->totalNumPages, fHandle, memPage);
}

// +----------------+----------------------------------------------------------*
//    Description 	 Read the current, previous, or next page relative to the 
//                   curPagePos of the file. The curPagePos should be moved 
//                   to the page that was read. If the user tries to read a 
//                   block before the first page of after the last page of 
//                   the file, the method should return 
//                   RC_READ_NON_EXISTING_PAGE.
// +----------------+----------------------------------------------------------*
extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos-1, fHandle, memPage);
}

extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->curPagePos+1, fHandle, memPage);
}
