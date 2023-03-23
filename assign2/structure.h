#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include "buffer_mgr.h"
#include "storage_mgr.h"

typedef struct BM_FrameHandle
{
    BM_PageHandle *pageHandle;
    bool isDirty;
    int fixedCount;
    int lastUsedTime;
} BM_FrameHandle;

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

typedef struct BM_Metadata
{
    SM_FileHandle *fileHandle;
    BM_FrameHandle *frames;
    BM_Stats *statData;
} BM_Metadata;

// convenience macros
#define MAKE_FRAMES(numFrames) \
    ((BM_FrameHandle *)malloc(sizeof(BM_FrameHandle) * (numFrames)))

#define MAKE_STATS() ((BM_Stats *)malloc(sizeof(BM_Stats)))

#define MAKE_FILE_HANDLE() ((SM_FileHandle *)malloc(sizeof(SM_FileHandle)));

#define MAKE_BM_METADATA() ((BM_Metadata *)malloc(sizeof(BM_Metadata)))

#endif // STRUCTURES_H_
