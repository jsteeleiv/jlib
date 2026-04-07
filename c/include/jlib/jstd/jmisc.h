#pragma once
#ifndef JMISC_H
#define JMISC_H

#include <jstd/jerr.h>

typedef struct Bit {
    uint8_t byte;
    unsigned int val : 1; // occupies 1 bit
} bit_t; // 2 bytes, even with the single bit `val`

typedef struct Information {
    error_t error;       // custom error type
    const char *mod;
    const char *ctx;
    vec_t *data;
} info_t;

typedef struct Random{
    Jtime time;
    void *abyss;
    int seed;
    float fval;
    int ival;
} rand_t;

typedef struct {
    Jerror err;
    Jtime time;
    value_t type;
    Jmap stars;
    char *omen;
    char *read;
    bool omen;
} Oracle;

#endif /* JMISC_H */

#ifdef JMISC_IMPL



#endif /* JMISC_IMPL */