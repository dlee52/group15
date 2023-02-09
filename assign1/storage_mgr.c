/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr.c
 * +----------------+----------------------------------------------------------+
 *    Description    Implementation of Return Codes/Messges 
 *    Remark         RC: Return Code
 *                   RM: Return Message
 * +----------------+----------------------------------------------------------+
 */

#include <stdio.h>
#include "storage_mgr.h"

#include "dberror.h"

int main()
{
    RC rc;

    SM_FileHandle fHandle;
    SM_PageHandle memPage;


    char filename[100]= "/Users/daniel/c/sandbox/assign1/page1.txt";


    rc = createPageFile(filename);
    //rc = readBlock(1, &fHandle, memPage);

    openPageFile(filename, &fHandle);

    printf("\n[fHandle]\nfileName: %s, \ntotalNumPages: %d\n"
            , fHandle.fileName
            , fHandle.totalNumPages);

    fclose(fHandle.mgmtInfo.fp);
    return RC_OK;
}
