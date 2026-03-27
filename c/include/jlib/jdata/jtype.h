#pragma once
#ifndef JTYPE_H
#define JTYPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <jlib/jcore/jtime.h>
#include <jlib/jcore/jerr.h>
#include <jlib/jdata/jstk.h>
#include <jlib/jdata/jlist.h>
//#include <jlib/jdata/jmap.h>


/* types */
typedef enum Kind {
    NONE = 0,
    /* basic */
    BOOL,
    /* signed integers */
    I8, I16, I32, I64,
    /* unsigned integers */
    U8, U16, U32, U64,
    /* floating point */
    F32, F64,
    /* string types */
    CSTR, STR,
    /* generic pointers */
    PTR, CONST_PTR,
    /* advanced */
    STRUCT, ARRAY, FUNC, 
    /* custom types */
    ANY, VALUE, XPRESSN,
    /* fallback */
    OPAQUE_T, NULL_T
} kind_t;

typedef struct Bitset {
    size_t bits;

} bitset_t;

typedef struct Value {
    /* data */

} value_t;

typedef struct Random{
   Jerror jerr;
   Jtime time;
   Jstack stack;
   //Jansi ansi;
   value_t val;
} rand_t;

typedef struct Any {
    kind_t kind;
    union {
        bool boolean;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
        const char *cstr;
        const void *cptr;
        char *str;
        void *ptr;
    } as; // <-- https://www.youtube.com/@TsodingDaily
} any;

typedef struct Anything {
    kind_t kind;
    typeinfo_t info;
    size_t size;
    void *data;
    uint32_t flags;
    any type;
} any_t;

typedef struct RingBuffer {
    Jtype *data;
    size_t head;
    size_t tail;
    size_t cap;
    size_t len;
    bool owns;
} ringbuf_t;

static inline void ringbuf_init(ringbuf_t *rb, Jtype *buf, size_t cap);
static inline bool ringbuf_alloc(ringbuf_t *rb, size_t cap);
static inline bool ringbuf_push(ringbuf_t *rb, Jtype *val);
static inline bool ringbuf_pull(ringbuf_t *rb, Jtype *val);
static inline bool ringbuf_peek(ringbuf_t *rb, Jtype *val);
static inline void ringbuf_clear(ringbuf_t *rb);
static inline void ringbuf_free(ringbuf_t *rb);

typedef struct TypeInfo {
    const char *name;
    uintptr_t addr;
    size_t size;
    kind_t kind;
} typeinfo_t;


typedef struct {
    Jerror errs;
    Jlist list;     // doubly linked list for casting ...
    typeinfo_t info;    // type info about the `value`
    any_t cast;
    any_t val;
} Jtype;

typedef struct {
    Jerror errs;
    Jtime time;
    //Jmap constellation;
    bool omen;
} Oracle;

/* function declarations */
void JTYPE_example(void);

typedef struct ByteArray {
    unsigned char *data;
    size_t length;
} bArray;

Jerror readfile_bArray(const char *path);


#endif /* JTYPE_H */
#ifdef JTYPE_IMPL

#include <stdlib.h>

/*  =========================
    RingBuffer
    ========================= */

static inline void ringbuf_clear(ringbuf_t *rb){
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
}

static inline void ringbuf_init(ringbuf_t *rb, Jtype *buf,  size_t cap){
    rb->data = buf;
    rb->cap = cap;
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
    rb->owns = false;
}

static inline bool ringbuf_alloc(ringbuf_t *rb, size_t cap){
    rb->data = (Jtype *)malloc(sizeof(Jtype) * cap);
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
    rb->capacity = 0;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->owns_data = 0;
}

static inline bool ringbuf_push(ringbuf_t *rb, Jtype *val){
    if (rb->len == rb->cap) return false;
    rb->data[rb->tail] = val;
    rb->tail = (rb->tail + 1) & rb->cap;
    rb->len++;
    return true;
}

static inline bool ringbuf_pull(ringbuf_t *rb, Jtype *val){
    if (rb->len == 0) return false;
    *val = rb->data[rb->head];
    rb->head = (rb->head + 1) & rb->cap;
    rb->len--;
    return true;
}

static inline bool ringbuf_peek(ringbuf_t *rb, Jtype *val){
    if (rb->len == 0) return false;
    *val = rb->data[rb->head];
    return true;
}


Jerror readfile_bArray(const char *path) {
    Jerror jerr;
    FILE *f;
    bArray arr;
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

/* function definitions */
void JTYPE_example(void)
{
    /* implementation */
}

#endif /* JTYPE_IMPL */