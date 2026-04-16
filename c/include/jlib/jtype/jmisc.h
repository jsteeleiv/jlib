#pragma once
#ifndef JMISC_H
#define JMISC_H

#include "jcpu.h"
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
} mirror_t;


typedef struct Turing {

} turing_t;

typedef struct DynamicProgramming {
    /* Dynamic Programming
    ----------------------
        a method for somving complex problems by breaking them down
            create simpler sub-problems and store results
        avoids redundant computations

        ex: knapsack problem, longest increasing subsequence, fibonacci sequence
    */

} dynprogging_t;

typedef struct GreedyAlgorithm {
    /* Dynamic Programming
    ----------------------
        make the LOCALLY optimal choice at each step with hope of finding GLOBAL optimum
        suitable for optimization problems where choosing best immediate option ...
            leads to overall optimal solution

        ex: interval scheduling, huffman coding (data compression, ...
            Prim's and Kruskal's algorithms for finding the minimum spanning tree) (?)
    */
} greedyalgo_t;
    
typedef struct BackTracker {
    /* Dynamic Programming
    ----------------------
        algorithmic technique for solving problems recursively
            tries to build a solutino incrementally and abandoning it if necessary

        ex: sudoku, N-Queens problem, generating permutations & combinations, ...
            constraint satisfaction problems
    */
} backtracker_t;


typedef struct {
    error_t err;
    tmstamp_t time;
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