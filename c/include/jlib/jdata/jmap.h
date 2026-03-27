#ifndef JMAP_H
#define JMAP_H

#include <jlib/jdata/jtype.h>
//#include <jlib/jcore/jlog.h>


typedef struct Entry {
    Jtype val;
    char *key;
    size_t hash;
} entry_t;

typedef struct Buckets {
    Jlist *entries;
    size_t size;        // amount of buckets in list
} bucket_t;

typedef struct Map {
    size_t size;
    size_t cap;        // max buckets
    bucket_t *buckets;
} map_t;

typedef struct {
    Jerror errs;
    ////Jlog logs;
    map_t *map;
} Jmap;




#endif /* JMAP_H */

#ifdef JMAP_IMPL



#endif /* JMAP_IMPL */