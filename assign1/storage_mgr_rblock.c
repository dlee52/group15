/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_rblock.c
 * +----------------+----------------------------------------------------------+
 *    Description    reading blocks from disc
 *                   There are two types of read and write methods that have 
 *                   to be implemented: Methods with absolute addressing 
 *                   (e.g., readBlock) and methods that address relative to 
 *                   the current page of a file (e.g., readNextBlock).
 *    Remark         
 *                   by Daniel
 * +----------------+----------------------------------------------------------+
 */
#include "storage_mgr.h"

// +----------------+----------------------------------------------------------*
//    Description 	 read block
//                   The method reads the block at position pageNum from 
//                   a file and stores its content in the memory pointed to 
//                   by the memPage page handle. If the file has less than 
//                   pageNum pages, the method should return 
//                   RC_READ_NON_EXISTING_PAGE.
// +----------------+----------------------------------------------------------*
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 Return the current page position in a file
// +----------------+----------------------------------------------------------*
extern int getBlockPos (SM_FileHandle *fHandle)
{
	return 0;
}


// +----------------+----------------------------------------------------------*
//    Description 	 Read the first respective last page in a file
// +----------------+----------------------------------------------------------*
extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description 	 Read the current, previous, or next page relative to the 
//                   curPagePos of the file. The curPagePos should be moved 
//                   to the page that was read. If the user tries to read a 
//                   block before the first page of after the last page of 
//                   the file, the method should return 
//                   RC_READ_NON_EXISTING_PAGE.
// +----------------+----------------------------------------------------------*
extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}

extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	return RC_OK;
}
