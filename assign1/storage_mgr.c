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
    printf("\n%lu\n",sizeof(MGMT_Info));
	printError(RC_OK);
	return RC_OK;
}
