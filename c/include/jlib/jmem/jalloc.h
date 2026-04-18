#pragma once
#ifndef JALLOC_H
#define JALLOC_H


typedef enum AllocationKind {
    ALLOC_NONE = 0,
    ALLOC_MALLOC,
    ALLOC_CALLOC,
    ALLOC_REALLOC,
    ALLOC_ARENA,
    ALLOC_EXTERNAL,
    ALLOC_CUSTOM
} allockind_t;


#endif /* JALLOC_H */
#define JALLOC_IMPL // #debug-mode
#ifdef JALLOC_IMPL



#endif /* JALLOC_IMPL */
