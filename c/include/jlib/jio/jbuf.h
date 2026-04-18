#pragma once
#ifndef JBUF_H
#define JBUF_H

#include <stdio.h>

#include "../jtype/jval.h"
#include "../jstd/jerr.h"

/*  =========================
    BufferStatus
    ========================= */

typedef enum BufferStatus {
    BUF_OK = 0,
    BUF_ERR_GEN,
    BUF_ERR_ARGS,
    BUF_ERR_ALLOC,
    BUF_ERR_REALLOC,
    BUF_ERR_BOUNDS,
    BUF_ERR_OVERFLOW
}bufstat_t;

/*  =========================
    BitBuffer
    ========================= */

typedef struct BitReader {
    bufstat_t status;
    const uint8_t *data;
    size_t bytesize;
    size_t bitpos;
    char *msg;
} bitread_t;

typedef struct BitWriter {
    bufstat_t status;
    uint8_t *data;
    size_t bytecap;
    size_t bitpos;
    char *msg;
} bitwrit_t;

static inline bitwrit_t bitwrit_init(bitwrit_t *bw, size_t bytecap);
static inline void bitwrit_free(bitwrit_t *wb);
static inline void bitwrit_reset(bitwrit_t *bw);
static inline void bitwrit_set();

typedef struct BitBuffer {
    bufstat_t status;
    type_t *data;
    size_t head;
} bitbuf_t;

/*  =========================
    ByteArray
    ========================= */

typedef struct ByteArray {
    bufstat_t status;
    unsigned char *data;
    size_t length;
} bytearr_t;

static inline error_t readfile_bArray(const char *path);

/*  =========================
    RingBuffer
    ========================= */

typedef struct RingBuffer {
    // type_t *data;
    void *data;
    bufstat_t status;
    size_t head;
    size_t tail;
    size_t cap;
    size_t len;
    bool owns;
    char cursor;
} ringbuf_t;

static inline void ringbuf_init(ringbuf_t *rb, type_t *buf, size_t cap);
static inline bool ringbuf_alloc(ringbuf_t *rb, size_t cap);
static inline bool ringbuf_push(ringbuf_t *rb, type_t *val);
static inline bool ringbuf_pull(ringbuf_t *rb, type_t *val);
static inline bool ringbuf_peek(ringbuf_t *rb, type_t *val);
static inline void ringbuf_clear(ringbuf_t *rb);
static inline void ringbuf_free(ringbuf_t *rb);

/*  =========================
    GapBuffer
    [hello ][ GAP GAP GAP GAP ][world]
             ^
           cursor
    ========================= */

typedef struct GapBuffer {
    char *data;
    size_t cap;
    size_t beg;
    size_t end;
    char cursor;
} gapbuf_t;

size_t gapbuf_size(const gapbuf_t *gb);
size_t gapbuf_len(const gapbuf_t *gb);
size_t gapbuf_get_curs(const gapbuf_t *gb);
int gapbuf_set_curs(gapbuf_t *gb, size_t pos);
int gapbuf_init(gapbuf_t *gb, size_t cap);
void gapbuf_free(gapbuf_t *gb);
int gapbuf_ins(gapbuf_t *gb, type_t item);
int gapbuf_back(gapbuf_t *gb);
int gapbuf_del(gapbuf_t *gb);
int gapbuf_mvleft(gapbuf_t *gb);
int gapbuf_mvright(gapbuf_t *gb);
int gapbuf_get_item(const gapbuf_t *gb, size_t idx, type_t *item);
char *gapbuf_cstr(const gapbuf_t *gb);


#endif /*.JBUF_H */
#define JBUF_IMPL
#ifdef JBUF_IMPL

#include <stdlib.h>

/*  =========================
    ByteArray
    ========================= */

static inline error_t readfile_bArray(const char *path) {
    error_t jerr;
    FILE *f;
    bytearr_t arr;
    long file_sz;
    size_t bytes_read;
    unsigned char *tmpbuf;

    if (!path) {
        //JERR("No filepath provided", ERROR, LOG_ARGS, NULL_ARG, -1, false);
        return jerr;
    }

    f = fopen(path, "rb");
    if (!f) {
        //JERR("could not read file", ERROR, LOG_FILE, FILE_OPEN, -1, false);
        return jerr;
    }

    arr.data = NULL;
    arr.length = 0;

    // Read the file byte by byte and store it in the array
    unsigned char byte;
    while (fread(&byte, sizeof(byte), 1, f) == 1) {
        // Reallocate memory for the byte array and append the byte
        arr.data = (unsigned char*)realloc(arr.data, arr.length + 1);
        if (!arr.data) {
            //JERR("Could not allocate memory", ERROR, LOG_MEM, MEM_NULL, -1, false);
            fclose(f);
            return jerr;
        }
        arr.data[arr.length] = byte;
        arr.length++;
    }

    fclose(f);

    // Print the bytes stored in the struct
    printf("Bytes read from file (%s):\n", path);
    for (size_t i = 0; i < arr.length; i++) {
        printf("%02X ", arr.data[i]);  // Print bytes as hexadecimal
    }
    printf("\n");

    // Free the allocated memory for the byte array
    free(arr.data);
    //JERR("NONE", NONE, LOG_NONE, SUCCESS, 0, false);
    return jerr;

}

/*  =========================
    RingBuffer
    ========================= */

static inline void ringbuf_clear(ringbuf_t *rb){
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
}

static inline void ringbuf_init(ringbuf_t *rb, type_t *buf,  size_t cap){
    rb->data = buf;
    rb->cap = cap;
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
    rb->owns = false;
}

static inline bool ringbuf_alloc(ringbuf_t *rb, size_t cap){
    rb->data = (type_t *)malloc(sizeof(type_t) * cap);
    if (!rb) return false;
    rb->cap = cap;
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
    rb->owns = true;

    return true;
}
static inline void ringbuf_free(ringbuf_t *rb){
    if (rb->owns && rb->data) free(rb->data);
    rb->data = NULL;
    rb->cap = 0;
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
    rb->owns = 0;
}

// static inline bool ringbuf_push(ringbuf_t *rb, void *val){
//     if (rb->len == rb->cap) return false;
//     rb->data[rb->tail] = val;
//     rb->tail = (rb->tail + 1) & rb->cap;
//     rb->len++;
//     return true;
// }

// static inline bool ringbuf_pull(ringbuf_t *rb, void *val){
//     if (rb->len == 0) return false;
//     val = rb->data[rb->head];
//     rb->head = (rb->head + 1) & rb->cap;
//     rb->len--;
//     return true;
// }

// static inline bool ringbuf_peek(ringbuf_t *rb, void *val){
//     if (rb->len == 0) return false;
//     val = rb->data[rb->head];
//     return true;
// }

/*  =========================
    GapBuffer
    ========================= */

// data[gap_start++] = ch;
// gap_start--;

/*  =========================
    if (gb->gap_start > 0) {
        gb->data[--gb->gap_end] = gb->data[--gb->gap_start];
    }
    ========================= */


// useful helpers
// check current size of buffer
static inline size_t gapbuf_size(const gapbuf_t *gb){
    return gb->end - gb->beg;
}
// check amount of `items` cap-size=length
static inline size_t gapbuf_len(const gapbuf_t *gb){
    return gb->cap - gapbuf_size(gb);
}

#endif /* JBUF_IMPL */
