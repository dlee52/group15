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

extern RC createPageFile(const char *filename) {
    MGMT_Info mgmtInfo;
    int     mgmtLen = sizeof(MGMT_Info);
    int     buffLen = PAGE_SIZE + mgmtLen;
    int     len;
    char    buff[buffLen];
    FILE    *fp;

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
    printf("\nCreated a page, %s. Wrote %d(mgmtInfo: %d + page:%d) bytes\n", fileName
            , PAGE_SIZE + mgmtLen
            , PAGE_SIZE, mgmtLen);

    return RC_OK;
/*
  // Open the file with "w+" mode, creating it if it doesn't exist, and allowing both reading and writing.
  FILE *fp = fopen(filename, "w+");
  // If the file pointer is NULL, the file could not be opened and return RC_FILE_NOT_FOUND.
  if (!fp) {
    return RC_FILE_NOT_FOUND;
  }

  // Allocate memory for the page using calloc, which sets all bytes to zero.
  char *page = calloc(PAGE_SIZE, sizeof(char));
  // If the memory allocation failed, close the file and return RC_WRITE_FAILED.
  if (!page) {
    fclose(fp);
    return RC_WRITE_FAILED;
  }

  // Write the contents of the memory block to the file.
  size_t elements_written = fwrite(page, sizeof(char), PAGE_SIZE, fp);
  // If the number of elements written is not equal to PAGE_SIZE, free the memory, close the file, and return RC_WRITE_FAILED.
  if (elements_written != PAGE_SIZE) {
    free(page);
    fclose(fp);
    return RC_WRITE_FAILED;
  }

  // Free the memory used for the page.
  free(page);
  // Close the file.
  fclose(fp);
  // Return RC_OK to indicate success.
  return RC_OK;
  */
}


extern RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
	//open the file with the given filename and "r" mode (read-only)
    FILE *fp = fopen(filename, "r");

    //declare two integer variables to store the file page count and the file size
    int filepgcnt, filesize;

    //check if the file has been successfully opened
    if (fp == NULL) {
        //if file is not found, return the error code RC_FILE_NOT_FOUND
        return RC_FILE_NOT_FOUND;
    } else {
        //if file is found, store the filename in the SM_FileHandle struct
        fileHandle->fileName = filename;

        //initialize the current page position to 0
        fileHandle->curPagePos = 0;

        //get the size of the file by calling getFileSize
        filesize = getFileSize(fp);

        //calculate the number of pages by dividing the file size by the page size
        filepgcnt = filesize / PAGE_SIZE;

        //store the calculated number of pages in the SM_FileHandle struct
        fileHandle->totalNumPages = filepgcnt;

        //store the file pointer in the SM_FileHandle struct's management information
        fileHandle->mgmtInfo = fp;

        //return RC_OK if everything goes well
        return RC_OK;
    }
}

int getFileSize(FILE *fp) {
    //get the current position of the file pointer
    int currentPos = ftell(fp);

    //move the file pointer to the end of the file
    fseek(fp, 0, SEEK_END);

    //get the size of the file
    int size = ftell(fp);

    //move the file pointer back to its original position
    fseek(fp, currentPos, SEEK_SET);

    //return the size of the file
    return size;
}

extern RC closePageFile (SM_FileHandle *fHandle)
{
	/* Close the file handle pointed to by fHandle->mgmtInfo */
	int result = fclose((FILE) fHandle->mgmtInfo);

	/* Check if the file handle was closed successfully */
	if (result == 0) {
    	/* Set the mgmtInfo field of the file handle to NULL */
    	fHandle->mgmtInfo = NULL;
    	/* Return RC_OK to indicate that the file handle was closed successfully */
    	return RC_OK;
	} else {
    	/* Return RC_FILE_NOT_CLOSED to indicate that the file handle was not closed successfully */
    	return RC_FILE_NOT_CLOSED;
	}
}


extern RC destroyPageFile(char *fileName) {
  /* Use standard library function to delete the file */
  int result = remove(fileName);

  /* Check if file was successfully deleted */
  if (result == 0) {
    return RC_OK;
  } else {
    /* Return RC_FILE_NOT_FOUND if file was not found */
    return RC_FILE_NOT_FOUND;
  }
}

