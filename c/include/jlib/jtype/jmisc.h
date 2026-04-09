#pragma once
#ifndef JMISC_H
#define JMISC_H

#include "../jstd.h"
#include "../jdata/jvec.h"

typedef struct Information {
    vec_t *arrs;
    const char *mod;
    const char *ctx;
} info_t;

typedef struct Abyss {
    void *ether;
} abyss_t;

typedef struct Random{
    chrono_t time;
    void *abyss;
    int64_t seed;
    float fval;
    int ival;
} rand_t;

typedef struct Fate {

} fate_t;

typedef struct Quest {
    mirror_t reflection;
    bool reflect;
} quest_t;

typedef struct Mirror {
    char *was;
    char *is;
    fate_t fate;
    quest_t *quest;
} mirror_t;







typedef struct {
    Jerror err;
    Jtime time;
    value_t type;
    map_t stars;
    char *omen;
    char *read;
    bool omen;
} Oracle;





#endif /* JMISC_H */
#define JMISC_IMPL // debug-mode
#ifdef JMISC_IMPL



#endif /* JMISC_IMPL */