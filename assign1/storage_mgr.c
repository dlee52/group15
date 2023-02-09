/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr.c
 * +----------------+----------------------------------------------------------+
 *    Description    Implementation of Return Codes/Messges 
 *    Remark         RC: Return Code
 *                   RM: Return Message
 * +----------------+----------------------------------------------------------+
 */

#include "storage_mgr.h"

int main()
{
    RC rc;

    SM_FileHandle fHandle;
    SM_PageHandle memPage;


    char filename[100]= "/Users/daniel/c/525/assign1/page2.txt";


    rc = createPageFile(filename);

    openPageFile(filename, &fHandle);
    
    memPage = (SM_PageHandle)malloc(PAGE_SIZE);
    rc = readBlock(1, &fHandle, memPage);
    rc = readFirstBlock(&fHandle, memPage);
    rc = readPreviousBlock(&fHandle, memPage);
    rc = readCurrentBlock(&fHandle, memPage);
    rc = readNextBlock(&fHandle, memPage);
    rc = readLastBlock(&fHandle, memPage);

    #ifdef __DEBUG__
    printf("\n[fHandle] fileName: %s, totalNumPages: %d, getBlockPos: %d"
            , fHandle.fileName
            , fHandle.totalNumPages
            , getBlockPos(&fHandle));
    printf("\n"); 
    #endif

    fclose(fHandle.mgmtInfo.fp);
    return rc;
}
