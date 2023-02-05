/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_rblock.c
 * +----------------+----------------------------------------------------------+
 *    Description    reading blocks from disc
 *    Remark         
 *                   by Daniel
 * +----------------+----------------------------------------------------------+
 */
#include "storage_mgr.h"

// +----------------+----------------------------------------------------------*
//    Description 	 read block
// +----------------+----------------------------------------------------------*
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 get block current block position
// +----------------+----------------------------------------------------------*
int getBlockPos (SM_FileHandle *fHandle)
{
	return 0;
}

// +----------------+----------------------------------------------------------*
//    Description 	 read first block
// +----------------+----------------------------------------------------------*
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 read previous block
// +----------------+----------------------------------------------------------*
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 read current block
// +----------------+----------------------------------------------------------*
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 read next block
// +----------------+----------------------------------------------------------*
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 read last block
// +----------------+----------------------------------------------------------*
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

