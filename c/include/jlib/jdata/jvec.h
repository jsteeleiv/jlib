#pragma once
#ifndef JVEC_H
#define JVEC_H

//#include <jlib/jdata/jtype.h>
#include <stdio.h>


// macro used to 'capture' the name of the variable (#v), for err message
#define rsz_vec(v) _rsz_vec(v, #v)

// dynamic array<vector>
typedef struct Vector {
    //Jerror errs;       // errors struct to help handle errno etc.
    void *data;      // pointer to the data
    size_t size;     // size of ONE element (e.g., sizeof(int))
    size_t len;      // current amount of items in list
    size_t cap;      // total capacity
    size_t grew;     // times it's been resized
    float rate;      // growth rate (e.g. how much to resize)
    bool full;       // nearing capacity?
    bool grown;      // has the address been relocated?
} vec_t;

#ifdef JVEC_IMPL

//Initialize the vector
void new_vec(vec_t *v, size_t size, size_t cap, float rate){
    v->size = size;
    v->cap = cap;
    v->len = 0;
    v->grew = 0;
    v->rate = rate;
    bool full = false;
    bool grown = false;
    bool error = false;

    // allocate `amount` of items * bytes per item
    v->data = malloc(v->cap * v->size);
}

void _rsz_vec(vec_t *v, const char *name){
    // track the old pointer to see if addr is being moved
    void* old_ptr = v->data;
    // calculate new capacity depending on the desired rate of growth
    size_t new_cap = (v->cap == 0) ? 1 : (size_t)v->cap * v->rate;
    // re-allocate the memory size, will move if no longer fits
    void *tmp = realloc(v->data, new_cap * sizeof(v->size));
    if (tmp == NULL){
        fprintf(stderr, "[error] jlib: Memory re-allocation failed! %s has not grown!\n", name);
    }

    // update variables
    v->data = tmp;
    v->cap = new_cap;
    v->grew++;
    v->grown = (v->data != old_ptr);
}

void del_vec(vec_t* v){
    if (v == NULL) return;
    if (v->data != NULL){
        free(v->data);
        v->data = NULL; // prevents double free errors ... 
    }
    free(v);
}

void add_vec(vec_t *v, void *item){
    if (v->len >=v->cap){
        rsz_vec(v);
    }
    // cast to (char*) to do byte-level math: start + (index * size)
    void *dest = (char *)v->data + (v->len * v->size);
    memcpy(dest, item, v->size);
    v->len++;
    v->full = (v->len >= v->cap);
}

#endif /* JVEC_IMPL */

#endif /* JVEC_H */