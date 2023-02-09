/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_page.c
 * +----------------+----------------------------------------------------------+
 *    Description    manipulating page files
 *    Remark         
 *                   by Tejas
 * +----------------+----------------------------------------------------------+
 */
#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern void initStorageManager (void)
{
	printf("Storage Manager is getting Build\n");
	printf("Creating page file\n"); 
}

extern RC createPageFile(char *fileName) {
    int     mgmtLen = sizeof(MGMT_Info);
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
    memcpy(buff, &mgmtInfo, sizeof(MGMT_Info));

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


