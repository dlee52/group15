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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "record_mgr.h"
#include "tables.h"
#include "dberror.h"

// This is custom data structure defined for making the use of Record Manager.
typedef struct RecordManager
{
	// Buffer Manager's PageHandle for using Buffer Manager to access Page files
	BM_PageHandle pageHandle;	// Buffer Manager PageHandle 
	// Buffer Manager's Buffer Pool for using Buffer Manager	
	BM_BufferPool bufferPool;
	// Record ID	
	RID recordID;
	// This variable defines the condition for scanning the records in the table
	Expr *condition;
	// This variable stores the total number of tuples in the table
	int tuplesCount;
	// This variable stores the location of first free page which has empty slots in table
	int freePage;
	// This variable stores the count of the number of records scanned
	int scanCount;
} RecordManager;

const int MAX_NUMBER_OF_PAGES = 200;
const int ATTRIBUTE_SIZE = 20; // Size of the name of the attribute

RecordManager *recordManager;

// ******** CUSTOM FUNCTIONS ******** //

// This function returns a free slot within a page
int findFreePageSlot(char *data, int recordSize)
{
	int i, totalSlots = PAGE_SIZE / recordSize; 

	for (i = 0; i < totalSlots; i++)
		if (data[i * recordSize] != '+')
			return i;
	return -1;
}
typedef int RC;

#define RC_OK 0
#define RC_RM_NO_TUPLE_WITH_GIVEN_RID 1
#define RC_UNPIN_PAGE_FAILED 2
#define RC_OK 0
#define RC_PIN_PAGE_FAILED -1
#define CHECK_OK(expression) { \
    RC result = (expression); \
    if (result != RC_OK) { \
        return result; \
    } \
}



#define NEGATIVE_ONE -1
#define ZERO 0
#define ONE 1

#define SIZE_ONE 1
#define SIZE_FOUR 4

// table and manager
RC initRecordManager(void *mgmtData)
{
    return RC_OK;
}

RC shutdownRecordManager()
{
    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    Create the underlying page file and store information
//                   about the schema, free-space, … and so on in the Table
//                   Information pages.
// +----------------+----------------------------------------------------------*
RC createTable(char *name, Schema *schema)
{
    CHECK_OK(createPageFile(name));
    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    All operations on a table such as scanning or inserting
//                   records require the table to be opened first. Afterwards,
//                   clients can use the RM_TableData struct to interact
//                   with the table.
// +----------------+----------------------------------------------------------*
RC openTable(RM_TableData *rel, char *name)
{
    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    Cause all outstanding changes to the table to be written
//                   to the page file.
// +----------------+----------------------------------------------------------*
RC closeTable(RM_TableData *rel)
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
RC deleteTable(char *name)
{
    return RC_OK;
}

// +----------------+----------------------------------------------------------*
//    Description    Cause all outstanding changes to the table to be written
//                   to the page file.
// +----------------+----------------------------------------------------------*
int getNumTuples(RM_TableData *rel)
{
    return 0;
}

// handling records in a table
/**
 * Inserts a new record into the table.
 * Returns RC_OK on success, or an error code on failure.
 */


RC insertRecord(RM_TableData *rel, Record *record) {
    // Retrieve metadata stored in the table
    RecordManager *recordManager = rel->mgmtData;    
    
    // Set the record ID for this record
    RID *recordID = &record->id; 
    
    // Get the size in bytes needed to store one record for the given schema
    int recordSize = getRecordSize(rel->schema);
    
    // Set the first free page to the current page
    recordID->page = recordManager->freePage;

    // Pin the page, i.e. tell the buffer manager that we are using this page
    pinPage(&recordManager->bufferPool, &recordManager->pageHandle, recordID->page);
    
    // Set the data to the initial position of the record's data
    char *data = recordManager->pageHandle.data;
    
    // Get a free slot using our custom function
    recordID->slot = findFreePageSlot(data, recordSize);

    while (recordID->slot == -1) {
        // If the pinned page doesn't have a free slot then unpin that page
        unpinPage(&recordManager->bufferPool, &recordManager->pageHandle);        
        
        // Increment the page
        recordID->page++;
        
        // Bring the new page into the buffer pool using the buffer manager
        pinPage(&recordManager->bufferPool, &recordManager->pageHandle, recordID->page);
        
        // Set the data to the initial position of the record's data        
        data = recordManager->pageHandle.data;

        // Again check for a free slot using our custom function
        recordID->slot = findFreePageSlot(data, recordSize);
    }
    
    char *slotPointer = data;
    
    // Mark the page dirty to notify that this page was modified
    markDirty(&recordManager->bufferPool, &recordManager->pageHandle);
    
    // Calculate the slot starting position
    slotPointer += (recordID->slot * recordSize);

    // Append '+' as a tombstone to indicate that this is a new record and should be removed if space is less
    *slotPointer = '+';

    // Copy the record's data to the memory location pointed to by slotPointer
    memcpy(++slotPointer, record->data + 1, recordSize - 1);

    // Unpin the page, i.e. remove the page from the buffer pool
    unpinPage(&recordManager->bufferPool, &recordManager->pageHandle);
    
    // Increment the count of tuples
    recordManager->tuplesCount++;
    
    // Pin the page back
    pinPage(&recordManager->bufferPool, &recordManager->pageHandle, 0);

    return RC_OK;
}

RC deleteRecord(RM_TableData *rel, RID id)
{
    RecordManager *manager = rel->mgmtData;

    // Pin the page containing the record to delete
    RC rc = pinPage(&manager->bufferPool, &manager->pageHandle, id.page);
    if (rc != RC_OK) {
        return rc;
    }

    // Calculate the offset of the record within the page
    int recordSize = getRecordSize(rel->schema);
    char *recordData = manager->pageHandle.data + (id.slot * recordSize);

    // Mark the record as deleted using tombstone marker '-'
    *recordData = '-';

    // Mark the page as dirty since it was modified
    markDirty(&manager->bufferPool, &manager->pageHandle);

    // Unpin the page since it's no longer needed in memory
    unpinPage(&manager->bufferPool, &manager->pageHandle);

    // Update the free page in the record manager
    manager->freePage = id.page;

    return RC_OK;
}
RC updateRecord(RM_TableData *rel, Record *record)
{
    RecordManager *recordManager = rel->mgmtData;
    RID *recordID = &record->id;

    // Pin the page containing the record to be updated
    pinPage(&recordManager->bufferPool, &recordManager->pageHandle, recordID->page);

    // Retrieve the data and schema for the table
    char *data = recordManager->pageHandle.data;
    Schema *schema = rel->schema;

    // Calculate the offset of the record to be updated in the page data
    int recordOffset = recordID->slot * getRecordSize(schema);

    // Update the tombstone value to indicate that the record is not deleted
    data[recordOffset] = '+';

    // Copy the new record data to the page data
    memcpy(data + recordOffset + 1, record->data + 1, getRecordSize(schema) - 1);

    // Mark the page as dirty
    markDirty(&recordManager->bufferPool, &recordManager->pageHandle);

    // Unpin the page
    unpinPage(&recordManager->bufferPool, &recordManager->pageHandle);

    return RC_OK;
}


RC getRecord(RM_TableData *rel, RID id, Record *record) {
    // Retrieve the record manager from the table metadata
    RecordManager *recordManager = rel->mgmtData;

    // Pin the page containing the record we want to retrieve
    if (pinPage(&recordManager->bufferPool, &recordManager->pageHandle, id.page) != RC_OK) {
        return RC_PIN_PAGE_FAILED;
    }

    // Calculate the location of the record in the page
    int recordSize = getRecordSize(rel->schema);
    char *recordLocation = recordManager->pageHandle.data + (id.slot * recordSize);

    // Check if the record exists in the page
    if (*recordLocation != '+') {
        unpinPage(&recordManager->bufferPool, &recordManager->pageHandle);
        return RC_RM_NO_TUPLE_WITH_GIVEN_RID;
    }

    // Copy the record data to the Record struct
    record->id = id;
    char *data = record->data;
    memcpy(++data, recordLocation + 1, recordSize - 1);

    // Unpin the page
    if (unpinPage(&recordManager->bufferPool, &recordManager->pageHandle) != RC_OK) {
        return RC_UNPIN_PAGE_FAILED;
    }

    return RC_OK;
}


// scans
RC startScan(RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
    return RC_OK;
}

RC next(RM_ScanHandle *scan, Record *record)
{
    return RC_OK;
}

RC closeScan(RM_ScanHandle *scan)
{
    return RC_OK;
}

setRecordValues(Record **record, Schema *schema)
{

    // memory allocation for data
    (*record)->data = (char *)malloc(getRecordSize(schema));

    // Set page to negative to one since its a new record
    (*record)->id.page = NEGATIVE_ONE;

    // Set slot to negative to one since its a new record
    (*record)->id.slot = NEGATIVE_ONE;
}

int setSizeForBool(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_BOOL)
        return SIZE_ONE;
    return ZERO;
}

int setSizeForFloat(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_FLOAT)
        return SIZE_FOUR;
    return ZERO;
}

int setSizeForInt(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_INT)
        return SIZE_FOUR;
    return ZERO;
}

int setSizeForString(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_STRING)
        return schema->typeLength[j];
    return ZERO;
}

int setOffsetString(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_STRING)
        return schema->typeLength[j];
    return ZERO;
}

int setOffsetInt(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_INT)
        return sizeof(int);
    return ZERO;
}

int setOffsetFloat(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_FLOAT)
        return sizeof(float);
    return ZERO;
}

int setOffsetBool(Schema *schema, int j)
{
    if (schema->dataTypes[j] == DT_BOOL)
        return sizeof(bool);
    return ZERO;
}

// utility method to copy from one string to another
copyString(char *valueChar, char *recordDataCharacter)
{
    strcpy(valueChar, recordDataCharacter);
}

setObjectForIntDataType(Schema *schema, Value *valueObject, int attrNum, char *recordDataCharacter)
{
    if (schema->dataTypes[attrNum] == DT_INT)
    {
        // The C library function void *memcpy(void *dest, const void *src, size_t n) copies n characters from memory area src to memory area dest.
        memcpy(&(valueObject->v.intV), recordDataCharacter, SIZE_FOUR);
        valueObject->dt = DT_INT;
    }
}

setObjectForFloatDataType(Schema *schema, Value *valueObject, int attrNum, char *recordDataCharacter)
{
    if (schema->dataTypes[attrNum] == DT_BOOL)
    {
        valueObject->dt = DT_BOOL;
        // The C library function void *memcpy(void *dest, const void *src, size_t n) copies n characters from memory area src to memory area dest.
        memcpy(&(valueObject->v.boolV), recordDataCharacter, ONE);
    }
}

setObjectForBooleanDataType(Schema *schema, Value *valueObject, int attrNum, char *recordDataCharacter)
{
    if (schema->dataTypes[attrNum] == DT_FLOAT)
    {
        valueObject->dt = DT_FLOAT;
        // The C library function void *memcpy(void *dest, const void *src, size_t n) copies n characters from memory area src to memory area dest.
        memcpy(&(valueObject->v.floatV), recordDataCharacter, SIZE_FOUR);
    }
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
