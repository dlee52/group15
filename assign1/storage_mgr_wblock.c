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

extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return RC_OK;
}

extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return RC_OK;
}

extern RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    return RC_OK;
}

extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
    return RC_OK;
}


