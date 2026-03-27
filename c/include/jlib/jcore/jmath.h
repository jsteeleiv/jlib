#pragma once
#ifndef JMATH_H
#define JMATH_H

#ifndef SET_CAP
#define SET_CAP 8
#endif

#ifndef VEC_CAP
#define VEC_CAP 8
#endif

#ifndef MAT_CAP
#define MAT_CAP 8
#endif

#ifndef COORD_CAP
#define COORD_CAP 8
#endif

#include <jlib/jlib.h>

typedef struct Set {
    Jtype *data;
    size_t size;
    size_t cap;
} set_t;

typedef struct Vector {

} vector_t;

typedef struct Matrix {

} matrix_t;

typedef struct Rational {

} rational_t;

typedef struct Polynomial {

} polynomial_t;

typedef struct Complex {

} complex_t;

typedef struct Coordinate {

} coord_t;

/* lifecycle */
set_t *set_mk(void);
void set_die(set_t *set);

/* basic operations */
static inline bool set_ins(set_t *set, Jtype val);
static inline bool set_del(set_t *set, Jtype val);
static inline bool set_rsz(set_t *set, size_t cap);
static inline bool set_has(const set_t *set, Jtype val);
static inline void set_print(const set_t *set);
static inline size_t set_size(const set_t *set);

/* set algebra */
static inline set_t *set_diff(const set_t *a, const set_t *b);
static inline set_t *set_union(const set_t *a, const set_t *b);
static inline set_t *set_intersect(const set_t *a, const set_t *b);


#endif /* JMATH_H */

#ifdef JMATH_IMPL

static inline bool set_mk(void){
    set_t *s = malloc(sizeof(set_t));
    if (!s) return NULL;
    s->data = malloc(SET_CAP * sizeof(Jtype));
    if (!s->data){
        free(s);
        return NULL;
    }
    s->size = 0;
    s->cap = SET_CAP;
    return s;
}

static inline void set_die(set_t *s){
    if (!s) return NULL;
    free(s->data);
    free(s);
}

static inline bool set_ins(set_t *s, Jtype val){
    if (!s) return false;
    if (set_has(s, val)) return false;
    if (s->size == s->cap){
        if (!set_rsz(s, s->cap * 2))return false
    }
    s->data[s->size++] = val;
    return true;
}

static inline bool set_del(set_t *s, Jtype val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i] == value){
            s->data[i] = s->data[s->size - 1];
            s->size--;
            return true;
        }
    }
    return false;
}

static inline bool set_rsz(set_t *s, size_t cap){
    Jtype *new = realloc(s->data, cap * sizeof(Jtype));
    if (!new) return false;
    s->data = new;
    s->cap = cap;
    return true;
}

static inline bool set_has(set_t *s, Jtype val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i] == val) return true;
    }
    return false;
}

static inline void set_print(const set_t *s){
    f (s == NULL) {
        printf("{ }\n");
        return;
    }

    printf("{ ");
    for (size_t i = 0; i < s->size; i++) {
        printf("%d", s->data[i]);
        if (i + 1 < s->size) {
            printf(", ");
        }
    }
    printf(" }\n");
}

static inline size_t set_size(set_t *s){
    return s ? s->size : 0;
}

static inline set_t *set_diff(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++){
        if (!set_has(b, a->data[i])) set_ins(c, a->data[i]);
    }
    return c;
}

static inline set_t *set_union(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++){
        if (!set_insert(result, a->data[i])) {
            /* duplicate is fine, allocation failure is harder to distinguish here */
        }
    }
    for (size_t i = 0; i < b->size; i++) {
        if (!set_insert(result, b->data[i])) {
            /* duplicate is fine */
        }
    }
    return c;
}

static inline set_t *set_intersect(const set_t *a, const set_t *b){
    if (!a || !b) return NULL;
    set_t *c = set_mk();
    if (!c) return NULL;
    for (size_t i = 0; i < a->size; i++) {
        if (set_has(b, a->data[i])) set_ins(result, a->data[i]);
    }
    return c;
}


#endif /* JMATH_IMPL */