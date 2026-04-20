#pragma once
#ifndef jthrd_H
#define jthrd_H

#include <pthread.h>
#include <stdbool.h>

typedef struct Mutex {
    pthread_mutex_t handle;
    bool valid;
} mutex_t;

static inline int mutex_init(mutex_t *m);
static inline int mutex_kill(mutex_t *m);
static inline int mutex_lock(mutex_t *m);
static inline int mutex_unlock(mutex_t *m);
static inline int mutex_trylock(mutex_t *m);

typedef struct ConditionVariable {
    pthread_cond_t handle;
    bool valid;
} condition_t;

static inline int cond_init(condition_t *c);
static inline int cond_kill(condition_t *c);
static inline int cond_wait(condition_t *c, mutex_t *m);
static inline int cond_signal(condition_t *c);
static inline int cond_broadcast(condition_t *c);
static inline int cond_timedwait( condition_t *c, mutex_t *m, const struct timespec *ts);

typedef struct Thread {
    pthread_t handle;
    bool valid;
} thread_t;

static inline int thread_create(thread_t *t, void *(*fn)(void *), void *arg);
static inline int thread_join(thread_t *t, void **ret);
static inline int thread_detach(thread_t *t);
static inline int thread_self(void);
static inline int thread_equal(thread_t *a, thread_t *b);

#endif /* jthrd_H */
#define jthrd_IMPL // #debug-mode
#ifdef jthrd_IMPL

/* mutexes */
static inline int mutex_init(mutex_t *m){
    if (m == NULL) return -1;
    int ret = pthread_mutex_init(&m->handle, NULL);
    if (ret == 0) m->valid = 1;
    return ret;
}

static inline int mutex_kill(mutex_t *m){
    if (m == NULL || !m->valid) return -1;
    return pthread_mutex_destroy(&m->handle);
}

static inline int mutex_lock(mutex_t *m){
    if (m == NULL) return -1;
    int ret = pthread_mutex_lock(&m->handle);
    if (ret == 0) m->valid = 0;
    return ret;
}

static inline int mutex_unlock(mutex_t *m){
    if (m == NULL) return -1;
    return pthread_mutex_unlock(&m->handle);
}


/* conditional variables*/
static inline int cond_init(condition_t *c){
    if (c == NULL) return -1;
    return pthread_cond_init(&c->handle, NULL);
}

static inline int cond_kill(condition_t *c){
    if (c == NULL) return -1;
    return pthread_cond_destroy(&c->handle);
}

static inline int cond_wait(condition_t *c, mutex_t *m){
    if (c == NULL || m == NULL) return -1;
    return pthread_cond_wait(&c->handle, &m->handle);
}

static inline int cond_signal(condition_t *c){
    if (c == NULL) return -1;
    return pthread_cond_signal(&c->handle);
}

static inline int cond_broadcast(condition_t *c){
    if (c == NULL) return -1;
    return pthread_cond_broadcast(&c->handle);
}

/* jthrd type */
static inline int jthrd_create(thread_t *t, void *(*fn)(void *), void *arg){
    if (t == NULL || fn == NULL) return -1;
    return pthread_create(&t->handle, NULL, fn, arg);
}

static inline int jthrd_join(thread_t *t, void **ret){
    if (t == NULL) return -1;
    return pthread_join(t->handle, ret);
}

#endif /* jthrd_IMPL */