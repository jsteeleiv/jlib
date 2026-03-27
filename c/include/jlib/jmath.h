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

} vec_t;

typedef struct Matrix {

} mat_t;

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

}

static inline bool set_del(set_t *s, Jtype val){
    
}

static inline bool set_rsz(set_t *s, size_t cap){
    
}

static inline bool set_has(set_t *s, Jtype val){
    if (!s) return false;
    for (size_t i = 0; i < s->size; i++){
        if (s->data[i] == val) return true;
    }
    return false;
}

static inline void set_print(void){

}

static inline size_t set_size(void){

}


#endif /* JMATH_IMPL */