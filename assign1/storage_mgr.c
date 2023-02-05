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
	printError(RC_FILE_NOT_FOUND);
	return 0;
}
