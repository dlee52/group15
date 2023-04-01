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
#include "validation.h"
#include "storage_mgr.h"

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
    // create a page file
    CHECK_OK(createPageFile(name));

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
extern int getRecordSize(Schema *schema)
{

    // initializing record size
    int rs = ZERO;
    for (int j = 0; j < schema->numAttr; j++)
    {

        // case depending on Boolean DATA TYPE
        rs += setSizeForBool(schema, j);

        // case depending on Float DATA TYPE
        rs += setSizeForFloat(schema, j);

        // case depending on integer DATA TYPE
        rs += setSizeForInt(schema, j);

        // case depending on String DATA TYPE
        rs += setSizeForString(schema, j);
    }
    return rs;
}

extern Schema *createSchema(int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{

    // memory allocation
    Schema *schema = (Schema *)malloc(sizeof(Schema));
    ;

    // initializing numattr to schema
    schema->numAttr = numAttr;

    // initializing attrNames to schema
    schema->attrNames = attrNames;

    // initializing dataTypes to schema
    schema->dataTypes = dataTypes;

    // initializing typeLength to schema
    schema->typeLength = typeLength;

    // initializing keySize to schema
    schema->keySize = keySize;

    // initializing keys to schema
    schema->keyAttrs = keys;

    return schema;
}

extern RC freeSchema(Schema *schema)
{

    // The C library function void free(void *ptr) deallocates the memory previously
    // allocated by a call to calloc, malloc, or realloc
    free(schema);
    return RC_OK;
}

    

// dealing with records and attribute values
extern RC createRecord(Record **record, Schema *schema)
{

    // memory allocation for new record
    *record = (Record *)malloc(sizeof(Record));

    // set record values
    setRecordValues(record, schema);

    return RC_OK;
}

extern RC freeRecord(Record *record)
{
    // The C library function void free(void *ptr) deallocates the memory previously
    // allocated by a call to calloc, malloc, or realloc
    free(record);
    return RC_OK;
}

extern RC getAttr(Record *record, Schema *schema, int attrNum, Value **value)
{

    int res = 0;
    for (int j = 0; j < attrNum; j++)
    {

        // case depending on the datatype string
        res += setOffsetString(schema, j);

        // case depending on the datatype int
        res += setOffsetInt(schema, j);

        // case depending on the datatype boolean
        res += setOffsetBool(schema, j);

        // case depending on the datatype float
        res += setOffsetFloat(schema, j);
    }
    int objectSize = 0;
    Value *valueObject = (Value *)malloc(sizeof(Value));
    char *recordDataCharacter = record->data;
    recordDataCharacter += res;

    // case depending on the datattype int
    setObjectForIntDataType(schema, valueObject, attrNum, recordDataCharacter);

    // case depending on the datattype string
    if (schema->dataTypes[attrNum] == DT_STRING)
    {
        // assigning string datatype to string in value object
        valueObject->dt = DT_STRING;
        objectSize = schema->typeLength[attrNum];
        // storing the object size to value object
        valueObject->v.stringV = (char *)malloc(objectSize + ONE);
        // utility method to copy string from one to another
        copyString(valueObject->v.stringV, recordDataCharacter);
        valueObject->v.stringV[objectSize] = '\0';
    }

    // case depending on the datattype float
    setObjectForFloatDataType(schema, valueObject, attrNum, recordDataCharacter);

    // case depending on the datattype boolean
    setObjectForBooleanDataType(schema, valueObject, attrNum, recordDataCharacter);

    *value = valueObject;
    return RC_OK;
}

extern RC setAttr(Record *record, Schema *schema, int attrNum, Value *value)
{
    int res = 0;
    for (int j = 0; j < attrNum; j++)
    {

        // case depending on the datatype string
        res += setOffsetString(schema, j);

        // case depending on the datatype int
        res += setOffsetInt(schema, j);

        // case depending on the datatype boolean
        res += setOffsetBool(schema, j);

        // case depending on the datatype float
        res += setOffsetFloat(schema, j);
    }

    // set data of the record to char variable
    char *objectValue = record->data;
    // assigning the data to the result
    objectValue += res;
    char *offsetResult;
    int objectSize = schema->typeLength[attrNum];

    // case depending on the datatype int
    if (schema->dataTypes[attrNum] == DT_INT)
    {
        // storing int value from value object and storing them to objectvalue
        // casting object value to int
        *(int *)objectValue = value->v.intV;
    }

    // case depending on the datatype int
    if (schema->dataTypes[attrNum] == DT_STRING)
    {
        offsetResult = (char *)malloc(objectSize + ONE);
        // copy string value from value object to offsetresult
        //  /The C library function char *strcpy(char *dest, const char *src) copies the string pointed to, by src to dest.
        strcpy(offsetResult, value->v.stringV);
        offsetResult[objectSize] = '\0';
        // copy objectResult from value object to offsetvalue
        //  /The C library function char *strcpy(char *dest, const char *src) copies the string pointed to, by src to dest.
        strcpy(objectValue, offsetResult);
        // The C library function void free(void *ptr) deallocates the memory previously allocated by a call to calloc, malloc, or realloc.
        free(offsetResult);
    }

    // case depending on the datatype float
    if (schema->dataTypes[attrNum] == DT_FLOAT)
    {
        // storing float value from value object and storing them to objectvalue
        // casting object value to float
        *(float *)objectValue = value->v.floatV;
    }

    // case depending on the datatype noolean
    if (schema->dataTypes[attrNum] == DT_BOOL)
    {
        // storing boolean value from value object and storing them to objectvalue
        // casting object value to boolean
        *(bool *)objectValue = value->v.boolV;
    }

    return RC_OK;
}
