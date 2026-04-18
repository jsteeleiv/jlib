#pragma once
#ifndef JMEMCTRL_H
#define JMEMCTRL_H

#include <stdio.h>
#include <stdlib.h>

#include "jalloc.h"
#include "jheap.h"
#include "../jdata/jmap.h"
#include "../jstd.h"


// what is the actual thing we want to occur?
typedef enum MemoryOperationFlags {
    /* standard allocation stuff */
    MEMOP_FREE = 0,
    MEMOP_ALLOC,
    MEMOP_CALLOC,
    MEMOP_MALLOC,
    MEMOP_REALLOC,
    /* custom/proprietary stuff */
    MEMOP_TAG,
    MEMOP_MARK,
    MEMOP_DUMP,
    MEMOP_NUKE,
    MEMOP_SWEEP,
    MEMOP_RETAIN,
    MEMOP_RELEASE,
    MEMOP_COLLECT,
    MEMOP_REGISTER,
    MEMOP_UNREGISTER,
    /* flag capacity */
    MEMOP_COUNT = 16
} memop_flags; // memory operation to perform

typedef uint16_t memop_flags;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


// wrap flags in struct
typedef struct MemoryOperation {
    memop_flags *flags;
    uint32_t mask;
    int type;
} memop_t;

typedef unsigned int jmem_loc_flags_t;

typedef enum MemoryLocationFlags {
    JMEM_LOC_NONE            = 0u,

    JMEM_LOC_HAS_FILE        = 1u << 0,
    JMEM_LOC_HAS_LINE        = 1u << 1,
    JMEM_LOC_HAS_FUNC        = 1u << 2,

    JMEM_LOC_CALLSITE        = 1u << 3,  /* came from user-facing macro */
    JMEM_LOC_INTERNAL        = 1u << 4,  /* internal jmem event */
    JMEM_LOC_SYNTHETIC       = 1u << 5   /* generated / not direct source */
} memloc_flags;

// where did this operation come from?
typedef struct MemoryLocation {
    uintptr_t addr;
    file_t file;
    bool owns;
} memloc_t; // where an allocation came from

/*
    bitmask; stores checks, properties, or modifiers, (etc?)
    {what behavior, or configuration, applies here?}
*/
typedef struct MemoryFlags {
    bool tracked;
    bool destructor;
    bool borrowed;
    bool fromarena;
    bool marked;
    bool pinned;
    bool freed;
    bool nologs;
    bool notrack;
} memflags_t;

typedef enum MemoryConstructor {
    MEM_CTOR_SUCCESS = 0,
    MEM_CTOR_ONFREE,
    MEM_CTOR_ONRESET,
    MEM_CTOR_ONSWEEP
} memctor_t;

typedef enum MemoryDestructor {
    MEM_DTOR_NEVER = 0,
    MEM_DTOR_ONFREE,
    MEM_DTOR_ONRESET,
    MEM_DTOR_ONSWEEP
} memdtor_t;

// counters, checks, and other general stats
typedef struct MemoryStatistics { 

    uint64_t peak;    // peak memory usage

    /* how many ... ? */
    size_t frees;
    size_t allocs;
    size_t reallocs;
    size_t livebytes;
    size_t blocks;
    size_t pages;
    size_t fails;
    uint64_t leakedbytes;

    /* did we ... ? */
    bool leak;
    bool fail;
} memstat_t;


typedef enum MemoryRecordFlags{
    MEMREC_NONE            = 0u,
    MEMREC_TRACKED         = 1u << 0,  /* has a live registry record */
    MEMREC_FREED           = 1u << 1,  /* record represents freed block */
    MEMREC_HAS_DTOR        = 1u << 2,  /* destructor callback is set */
    MEMREC_BORROWED        = 1u << 3,  /* memory not owned by this ctx */
    MEMREC_EXTERNAL        = 1u << 4,  /* externally allocated/registered */
    MEMREC_ARENA           = 1u << 5,  /* allocated from arena/region */
    MEMREC_PINNED          = 1u << 6,  /* exempt from relocation/GC ideas */
    MEMREC_MARKED          = 1u << 7,  /* future mark/sweep support */
    MEMREC_ROOT            = 1u << 8,  /* considered a GC root later */
    MEMREC_OWNS_CHILDREN   = 1u << 9,  /* object owns nested allocations */
    MEMREC_ZEROED          = 1u << 10, /* memory contents were zeroed */
    MEMREC_REALLOCATED     = 1u << 11, /* block has been realloc'd */
    MEMREC_NO_FREE         = 1u << 12, /* registry may track, but not free */
    MEMREC_NO_DTOR         = 1u << 13, /* destructor explicitly disabled */
    MEMREC_IMMORTAL        = 1u << 14, /* ignore normal release rules */
    MEMREC_USER0           = 1u << 15  /* reserved experiment bit */
} memrec_flags;

typedef unsigned int jmem_rec_flags_t;

 // metadata record for one memory allocation {what do we know about the operation?}
typedef struct MemoryRecord {
    /*
        leak detection
        ownership metadata
        destructor callbacks
        tracking realloc history
        “who allocated this?”
        “is this block already freed?”
        future GC experiments
        registry dumps / debug reports
    */
   memrec_flags flags;
   memloc_t src;
   memop_t oper;
   uint64_t size;
} memrec_t;

// how should this object clean up behind itself or others
typedef struct MemoryPointer {
    memrec_t record;
    memloc_t oldloc;
    void *buffer;
    bool collect;
} memptr_t; // callback to cleanup the unkown, allocated 'object'


typedef unsigned int jmem_cfg_flags_t;

enum {
    JMEM_CFG_NONE            = 0u,

    JMEM_CFG_INIT_TRACK      = 1u << 0,
    JMEM_CFG_INIT_LOG        = 1u << 1,
    JMEM_CFG_INIT_CAPTURE    = 1u << 2,
    JMEM_CFG_INIT_STRICT     = 1u << 3,
    JMEM_CFG_INIT_ABORT_OOM  = 1u << 4,
    JMEM_CFG_INIT_AUTO_DTOR  = 1u << 5
};


// what are the choices, options, settings, etc. ?
typedef struct MemoryConfig {
    memflags_t options;   // master set of options as reference 
    memrec_t mirror;      // memory record for "reflection" or mimicry ...
    bool threaded;        // thread safety option
    bool harikiri;        // abort on alloc failure, cleanup included
} memcfg_t;



typedef enum MemoryContextFlags {
    MEMCTX_NONE            = 0u,
    MEMCTX_TRACK           = 1u << 0,  /* maintain allocation registry */
    MEMCTX_CAPTURE         = 1u << 1,  /* record file/line/func info */
    MEMCTX_LOG             = 1u << 2,  /* emit basic memory logs */
    MEMCTX_ABORT           = 1u << 3,  /* abort on allocation failure */
    MEMCTX_STRICT          = 1u << 4,  /* enforce strict validation */
    MEMCTX_AUTO_DTOR       = 1u << 5,  /* call destructor on free */
    MEMCTX_LOG_ALLOC       = 1u << 6,  /* log alloc/calloc */
    MEMCTX_LOG_REALLOC     = 1u << 7,  /* log realloc */
    MEMCTX_LOG_FREE        = 1u << 8,  /* log free */
    MEMCTX_LOG_FAIL        = 1u << 9,  /* log failures/warnings */
    MEMCTX_ALLOW_UNTRACK   = 1u << 10, /* permit ops on untracked ptrs */
    MEMCTX_ZERO_ON_ALLOC   = 1u << 11, /* zero memory after alloc */
    MEMCTX_ZERO_ON_FREE    = 1u << 12, /* scrub memory before free */
    MEMCTX_USE_DTORS       = 1u << 13, /* destructor system enabled */
    MEMCTX_LEAK_REPORT     = 1u << 14, /* dump leaks on shutdown */
    MEMCTX_INTERNAL_DEBUG  = 1u << 15  /* extra assertions/diagnostics */
};

typedef unsigned int memctx_flags;

// which memory system is managing this?
typedef struct MemoryContext {
    map_t map; // custom map (key=value pairs; key == pointer as index)
    memcfg_t config;
    memflags_t flags;
    size_t rec_count;
    memrec_t *record;
    memstat_t stats;
    memptr_t oldptr;
    memop_t oper;

    void *(*malloc_fn)(size_t);
    void *(*calloc_fn)(size_t, size_t);
    void *(*realloc_fn)(void *, size_t);
    void  (*free_fn)(void *);

    FILE *fstream;
} memctx_t; // context manager

typedef struct MemoryRequest {
    memctx_t context;
    time_t epoch;
    logger_t logger;
    uint32_t mask;
} memreq_t; // central dispatcher of contexts

typedef struct GarbageManager {
    const error_t lasterr;
    size_t threshold;
    memreq_t *memory;
    stack_t graystk;
    error_t error;
    chrono_t time;
    size_t cursor;
    time_t epoch;
    void *abyss;

    
} garbageman;

typedef enum jmem_err {
    JMEM_ERR_NONE = 0,

    JMEM_ERR_OOM,              /* out of memory */
    JMEM_ERR_NULL_CTX,         /* null context passed */
    JMEM_ERR_NULL_PTR,         /* null pointer where invalid */
    JMEM_ERR_SIZE_ZERO,        /* zero-size alloc if treated as error */
    JMEM_ERR_NOT_FOUND,        /* pointer not found in registry */
    JMEM_ERR_DOUBLE_FREE,      /* already freed */
    JMEM_ERR_INVALID_FREE,     /* free on invalid/untracked pointer */
    JMEM_ERR_INVALID_REALLOC,  /* realloc on invalid/untracked pointer */
    JMEM_ERR_DTOR_FAILED,      /* destructor reported failure, if supported */
    JMEM_ERR_BAD_FLAGS,        /* invalid/unsupported flag combination */
    JMEM_ERR_INTERNAL          /* jmem bookkeeping error */
} jmem_err_t;

typedef enum jmem_status {
    JMEM_STATUS_OK = 0,
    JMEM_STATUS_NOOP,
    JMEM_STATUS_WARN,
    JMEM_STATUS_ERR
} jmem_status_t;

typedef enum jmem_registry_kind {
    JMEM_REGISTRY_NONE = 0,
    JMEM_REGISTRY_LIST,
    JMEM_REGISTRY_VECTOR,
    JMEM_REGISTRY_HASH
} jmem_registry_kind_t;

typedef enum jmem_ownership {
    JMEM_OWN_NONE = 0,     /* unspecified */
    JMEM_OWN_FULL,         /* jmem owns and frees it */
    JMEM_OWN_BORROWED,     /* tracked but not freed */
    JMEM_OWN_SHARED,       /* shared / refcount territory */
    JMEM_OWN_EXTERNAL      /* managed elsewhere */
} jmem_ownership_t;


#endif /* JMEMCTRL_H */
#define JMEMCTRL_IMPL // #debug-mode
#ifdef JMEMCTRL_IMPL



#endif /* JMEMCTRL_IMPL */