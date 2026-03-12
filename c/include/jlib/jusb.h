#ifndef JUSB_H
#define JUSB_H

#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uint64_t */

/* extra bytes to budget for overhead (default 10 MiB) */
#ifndef BUFFER
#define BUFFER (10ULL * 1024ULL * 1024ULL)
#endif


/* 
    Return pointer to internal table of common USB sizes (bytes).
        -- out_count may be NULL. 
*/
const uint64_t *usb_sizes(size_t *count);

/*
    Given required bytes, return smallest supported USB size (bytes).
        -- returns 0 if no supported size is large enough.
*/
uint64_t usb_minsize(uint64_t bytes);

/* Same, but includes a buffer/overhead amount. */
uint64_t usb_minsize_buff(uint64_t bytes, uint64_t buffer);

#ifdef JUSB_IMPL

static const uint64_t usb_table[] = {
    512ULL * 1024ULL * 1024ULL,        /* 512MB */
    1ULL   * 1024ULL * 1024ULL * 1024ULL,
    2ULL   * 1024ULL * 1024ULL * 1024ULL,
    4ULL   * 1024ULL * 1024ULL * 1024ULL,
    8ULL   * 1024ULL * 1024ULL * 1024ULL,
    16ULL  * 1024ULL * 1024ULL * 1024ULL,
    32ULL  * 1024ULL * 1024ULL * 1024ULL,
    64ULL  * 1024ULL * 1024ULL * 1024ULL
    /* add more if needed */
};

static inline const uint64_t *usb_sizes(size_t *count){
    if (count){
        *count = sizeof(usb_table) / sizeof(usb_table[0]);
    }
    return usb_table;
}

static inline uint64_t usb_minsize(uint64_t bytes){
    size_t n = 0;
    const uint64_t *sizes = usb_sizes(&n);
    for (size_t i = 0; i < n;  ++i){
        if (bytes <= sizes[i]) return sizes[i];
    }
    return 0; /* not found */
}

static inline uint64_t usb_minsize_buff(uint64_t bytes, uint64_t buffer){
    return usb_minsize(bytes + buffer);
}


#endif /* JUSB_IMPL */
#endif /* JUSB_H */