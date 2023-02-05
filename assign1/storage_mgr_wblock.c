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

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return RC_OK;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return RC_OK;
}

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    return RC_OK;
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
    return RC_OK;
}


