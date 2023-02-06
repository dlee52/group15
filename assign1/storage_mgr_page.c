/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   storage_mgr_page.c
 * +----------------+----------------------------------------------------------+
 *    Description    manipulating page files
 *    Remark         
 *                   by Tejas
 * +----------------+----------------------------------------------------------+
 */
#include "storage_mgr.h"

extern void initStorageManager ()
{
	return;
}

extern RC createPageFile (char *fileName) 
{
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
