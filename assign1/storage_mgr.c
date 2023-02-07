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

    // set fHandle accordingly
    fHandle.fileName = "/Users/daniel/c/cs525-s23-group-15/assign1/page1.txt";
    fHandle.curPagePos = 0;
    fHandle.totalNumPages = 0;
    fHandle.mgmtInfo = NULL;

    rc = createPageFile(fHandle.fileName);
    //rc = readBlock(1, &fHandle, memPage);

    printf("\n%s\n",fHandle.fileName);
    printf("\n%lu\n",sizeof(MGMT_Info));
	//printError(RC_OK);
	return RC_OK;
}
