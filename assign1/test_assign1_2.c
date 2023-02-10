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

int main()
{
    RC rc;


    SM_FileHandle fHandle;
    SM_PageHandle memPage;


    char filename[100]= "/Users/daniel/c/525/assign1/page2.txt";


#ifndef __DEBUG__
    printf("\n>>>> Invoking createPageFile [PAGE_SIZE: %d, mgmtInfo: %lu]", PAGE_SIZE, sizeof(MGMT_Info));
#endif    
    rc = createPageFile(filename);

#ifndef __DEBUG__
    printf("\n>>>> Invoking openPageFile");
#endif    
    openPageFile(filename, &fHandle);
    
    memPage = (SM_PageHandle)malloc(PAGE_SIZE);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking readBlock");
#endif    
    rc = readBlock(0, &fHandle, memPage);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking readFirstBlock");
#endif    
    rc = readFirstBlock(&fHandle, memPage);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking writeBlock");
#endif    
    rc = writeBlock(0, &fHandle, memPage);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking readPreviousBlock");
#endif    
    rc = readPreviousBlock(&fHandle, memPage);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking readCurrentBlock");
#endif    
    rc = readCurrentBlock(&fHandle, memPage);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking readNextBlock");
#endif    
    rc = readNextBlock(&fHandle, memPage);
#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
    printf("\n>>>> Invoking readLastBlock");
#endif    
    rc = readLastBlock(&fHandle, memPage);

#ifndef __DEBUG__
    printf("\n[fHandle]totalNumPages: %d, getBlockPos: %d" , fHandle.totalNumPages, getBlockPos(&fHandle)); printf("\n");
#endif

    #ifndef __DEBUG__
    fclose(fHandle.mgmtInfo.fp);
    return rc;
    #endif
}