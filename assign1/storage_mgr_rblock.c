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

