/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_wblock.c
 * +----------------+----------------------------------------------------------+
 *    Description    writing blocks to a page file
 *    Remark         
 *                   by Vatsal
 * +----------------+----------------------------------------------------------+
 */

#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>

// Writing the page to a Disk by using absolute count
RC writeBlockToDisk(int pageNum, SM_FileHandle *fileHandle, SM_PageHandle memoryPage) {
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  FILE *fp = getFileDescriptor(fileHandle);
  if (!fp) return RC_FILE_NOT_FOUND;

  ensureSufficientCapacity(pageNum, fileHandle);

  int offset = sizeof(MGMT_Info);
  int pageLocation = offset + (pageNum * PAGE_SIZE);
  if (fseek(fp, pageLocation, SEEK_SET) != 0) return RC_WRITE_FAILED;
  // creating a block

  fwrite(memoryPage, sizeof(char), PAGE_SIZE, fp);
  // creating an update of page position by using written block
  fileHandle->curPagePos = pageNum;
  // Now we have to update the overall number of pages.

  int fileSize = ftell(fp);
  int totalPages = (fileSize - offset) / PAGE_SIZE;
  if ((fileSize - offset) % PAGE_SIZE != 0) ++totalPages;
  fileHandle->totalNumPages = totalPages;

  return RC_OK;
}
// Now we have to write the current block
RC writeCurrentBlockToFile(SM_FileHandle *fileHandle, SM_PageHandle memoryPage) {
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  int currPage = fileHandle->curPagePos;
  return writeBlockToDisk(currPage, fileHandle, memoryPage);
}
//Here we are appending the empty block
RC appendEmptyPage(SM_FileHandle *fileHandle) {
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  int newPage = fileHandle->totalNumPages;

  FILE *fp = getFileDescriptor(fileHandle);
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
RC ensureSufficientCapacity(int numPages, SM_FileHandle *fileHandle)
{
  if (!fileHandle) return RC_FILE_HANDLE_NOT_INIT;
  while (numPages > fileHandle->totalNumPages); {
    RC result = appendEmptyPage(fileHandle);
    if (result != RC_OK) return RC_WRITE_FAILED;
  }
  return RC_OK;
}

