/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr.c
 * +----------------+----------------------------------------------------------+
 *    Description    Implementation of Return Codes/Messges 
 *    Remark         RC: Return Code
 *                   RM: Return Message
 * +----------------+----------------------------------------------------------+
 */

#include <errno.h>
#include <string.h>
#include "storage_mgr.h"

extern void initStorageManager (void)
{
    printf("Storage Manager is getting Build\n");
    printf("Creating page file\n"); 
}

extern RC createPageFile(char *fileName) {
    int     mgmtLen = sizeof(MGMT_Info);

    // buffer length
    int     buffLen = PAGE_SIZE + mgmtLen;
    int     len;
    char    buff[buffLen];
    FILE    *fp;
    MGMT_Info mgmtInfo;

    // File Write
    fp = fopen(fileName, "w+");
    if (fp == NULL) {
        printf(" [%s] file write error [%d][%s]\r\n",
            fileName, errno, strerror(errno));
        return RC_WRITE_FAILED;
    }

    // construct buffer to write 
    mgmtInfo.totalNumPages = 1; 
    memset(buff, 0x00, buffLen);
    memmove(buff, &mgmtInfo, sizeof(MGMT_Info));

    // write the buffer to file
    len = fwrite(buff, 1, buffLen, fp);

    // validate write succeeded
    if (len != buffLen) {
        printf(" [%s] File write error [%d] [%d] \r\n\a\a",
            fileName, buffLen, len);
        fclose(fp);
        return RC_WRITE_FAILED;
    }    

    fclose(fp);
    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    Opens an existing page file. Should return RC_FILE_NOT_FOUND 
//                   if the file does not exist. The second parameter is an 
//                   existing file handle. If opening the file is successful, 
//                   then the fields of this file handle should be initialized 
//                   with the information about the opened file. For instance, 
//                   you would have to read the total number of pages that are 
//                   stored in the file from disk.
// +----------------+----------------------------------------------------------*
extern RC openPageFile(char *filename, SM_FileHandle *fileHandle)
{
    MGMT_Info mgmtInfo;
    int rlen=0;

    //open the file with the given filename and "r" mode (read-only)
    FILE *fp = fopen(filename, "r");

    //declare two integer variables to store the file page count and the file size
    int filepgcnt, filesize;

    //check if the file has been successfully opened
    if (fp == NULL) {
        //if file is not found, return the error code RC_FILE_NOT_FOUND
        return RC_FILE_NOT_FOUND;
    } else {
        // Read mgmtInfo of the page
        rlen = fread(&mgmtInfo, 1, sizeof(MGMT_Info), fp);
        if (rlen != sizeof(MGMT_Info)) {
            fclose(fp);
            return RC_PAGE_INFO_READ_ERROR;
        }

        //if file is found, store the filename in the SM_FileHandle struct
        fileHandle->fileName = filename;

        //calculate the number of pages by dividing the file size by the page size
        fileHandle->totalNumPages = mgmtInfo.totalNumPages;

        //initialize the current page position to 0
        fileHandle->curPagePos = 0;

        //set the page info read from the file
        memmove(&fileHandle->mgmtInfo, &mgmtInfo, sizeof(MGMT_Info));

        //store the file pointer in the SM_FileHandle struct's management information     
        fileHandle->mgmtInfo.fp = fp;

        //return RC_OK if everything goes well
        return RC_OK;
    }
}

extern RC closePageFile(SM_FileHandle *fHandle)
{
    // Write MGMT_Info into the page file before closing
    MGMT_Info mgmtInfo;
    char buff[sizeof(MGMT_Info)];
    int len=0;

    // Read MGMT_Info in the page file into mgmtInfo
    memmove(&mgmtInfo, &fHandle->mgmtInfo, sizeof(MGMT_Info));
    mgmtInfo.totalNumPages = fHandle->totalNumPages;

    // Set the file position to the beginning
    // move to the start of the requested page
    fseek(fHandle->mgmtInfo.fp, 0, SEEK_SET);

    // write the buffer to file
    memset(buff, 0x00, sizeof(MGMT_Info));
    memmove(buff, &mgmtInfo, sizeof(MGMT_Info));    
    len = fwrite(buff, 1, sizeof(MGMT_Info), fHandle->mgmtInfo.fp);

    // validate write succeeded
    if (len != sizeof(MGMT_Info)) {
        //printf(" [%s] File write error [%d] [%d] \r\n\a\a", fHandle->fileName, (unsigned long) sizeof(MGMT_Info), len);
        fclose(fHandle->mgmtInfo.fp);
        return RC_WRITE_FAILED;
    }        
    // file write

  /* Close the file handle pointed to by fHandle->mgmtInfo */
  int result = fclose(fHandle->mgmtInfo.fp);

  /* Check if the file handle was closed successfully */
  if (result == 0)
  {
    /* Set the mgmtInfo field of the file handle to NULL */
    fHandle->mgmtInfo.fp = NULL;
    /* Return RC_OK to indicate that the file handle was closed successfully */
    return RC_OK;
  }
  else
  {
    /* Return RC_FILE_HANDLE_NOT_INIT to indicate that the file handle was not closed successfully */
    return RC_FILE_HANDLE_NOT_INIT;
  }
}

extern RC destroyPageFile(char *fileName)
{
  /* Use standard library function to delete the file */
  int result = remove(fileName);

  /* Check if file was successfully deleted */
  if (result == 0)
  {
    return RC_OK;
  }
  else
  {
    /* Return RC_FILE_NOT_FOUND if file was not found */
    return RC_FILE_NOT_FOUND;
  }
}


// +----------------+----------------------------------------------------------*
//    Description    read block
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
    if(pageNum <0 || pageNum > fHandle->totalNumPages) {
        printf("\n[Error] Invalid page number (requested: %d, allowed: 0 - %d).\n", pageNum, fHandle->totalNumPages);
        return RC_READ_NON_EXISTING_PAGE;
    }

    // calculate offset of the request page. Assume that page number starts from 0.
    offset = sizeof(MGMT_Info) + (pageNum) * PAGE_SIZE;

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
//    Description    Return the current page position in a file
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
//    Description    Read the first respective last page in a file
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
//    Description    Read the current, previous, or next page relative to the 
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

// Writing the page to a Disk by using absolute count
RC writeBlock(int pageNum, SM_FileHandle *fileHandle, SM_PageHandle memoryPage) {
    int len=0;
  if (!fileHandle) {
    return RC_FILE_HANDLE_NOT_INIT;
    }
  //FILE *fp = getFileDescriptor(fileHandle);
  FILE *fp = fileHandle->mgmtInfo.fp;
  if (!fp) return RC_FILE_NOT_FOUND;

  ensureCapacity(pageNum, fileHandle);

  int offset = sizeof(MGMT_Info);
  int pageLocation = offset + ((pageNum) * PAGE_SIZE);
  if (fseek(fp, pageLocation, SEEK_SET) != 0) return RC_WRITE_FAILED;
  // creating a block

  len = fwrite(memoryPage, sizeof(char), PAGE_SIZE, fp);

    // validate write succeeded
    if (len != PAGE_SIZE) {
        printf(" [%s] File write error [%d] [%d] \r\n\a\a",
            fileHandle->fileName, PAGE_SIZE, len);
        return RC_WRITE_FAILED;
    }    
  // creating an update of page position by using written block
  fileHandle->curPagePos = pageNum;
  // Now we have to update the overall number of pages.

 
  //int fileSize = ftell(fp);
  //int totalPages = (fileSize - offset) / PAGE_SIZE;
  //if ((fileSize - offset) % PAGE_SIZE != 0) ++totalPages;
  //fileHandle->totalNumPages = totalPages;
  fileHandle->totalNumPages++;

  return RC_OK;
}
// Now we have to write the current block
RC writeCurrentBlockToFile(SM_FileHandle *fileHandle, SM_PageHandle memoryPage) {
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  int currPage = fileHandle->curPagePos;
  return writeBlock(currPage, fileHandle, memoryPage);
}
//Here we are appending the empty block
RC appendEmptyPage(SM_FileHandle *fileHandle) {
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  int newPage = fileHandle->totalNumPages;

  FILE *fp = fileHandle->mgmtInfo.fp;
  if (!fp) return RC_FILE_NOT_FOUND;

  int offset = sizeof(MGMT_Info);
  int pageLocation = offset + (newPage * PAGE_SIZE);
  if (fseek(fp, pageLocation, SEEK_SET) != 0) return RC_WRITE_FAILED;

  char *emptyMemory = calloc(PAGE_SIZE, sizeof(char));
  if (!emptyMemory) return RC_WRITE_FAILED;
  fwrite(emptyMemory, sizeof(char), PAGE_SIZE, fp);
  free(emptyMemory);

  ++fileHandle->totalNumPages;

  return RC_OK;
}
// Here we have to ensure that there is sufficient capacity
// Description If the file has less than numberOfPages pages then increase the size to numberOfPages
RC ensureCapacity(int numPages, SM_FileHandle *fileHandle)
{
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  while (numPages > fileHandle->totalNumPages) {
    RC result = appendEmptyPage(fileHandle);
    if (result != RC_OK) return RC_WRITE_FAILED;
  }
  return RC_OK;
}