#pragma once
#ifndef JMAP_H
#define JMAP_H
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <stdint.h>
//#include <jlib/jdata/jvec.h>
//#include <jlib/jstd/jlog.h>

typedef struct MetaData {
    size_t len;
    size_t cap;
    void *hash_tbl;
    //ptrdiff_t tmp;
} meta_t;

typedef struct StringBlock {
    //struct strblock_t *next;
    char string[8];
} strblock_t;

typedef struct StringArena {
    strblock_t *string;
    size_t rest;
    unsigned char block;
    unsigned char mode; // not used by string arena itself (?)
}strarena_t;

typedef struct Bucket {
    //Jvec entries;
    size_t size;   // # of entries in bucket
    void *entries;
} bucket_t;

typedef struct HashIndex {
    char *tmpkey;  // must be first ???
    size_t slots;
    size_t used;
    size_t used_max;
    size_t used_shrink_max;
    size_t deaths;
    size_t deaths_max;
    size_t seed;
    size_t slotlog;
    strarena_t string;
    bucket_t *storage;
} hashidx_t;

typedef struct Entry {
    char *key;
    char *val;
    size_t hash;
} entry_t;


typedef struct Map {
    size_t size;
    size_t cap;    // max buckets
    bucket_t *bkts;
} map_t;

typedef struct {
    //Jerror err;
    //Jlog log;
    map_t *map;
} Jmap;




#endif /* JMAP_H */

#ifdef JMAP_IMPL



#endif /* JMAP_IMPL */