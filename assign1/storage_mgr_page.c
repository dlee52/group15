/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_page.c
 * +----------------+----------------------------------------------------------+
 *    Description    manipulating page files
 *    Remark         
 *                   by Tejas
 * +----------------+----------------------------------------------------------+
 */
#include <errno.h>
#include <string.h>
#include "storage_mgr.h"
extern void initStorageManager ()
{
	return;
}

extern RC createPageFile (char *fileName) 
{

    char    blockBuf[PAGE_SIZE];
    int     len;
    FILE    *fp;

    // File Write
    fp = fopen(fileName, "w+");
    if (fp == NULL) {
        printf(" [%s] file write error [%d][%s]\r\n",
            fileName, errno, strerror(errno));
        return RC_WRITE_FAILED;
    }

    fclose(fp);
    printf("\ncreatePageFile success\n");
   /* 
    recLen = sizeof(CloseRec);
    memset(&CloseRec, 0x00, RecLen);

    
    len = sizeof(TableSize);
    rlen = fread(&TableSize, 1, len, Fp);
    if (len != rlen) {
        printf(" [%s][%d][%d] file read error [%d][%s]\r\n",
            len, rlen, FileName, errno, strerror(errno));
        fclose(Fp);
        return -1;
    }
    len = TableSize;
    memset(FareTable,   0x00, sizeof(FareTable)); // LKJ 970722
    rlen = fread(FareTable, 1, len, Fp);
    if (len != rlen) {
        printf(" [%s][%d][%d] file read error [%d][%s]\r\n",
            len, rlen, FileName, errno, strerror(errno));
        fclose(Fp);
        return -1;
    }
    fclose(Fp);
    CmdInitSR(NodeNum, TaskNum, &CmdSR);
    CmdSR.SendBuf = FareTable;
    CmdSR.SendLen = 52;   // 52 = (±¸°£:1 + ¿ä±Ý:2 * 6) * 4   LKJ 970722
    memset(RBuf, 0x00, sizeof(RBuf));
    CmdSR.RecvBuf = RBuf;
    if (CmdSendRecv(&CmdSR)) {  
        return -1;
    }
    return 0;
    */
        
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