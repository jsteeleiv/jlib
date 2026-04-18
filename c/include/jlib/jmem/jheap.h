#pragma once
#ifndef JHEAP_H
#define JHEAP_H

#include <stdio.h>
#include <stdbool.h>

#define HEAP_INIT_CAP 69420 
#define HEAP_ALLOC_CAP 4096
#define CHUNK_INIT_CAP 1024
#define CHUNK_ALLOC_CAP 1024

typedef struct HeapChunk {
    uintptr_t addr;
    char *data[CHUNK_INIT_CAP];
    void *start;
    size_t size;
} hchunk_t;

/* initialize a chunk for usage*/
static inline hchunk_t chunk_init(size_t sz);
/* allocate a new chunk on a heap, return pointer to addr */
static inline void *chunk_alloc(heap_t *heap, size_t sz);
/* free a chunk in a heap */
static inline void chunk_free(hchunk_t *ch);


typedef struct Heap {
    heapctrl_t ctrl;
    hchunk_t *chunks;
    size_t size;
    size_t tail;
} heap_t;

/* initialize a new heap */
static inline heap_t heap_init(size_t sz);
/* allocate a heap to a new location in memory */
static inline void *heap_alloc(heap_t *h, size_t sz, long long here);
/* collect the contents of the heap */
static inline void heap_collect(heap_t *h);
/* defragment the heap to ensure efficient reusage */
static inline void heap_defrag(heap_t *h);
/* free the heap */
static inline void heap_free(heap_t *h);

typedef struct HeapControl {
    hchunk_t heapallocs[HEAP_ALLOC_CAP];
    heap_t *heaps;
} heapctrl_t;

#endif /* JHEAP_H */
#define JHEAP_IMPL // #debug-mode
#ifdef JHEAP_IMPL

static inline hchunk_t chunk_init(size_t sz){
    hchunk_t chunk = {
        .start = 0, .size = sz
    };
    for (int i = 0; i < sz && i + sz < HEAP_INIT_CAP; ++i){
        chunk.data[i] = 0;
    }
    return chunk;
}

static inline void *chunk_alloc(heap_t *heap, size_t sz){
    if ((heap->size + sz) >= HEAP_INIT_CAP) return;
    
    hchunk_t ch = chunk_init(sz);
    void *ret = heap + ch.size;

    

}

hchunk_t heap_alloced[HEAP_ALLOC_CAP];
size_t heap_size = 0;

static inline heap_t heap_init(size_t sz){
    heap_t heap;
    heap.size = sz;
    // heap.chunk.arr = {0};
    // char heap[HEAP_CAP] = {0};
    // size_t heap_size = 0;
}

static inline void heap_alloc(size_t sz){

}

#endif /* JHEAP_IMPL */