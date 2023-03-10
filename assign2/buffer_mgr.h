#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

// Include return codes and methods for logging errors
#include "dberror.h"
#include "storage_mgr.h"

// Include bool DT
#include "dt.h"


#ifndef BUFFER_MANAGER_CUSTOM_H
#define BUFFER_MANAGER_CUSTOM_H

typedef struct BM_Stats
{
    int numReads;
    int numWrites;
    int lastWrittenFrame;
    int lastWriteTime;
    int *fixCountsArray;
    bool *dirtyFlagsArray;
    int *pageNumberArray;
} BM_Stats;

typedef int PageNumber;
typedef struct BM_PageHandle {
	PageNumber pageNum;
	char *data;
} BM_PageHandle;

typedef struct BM_FrameHandle
{
    BM_PageHandle *pageHandle;
    bool isDirty;
    int fixedCount;
    int lastUsedTime;    // for LRU
    int firstPinnedTime; // for FIFO
} BM_FrameHandle;

typedef struct BM_mgmtData
{
    SM_FileHandle *fileHandle;
    BM_FrameHandle *frames;
    BM_Stats *statData;
} BM_mgmtData;

// convenience macros
#define MAKE_FRAMES(numFrames) \
    ((BM_FrameHandle *)malloc(sizeof(BM_FrameHandle) * (numFrames)))

#define MAKE_STATS() ((BM_Stats *)malloc(sizeof(BM_Stats)))

#define MAKE_FILE_HANDLE() ((SM_FileHandle *)malloc(sizeof(SM_FileHandle)));

#define MAKE_BM_METADATA() ((BM_mgmtData *)malloc(sizeof(BM_mgmtData)))

#endif
// Replacement Strategies
typedef enum ReplacementStrategy {
	RS_FIFO = 0,
	RS_LRU = 1,
	RS_CLOCK = 2,
	RS_LFU = 3,
	RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
#define NO_PAGE -1

typedef struct BM_BufferPool {
	char *pageFile;
	int numPages;
	ReplacementStrategy strategy;
	BM_mgmtData *mgmtData; // use this one to store the bookkeeping info your buffer
	// manager needs for a buffer pool
} BM_BufferPool;



// convenience macros
#define MAKE_POOL()					\
		((BM_BufferPool *) malloc (sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE()				\
		((BM_PageHandle *) malloc (sizeof(BM_PageHandle)))

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		const int numPages, ReplacementStrategy strategy,
		void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);

// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
		const PageNumber pageNum);

// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm);
bool *getDirtyFlags (BM_BufferPool *const bm);
int *getFixCounts (BM_BufferPool *const bm);
int getNumReadIO (BM_BufferPool *const bm);
int getNumWriteIO (BM_BufferPool *const bm);

// Custom Functions
BM_FrameHandle *searchFrame(BM_BufferPool *const bm, PageNumber const pn);
BM_FrameHandle *searchEmptyFrame(BM_BufferPool *const bm);
BM_FrameHandle *searchFrameByStrategy(BM_BufferPool *const bm, ReplacementStrategy strategy);
#endif



