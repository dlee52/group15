/*
 * +----------------+----------------------------------------------------------+
 *    Module/Func.   record_mgr.c
 * +----------------+----------------------------------------------------------+
 *    Description    
 *                   The record manager handles tables with a fixed schema. 
 *                   Clients can insert records, delete records, update records, 
 *                   and scan through the records in a table. A scan is associated 
 *                   with a search condition and only returns records that match 
 *                   the search condition. Each table should be stored in a separate 
 *                   page file and your record manager should access the pages of 
 *                   the file through the buffer manager implemented in the 
 *                   last assignment.
 * +----------------+----------------------------------------------------------+
 */

#include "record_mgr.h"

// table and manager
RC initRecordManager (void *mgmtData)
{
    return RC_OK;
}
    
RC shutdownRecordManager ()
{
    return RC_OK;
}
    
// +----------------+----------------------------------------------------------*
//    Description    Create the underlying page file and store information 
//                   about the schema, free-space, … and so on in the Table 
//                   Information pages.
// +----------------+----------------------------------------------------------*
RC createTable (char *name, Schema *schema)
{
    return RC_OK;
}
    
// +----------------+----------------------------------------------------------*
//    Description    All operations on a table such as scanning or inserting 
//                   records require the table to be opened first. Afterwards, 
//                   clients can use the RM_TableData struct to interact 
//                   with the table.
// +----------------+----------------------------------------------------------*
RC openTable (RM_TableData *rel, char *name)
{
    return RC_OK;
}
    
// +----------------+----------------------------------------------------------*
//    Description    Cause all outstanding changes to the table to be written 
//                   to the page file.
// +----------------+----------------------------------------------------------*
RC closeTable (RM_TableData *rel)
{
    return RC_OK;
}
    
// +----------------+----------------------------------------------------------*
//    Description    
//                   Delete the page file
//    
//    Clarification  
//                   Any other behaviors required?
// +----------------+----------------------------------------------------------*
RC deleteTable (char *name)
{
    return RC_OK;
}
    

// +----------------+----------------------------------------------------------*
//    Description    Cause all outstanding changes to the table to be written 
//                   to the page file.
// +----------------+----------------------------------------------------------*
int getNumTuples (RM_TableData *rel)
{
    return 0;
}
    

// handling records in a table
RC insertRecord (RM_TableData *rel, Record *record)
{
    return RC_OK;
}
    
RC deleteRecord (RM_TableData *rel, RID id)
{
    return RC_OK;
}
    
RC updateRecord (RM_TableData *rel, Record *record)
{
    return RC_OK;
}
    
RC getRecord (RM_TableData *rel, RID id, Record *record)
{
    return RC_OK;
}
    

// scans
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
    return RC_OK;
}
    
RC next (RM_ScanHandle *scan, Record *record)
{
    return RC_OK;
}
    
RC closeScan (RM_ScanHandle *scan)
{
    return RC_OK;
}
    

// dealing with schemas
extern int getRecordSize (Schema *schema)
{
    return RC_OK;
}
    
extern Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{
    return NULL;
}
    
RC freeSchema (Schema *schema)
{
    return RC_OK;
}
    

// dealing with records and attribute values
RC createRecord (Record **record, Schema *schema)
{
    return RC_OK;
}
    
RC freeRecord (Record *record)
{
    return RC_OK;
}
    
RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
    return RC_OK;
}
    
RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
    return RC_OK;
}
