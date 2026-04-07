#pragma once
#ifndef JMAP_H
#define JMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct StringBlock {
    char string[8];
} strblock_t;

typedef struct MetaData {
    strblock_t *string;
    size_t len;
    size_t cap;
    void *hashtbl;
    ptrdiff_t tmp;
} meta_t;

typedef struct StringArena {
    strblock_t *string;
    size_t rest;
    unsigned char block;
    unsigned char mode; // not used by string arena itself (?)
}strarena_t;

typedef struct HashIndex {
    char *tmpkey;  // must be first ???
    size_t slots;
    size_t used;
    size_t usedmax;
    size_t usedshrinkmax;
    size_t deaths;
    size_t deathsmax;
    size_t seed;
    size_t slotlog;
    strarena_t string;
    meta_t meta;
} hashidx_t;

static inline void hashidx_clear(hashidx_t *idx);

typedef struct Entry {
    char *key;
    char *val;
    size_t hash;
} entry_t;

static inline entry_t entry_init(void);
static inline void entry_free(entry_t *entry);
static inline void entry_clear(entry_t *entry);

typedef struct Bucket {
    size_t size;   // # of entries in bucket
    entry_t *entries;
    hashidx_t hashidx;
} bucket_t;

static inline bucket_t bucket_init(void);
static inline void bucket_free(bucket_t *bucket);
static inline void bucket_clear(bucket_t *bucket);
static inline entry_t *bucket_find(bucket_t *bucket, const char *key, size_t hash);
static inline const entry_t *bucket_cfind(const bucket_t *bucket, const char *key, size_t hash);

typedef struct Map {
    size_t size;
    size_t cap;    // max buckets
    bucket_t *buckets;
} map_t;

/* FNV hash globals */
// 64 bit offset_basis = 0xCBF29CE4 84222325
#define OFFSET_BASIS 1469598103934665603ull
// 64 bit FNV_Prime = 2**40 + 2**8 + 0xB3 = 0x00000100 000001B3
#define FNV_PRIME 1099511628211ull

/* hashing / lookup / helpers */
static inline char *map_strdup(const char *src);
static inline size_t map_hash(const char *key); // FNV-1a inline
static inline size_t map_index(const map_t *map, const char *key);
static inline size_t map_index_hash(const map_t *map, size_t hash);

/* map lifecycle */
static inline map_t map_init(size_t cap);
static inline void map_free(map_t *map);
static inline void map_clear(map_t *map);
static inline bool map_ready(const map_t *map);

/* map alignment */
static inline bool map_ispow2(size_t x);
static inline size_t map_nextpow2(size_t x);

/* map operations */
static inline bool map_has(const map_t *map, const char *key);
static inline char *map_get(map_t *map, const char *key);
static inline const char *map_cget(const map_t *map, const char *key);
static inline int map_put(map_t *map, const char *key, const char *val);
static inline int map_grow(map_t *map, size_t new_cap);
static inline size_t map_index(const map_t *map, const char *key);
static inline int map_rem(map_t *map, const char *key);

typedef struct {
    /* something, something ... dark side ... */
} Jmap;

#endif /* JMAP_H */
// #define JMAP_IMPL // #debug-mode
#ifdef JMAP_IMPL

#include <string.h>

void hashidx_clear(hashidx_t *idx) {
    if (!idx) return;
    memset(idx, 0, sizeof(*idx));
}

static inline void entry_clear(entry_t *entry){ 
    // entry->key = NULL;
    // entry->val = NULL;
    // entry->hash = 0;
    if (!entry) return;
    memset(entry, 0, sizeof(*entry));
}

static inline entry_t entry_init(void){
    return ({ entry_t e; entry_clear(&e); e; });
}

static inline void entry_free(entry_t *entry){
    if (!entry) return;
    free(entry->key);
    free(entry->val);
    entry_clear(entry);
}

static inline void bucket_clear(bucket_t *bucket){ 
    bucket->entries = NULL;
    hashidx_clear(&bucket->hashidx);
    bucket->size = 0;
    
    
    ; 
}

static inline bucket_t bucket_init(void){
    return ({ bucket_t b; bucket_clear(&b); b; });
}

static inline void bucket_free(bucket_t *bucket){
    if (!bucket) return;
    for (size_t i = 0; i < bucket->size; ++i){
        entry_free(&bucket->entries[i]);
    }
    free(bucket->entries);
    bucket_clear(bucket);
}

static inline entry_t *bucket_find(bucket_t *bucket, const char *key, size_t hash){
    if (!bucket || !key || !bucket->entries) return NULL;
    for (size_t i = 0; i < bucket->size; i++){
        if (bucket->entries[i].hash != hash) continue;

        if (strcmp(bucket->entries[i].key, key) == 0)
            return &bucket->entries[i];
    }
    return NULL;
}

static inline const entry_t *bucket_cfind(const bucket_t *bucket, const char *key, size_t hash){
    if (!bucket || !key || !bucket->entries) return NULL;

    for (size_t i = 0; i < bucket->size; i++){
        if (bucket->entries[i].hash != hash) continue;

        if (strcmp(bucket->entries[i].key, key) == 0)
            return &bucket->entries[i];
    }
    return NULL;
}

/*
    where I learned all about these bit tricks ... well mostly, at least
    https://graphics.stanford.edu/~seander/bithacks.html
*/

/* == zero only if exactly 1 bit set | x & (x - 1) clears lowest set bit */
static inline bool map_ispow2(size_t x){ return x && !(x & (x - 1)); }

static inline size_t map_nextpow2(size_t x){
    if (x == 0) return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
#if SIZE_MAX > UINT32_MAX
    x |= x >> 32;
#endif
    x++; return x;
}

static inline char *map_strdup(const char *src){
    if (!src) return NULL;
    size_t len;
    char *dst;

    len = strlen(src) + 1;
    dst = (char *)malloc(len);
    if (!dst)  return NULL;

    memcpy(dst, src, len);
    return dst;
}

static inline void map_clear(map_t *map){ 
    if (!map) return;
    memset(map, 0, sizeof(*map));
}

static inline map_t map_init(size_t cap){
    map_t map = {0};
    map.cap = (cap == 0 ? 16 : cap);

    map.buckets = (bucket_t*)calloc(map.cap, sizeof(bucket_t));
    if (!map.buckets) return map;

    for (size_t i = 0; i < map.cap; i++)
        map.buckets[i] = bucket_init();

    return map;
}

static inline void map_free(map_t *map){
    if (!map) return;

    for (size_t i = 0; i < map->cap; i++)
        bucket_free(&map->buckets[i]);

    free(map->buckets);
    map_clear(map);
}

static inline bool map_ready(const map_t *map){
    return map && map->buckets && map->cap > 0;
}


static inline size_t map_hash(const char *key){
/*  FNV Non-Cryptographic Hash Algorithm 
    https://www.ietf.org/archive/id/draft-eastlake-fnv-25.html

    This document focuses on the FNV-1a function whose pseudo-code is as follows:

    hash = offset_basis
    for each octet_of_data to be hashed
        hash = hash xor octet_of_data
        hash = hash * FNV_Prime
    return hash

    the FNV hash will be stored in an array hash[N] with N bytes;
        such that its integer value can be retrieved as follows:

    unsigned char hash[N];
    for ( i = N-1, value = 0; i >= 0; --i )
        value = ( value << 8 ) + hash[i];

    -----------------------------------------------------------------
    do I understand it fully ... ? NO
    can I follow the steps given the provided information? ABSOLUTELY
*/
    if (!key) return 0;
    const unsigned char *p = (const unsigned char *)key;

    size_t hash = (size_t)OFFSET_BASIS; // hash = offset_basis
    // size_t hash = (size_t)1469598103934665603ull;

    while (*p){
        hash ^= (size_t)(*p++);    // hash = hash xor octet_of_data
        hash *= (size_t)FNV_PRIME; // hash = hash * FNV_Prime
        // hash *= (size_t)1099511628211ull;
    }
    return hash;
}

static inline size_t map_index_hash(const map_t *map, size_t hash){
    if (!map_ready(map)) return 0;

    return hash & (map->cap - 1);
}

static inline size_t map_index(const map_t *map, const char *key){
    if (!map_ready(map) || !key) return 0;

    return map_index_hash(map, map_hash(key));

    // size_t hash = map_hash(key);
    // return hash & (map->cap - 1);
}

static inline const char *map_cget(const map_t *map, const char *key){
    const bucket_t *bucket;
    const entry_t *entry;
    size_t hash;
    size_t idx;

    if (!map_ready(map) || !key) return NULL;

    hash = map_hash(key);
    idx = map_index_hash(map, hash);
    bucket = &map->buckets[idx];

    entry = bucket_cfind(bucket, key, hash);
    return entry ? entry->val : NULL;
}

static inline bool map_has(const map_t *map, const char *key){
    return map_cget(map, key) != NULL;
}

static inline char *map_get(map_t *map, const char *key){
    bucket_t *bucket;
    entry_t *entry;
    size_t hash, idx;

    if (!map_ready(map) || !key) return NULL;

    hash = map_hash(key);
    idx  = map_index_hash(map, hash);
    bucket = &map->buckets[idx];

    entry = bucket_find(bucket, key, hash);
    return entry ? entry->val : NULL;
}

static inline int map_put(map_t *map, const char *key, const char *val){
    if (!map_ready(map) || !key || !val) return -1;

    bucket_t *bucket;
    entry_t *entry;
    entry_t *grown;
    char *newkey;
    char *newval;
    size_t hash, idx;

    if (map->size >= (map->cap * 213) >> 8){    // 83% threshold (0.83203125)
        if (map_grow(map, map->cap << 2) != 0)  // multiply (grow) by 4
            return -1;
    }

    hash = map_hash(key);
    idx  = map_index_hash(map, hash);
    bucket = &map->buckets[idx];

    entry = bucket_find(bucket, key, hash);
    if (entry){
        newval = map_strdup(val); // if allocation fails, old value is saved.
        if (!newval) return -1;

        free(entry->val);
        entry->val = newval;
        return 0;
    }

    grown = (entry_t *)realloc(
                bucket->entries, (bucket->size + 1) * sizeof *grown
    );

    if (!grown) return -1;

    bucket->entries = grown;
    entry = &bucket->entries[bucket->size];
    *entry = entry_init();

    newkey = map_strdup(key);
    if (!newkey) return -1;

    newval = map_strdup(val);
    if (!newval){ free(newkey); return -1; }

    entry->key  = newkey;
    entry->val  = newval;
    entry->hash = hash;
    bucket->size++;
    map->size++;

    return 0;
}

static inline int map_grow(map_t *map, size_t newcap){
    if (!map_ready(map)) return -1;

    newcap = (newcap == 0 ? 16 : map_nextpow2(newcap));
    if (newcap <= map->cap) return 0;

    map_t newer = map_init(newcap);
    if (!map_ready(&newer)) return -1;

    for (size_t i = 0; i < map->cap; i++){
        bucket_t *oldbucket = &map->buckets[i];

        for (size_t j = 0; j < oldbucket->size; j++){
            entry_t *src = &oldbucket->entries[j];
            size_t idx = map_index_hash(&newer, src->hash);
            bucket_t *dstbucket = &newer.buckets[idx];
            entry_t *grown;
            entry_t *dst;

            grown = (entry_t *)realloc(
                dstbucket->entries, (dstbucket->size + 1) * sizeof *grown
            );

            if (!grown){ map_free(&newer); return -1; }

            dstbucket->entries = grown;
            dst = &dstbucket->entries[dstbucket->size];
            *dst = entry_init();

            /* move ownership (pointer exchange), not duplicating */
            dst->key  = src->key;
            dst->val  = src->val;
            dst->hash = src->hash;

            dstbucket->size++;
            newer.size++;

            /* clear old slot so old cleanup won't free moved pointers */
            src->key  = NULL;
            src->val  = NULL;
            src->hash = 0;
        }
    }
    map_free(map);
    *map = newer;

    return 0;
}

static inline int map_del(map_t *map, const char *key){
    if (!map_ready(map) || !key) return -1;

    size_t hash = map_hash(key);
    size_t idx  = map_index_hash(map, hash);
    bucket_t *bucket = &map->buckets[idx];
    entry_t *entries = bucket->entries;

    if (!entries) return -1;

    for (size_t i = 0; i < bucket->size; i++){
        if (entries[i].hash != hash) continue;

        if (strcmp(entries[i].key, key) == 0){
            /* free the entry */
            entry_free(&entries[i]);

            /* shift remaining entries down */
            if (i + 1 < bucket->size){
                memmove( &entries[i], &entries[i + 1],
                    (bucket->size - i - 1) * sizeof(entry_t));
            }

            bucket->size--;
            map->size--;

            /* optional: shrink allocation */
            if (bucket->size == 0){
                free(bucket->entries);
                bucket->entries = NULL;
            } else {
                entry_t *shrunk = (entry_t *)realloc(
                    bucket->entries,
                    bucket->size * sizeof(entry_t)
                );

                if (shrunk) bucket->entries = shrunk;
            }
            return 0;
        }
    }
    return -1;
}
#endif /* JMAP_IMPL */
