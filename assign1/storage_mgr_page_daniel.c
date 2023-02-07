/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_page.c
 * +----------------+----------------------------------------------------------+
 *    Description    manipulating page files
 *    Remark         
 *                   by Daniel
 * +----------------+----------------------------------------------------------+
 */
#include <errno.h>
#include <string.h>
#include "storage_mgr.h"
extern void initStorageManager ()
{
	return;
}


SM_FileHandle fHandle;

// +----------------+----------------------------------------------------------*
//    Description    Create a new page file fileName. The initial file size 
//                   should be one page. This method should fill this single 
//                   page with '\0' bytes.
//            
//    Notes          It writes MGMT_Info at the beginning of the page file
// +----------------+----------------------------------------------------------*
extern RC createPageFile (char *fileName) 
{
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
}

extern RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
    
	return RC_OK;
}

extern RC closePageFile (SM_FileHandle *fHandle)
{
	return RC_OK;
}

extern RC destroyPageFile (char *fileName)
{
	return RC_OK;
}

/*
// Open Page file
extern RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
  // Read the file
  FILE *file = fopen(fileName, "r+");
  // Check the existence of file
  RETURN_IF_NULL(file, RC_FILE_NOT_FOUND, "File not found.");

  // Check  last position of file
  fseek(file, 0, SEEK_END);

  int length = ftell(file);
  int totalPages = length / PAGE_SIZE;
  if (length % PAGE_SIZE != 0) ++totalPages;

  fHandle->fileName = fileName;
  fHandle->totalNumPages = totalPages;
  fHandle->curPagePos = 0;
  fHandle->mgmtInfo = file;

  // Reset file description to beginning of file.
  fseek(file, 0, SEEK_SET);
  // Success Message
  RETURN_OK("File opened and metadata initialized");

  // NOTE: File remains open to be reused for future operations.
}

// Close Page File Method
extern RC closePageFile(SM_FileHandle *fHandle) {
  RETURN_IF_NULL(fHandle, RC_FILE_HANDLE_NOT_INIT, "File handle missing");
  FILE *file = getFileDescriptor(fHandle);
  RETURN_IF_NULL(file, RC_FILE_HANDLE_NOT_INIT,
                 "File not open or already closed.");

  fclose(fHandle->mgmtInfo);
  fHandle->mgmtInfo = NULL;

  RETURN_OK("File closed.");
}

// Destroy Page File
extern RC destroyPageFile(char *fileName) {
  int remoteStatus = remove(fileName);
  RETURN_IF_NOT_EQ(remoteStatus, 0, RC_FILE_NOT_FOUND, "File does not exist.");

  RETURN_OK("File destroyed");
}
*/