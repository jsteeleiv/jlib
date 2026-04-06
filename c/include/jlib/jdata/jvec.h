#pragma once
#ifndef JVEC_H
#define JVEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jlib/jstd/jerr.h>



// macro used to 'capture' the name of the variable (#v), for err message
#define rsz_vec(v) _rsz_vec(v, #v)
#define INIT_CAP 256

typedef struct Header {
    size_t count;
    size_t cap;
} header_t;

typedef struct DynamicArray {
    
} dynarr_t;

// `any` type achieved through macros
#define arr_len(arr) ((Header*)(arr) - 1)->count
#define arr_free(arr) free((Header*)(arr) - 1)
#define arr_push(arr, x)\
    do {\
    if (!arr) {\
        header_t *header = malloc(sizeof(*arr)*INIT_CAP + sizeof(header_t));\
        header->count = 0;\
        header->cap = INIT_CAP;\
        arr = (void*)(header + 1);\
    }\
    header_t *header = (header_t*)(arr) - 1;\
    if (header->count >= header->cap){\
        header->cap *= 2;\
        header = realloc(header, sizeof(*arr)*header->cap + sizeof(Header));\
        arr = (void*)(header + 1);\
    }\
    (arr)[header->count++] = (x);\
    } while (0)



// dynamic array<vector>
typedef struct Vector {
    uintptr_t addr;
    const char *type;    // type of items being stored
    void *items;         // the items themselves
    size_t size;         // size of ONE item (e.g., sizeof(type))
    size_t len;          // current amount of items in vector
    size_t cap;          // total capacity of vector
    size_t grew;         // amoutn it has grown since `birth`
    float rate;          // growth rate (e.g. how much to resize by)
    bool full;           // is it nearing it's capacity?
    bool grown;          // has it been resized? (addr relocated?)
    bool heap;           // Do we want to use the heap?
} vec_t;

typedef struct {
    Jerror err;
    vec_t vec;
} Jvec;


#endif /* JVEC_H */
#define JVEC_IMPL
#ifdef JVEC_IMPL

int *arr_init(size_t cap){
    header_t *header = malloc(sizeof(int)*cap + sizeof(header_t));
    header->count = 0;
    header->cap = cap;
    return (int*)(header + 1); // pointer arithmetic, uses size of type its pointing to
}

// #define arr_len(arr) ((Header*)(arr) - 1)->count
// size_t arr_len(int *arr){
//     return ((header_t*)arr - 1)->count;
// }


// is now a macro, defined above
// int arr_push(int *arr, int x){
//     if (!arr) return 1;
//     header_t *header = (header_t*)arr - 1;
//     arr[header->count++] = x;
// }

//Initialize the vector
void new_vec(Jvec *v, size_t size, size_t cap, float rate){
    v->vec.size = size;
    v->vec.cap = cap;
    v->vec.len = 0;
    v->vec.grew = 0;
    v->vec.rate = rate;
    v->vec.full = false;
    v->vec.grown = false;
    v->vec.heap = true;

    // allocate `amount` of items * bytes per item
    v->vec.items = malloc(v->vec.len * v->vec.size);
}

void _rsz_vec(Jvec *v, const char *name){
    // track the old pointer to see if addr is being moved
    uintptr_t old_ptr = v->vec.addr;
    // calculate new capacity depending on the desired rate of growth
    size_t new_cap = (v->vec.cap == 0) ? 1 : (size_t)v->vec.cap * v->vec.rate;
    // re-allocate the memory size, will move if no longer fits
    void *tmp = realloc(v->vec.items, new_cap * sizeof(v->vec.size));
    if (tmp == NULL){
        fprintf(stderr, "[error] jvec: Memory re-allocation failed! %s has not grown!\n", name);
    }

    // update variables
    v->vec.items = tmp;
    v->vec.cap = new_cap;
    v->vec.grew++;
    v->vec.grown = (v->vec.addr != old_ptr);
}

void del_vec(Jvec* v){
    if (v == NULL) return;
    if (v->vec.items != NULL){
        free(v->vec.items);
        v->vec.items = NULL; // prevents double free errors ... 
    }
    free(v);
}

void add_vec(Jvec *v, void *item){
    if (v->vec.len >=v->vec.cap){
        rsz_vec(v);
    }
    // cast to (char*) to do byte-level math: start + (index * size)
    void *dest = (char *)v->vec.items + (v->vec.len * v->vec.size);
    memcpy(dest, item, v->vec.size);
    v->vec.len++;
    v->vec.full = (v->vec.len >= v->vec.cap);
}

#endif /* JVEC_IMPL */