#pragma once
#ifndef JFUT_H
#define JFUT_H

#include "../jstd/jthrd.h"
#include "../jstd/jerr.h"
    
typedef enum FutureStatus {
    FUT_PENDING = 0,
    FUT_READY,
    FUT_FAILED
} future_st;

typedef enum FutureError {
    FUT_OK = 0,                  // success, value is avaliable
    FUT_ERR_INVALID = -1,        // bad pointer, handle, or misuse
    FUT_ERR_SATISFIED = -2,      // promise already completed once
    FUT_ERR_BROKEN_PROMISE = -3, // promise killed before fulfilled
    FUT_ERR_FALURE = -4          // promise explicitly completed w/ errors
} future_err;

typedef struct SharedState {
    mutex_t lock;
    condition_t ready;
    future_st status; 
    void *value;
    char *error;
    int refcount;
    int promise_broken;
} shrstate_t;

static inline shrstate_t *state_create(void);
static inline void state_release(shrstate_t *state);

typedef struct PromiseControl {
    mutex_t lock;
    condition_t idle;
    int refcount;     // references to this specific control object
    int activeops;    // number of "in-flight" API calls using this handle
    int killing;      // kill confirmed
    shrstate_t *state;
} promisectrl_t;

typedef struct Promise {
    promisectrl_t *ctrl;
    error_t *error;
} promise_t;

static inline int promise_set(promise_t *p, void *value);
static inline int promise_kill(promise_t *p);

typedef struct FutureControl {
    mutex_t lock;
    condition_t idle;
    int refcount;     // references to this specific control object
    int activeops;    // number of "in-flight" API calls using this handle
    int killing;      // kill confirmed
    shrstate_t *state;
} futurectrl_t;

typedef struct Future {
    futurectrl_t *ctrl;
    error_t *error;
} future_t;

static inline void future_init(future_t *f);
static inline void *future_get(future_t *f, void **val, void **err);
static inline void future_kill(future_t *f);

typedef struct CoRoutine {

} corout_t;

#endif /* JFUT_H */
#define JFUT_IMPL // #debug-mode
#ifdef JFUT_IMPL


/* Example ?
----------
    promise_t ---> shared_state_t <--- future_t

    When you create the pair ...
       shared_state_t *state = malloc(sizeof(*state));
        if (state == NULL) {
            return -1;
        }

        mutex_init(&state->lock);
        cond_init(&state->ready);

        state->status = FUT_PENDING;
        state->value = NULL;
        state->error = NULL;
        state->refcount = 2; // 1 for the promise one for the future

    Then
        promise_t p = { .state = state };
        future_t  f = { .state = state };
*/

static inline int promise_set(promise_t *p, void *value){
    if (p == NULL || p->ctrl->state == NULL) return FUT_ERR_INVALID;

    shrstate_t *state = p->ctrl->state;
    mutex_lock(&state->lock);

    // Prevent double fulfillment
    if (state->status != FUT_PENDING) {
        mutex_unlock(&state->lock);
        return FUT_ERR_SATISFIED;
    }
    state->value = value;
    state->status = FUT_READY;

    // Wake all waiting futures
    cond_brcast(&state->ready);
    mutex_unlock(&state->lock);
    return FUT_OK;
}

static inline int promise_errset(promise_t *p, char *err){
    if (p == NULL || p->ctrl->state == NULL) return FUT_ERR_INVALID;

    shrstate_t *state = p->ctrl->state;
    mutex_lock(&state->lock);
    // Prevent double fulfillment
    if (state->status != FUT_PENDING) {
        mutex_unlock(&state->lock);
        return FUT_ERR_SATISFIED;
    }

    state->error = err;
    state->promise_broken = 0;
    state->status = FUT_FAILED;

    cond_broadcast(&state->ready);
    mutex_unlock(&state->lock);
}

static inline int promise_kill(promise_t *p){
    if (p == NULL || p->ctrl->state == NULL) return FUT_ERR_INVALID;
    shrstate_t *state = p->ctrl->state;
    mutex_lock(&state->lock);
    if (state->status == FUT_PENDING){
        state->status = FUT_FAILED;
        state->error = "broken promise 💔";
        cond_broadcast(&state->ready);
    }
    mutex_unlock(&state->lock);
    state_release(p->ctrl->state);
    p->ctrl->state = NULL;
    return FUT_OK;
}

static inline void future_init(future_t *f){
    if (!f) return;
    f->ctrl->state->status = FUT_PENDING;
    f->ctrl->state->ready.valid = true;
}

static inline int future_status(future_t *f, future_st *st){
    if (f == NULL || f->ctrl->state == NULL || st == NULL) return FUT_ERR_INVALID;

    shrstate_t *state = f->ctrl->state;
    mutex_lock(&state->lock);
    *st = state->status;
    mutex_unlock(&state->lock);
    return FUT_OK;
}

static void *future_get(future_t *f, void **val, void **err){
    if (f == NULL || f->ctrl->state == NULL) return NULL;
    if (val != NULL) *val = NULL;
    if (err != NULL) *err = NULL;

    shrstate_t *state = f->ctrl->state;
    // if (state_acquire(state) != 0) return NULL;

    mutex_lock(&state->lock);
    while (state->status == FUT_PENDING) {
        cond_wait(&state->ready, &state->lock);
    }

    if (state->status == FUT_READY){
        if (val != NULL) *val = state->value;
        mutex_unlock(&state->lock);
        return FUT_OK;
    }

    if (state->status == FUT_FAILED){
        if (err != NULL) *err = state->error;

        int ret = state->promise_broken ? FUT_ERR_BROKEN_PROMISE : FUT_ERR_FALURE;
        mutex_unlock(&state->lock);
        return ret;
    }

    mutex_unlock(&state->lock);
    return FUT_ERR_INVALID;
    // void *result = state->value;
    // mutex_unlock(&state->lock);
    // state_release(state);
    // return result;
}

static inline void future_kill(future_t *f){
    if (f == NULL || f->ctrl->state == NULL) return;
    state_release(f->ctrl->state);
    f->ctrl->state = NULL;
}

static inline shrstate_t *state_create(void){
    shrstate_t *state = malloc(sizeof(*state));
    if (state == NULL) return NULL;

    if (mutex_init(&state->lock) != 0) {
        free(state); return NULL;
    }
    if (cond_init(&state->ready) != 0) {
        mutex_destroy(&state->lock);
        free(state); return NULL;
    }

    state->status = FUT_PENDING;
    state->value = NULL;
    state->error = NULL;
    state->refcount = 2;
    state->promise_broken = 0;
    return state;
}

static inline int state_acquire(shrstate_t *state){
    if (state == NULL) return -1;
    mutex_lock(&state->lock);

    if (state->refcount <= 0) {
        mutex_unlock(&state->lock); return -1;
    }
    state->refcount++;
    mutex_unlock(&state->lock);
    return 0;
}

static inline void state_release(shrstate_t *state){
    if (state == NULL) return;

    int destroy = 0;
    mutex_lock(&state->lock);
    state->refcount--;
    if (state->refcount == 0) destroy = 1;

    mutex_unlock(&state->lock);
    if (destroy) {
        cond_destroy(&state->ready);
        mutex_destroy(&state->lock);
        free(state);
    }
}

#endif /* JFUT_IMPL */
