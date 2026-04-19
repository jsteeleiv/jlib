#pragma once
#ifndef JBITS_H
#define JBITS_H

/* include sub-libraries */ 
#include "../jval.h"
#include "../jstr.h"

#include <stdlib.h>

#define MAX_DIGITS 1024

typedef uint8_t  bitpattern_8;
typedef uint16_t bitpattern_16;
typedef uint32_t bitpattern_32;
typedef uint64_t bitpattern_64;

/* Bit structure
----------------
    An attempt at representing a bit
*/
typedef struct Bit {
    uint8_t backbyte : 8;
    uint8_t val : 1; // occupies 1 bit /// ?
} bit_t; // 2 bytes, even with the single bit `val`

/* single bit functions */
static inline int bit_get(uint8_t byte, int pos);
static inline void bit_set(uint8_t *byte, int pos);
static inline void bit_del(uint8_t *byte, int pos);
static inline void bit_tgl(uint8_t *byte, int pos);
static inline void bit_scan(uint8_t val, uint8_t patt, int w);
static inline int bit_find(uint8_t val, uint8_t patt, int w, int *pos, int max);
/* basic print */
static inline void bits_print(uint8_t val);
/* print specific width */
static inline void bits_printw(uint8_t val, int w);
static inline void bits_printmax(bitpattern_64 val, size_t sz);

typedef struct BitPattern {
    uint64_t pattern;
    size_t bit_width;
} bitpatt_t;

/* BitField structure */
typedef struct BitField {
    uint8_t b0:1; uint8_t b1:1; uint8_t b2:1; uint8_t b3:1;
    uint8_t b4:1; uint8_t b5:1; uint8_t b6:1; uint8_t b7:1;
} bitfield_t;

/* Bit-Field editor
---------------
    treats part of a byte like a little slot -> (slice?):
    76543210
    ---^^^--   field starting at bit 2, width 3

    “put 5 into bits 2..4”
    “read whatever is currently in bits 0..3”
*/
static inline uint8_t field_cap(int cap);
static inline uint8_t field_get(uint8_t val, int s, int w);
static inline void demo_set(uint8_t old, int s, int w, uint8_t new);

/* nibble structure */
typedef struct Nibble {
    bit_t bits[4];
} nibble_t;

/* Byte structure */
typedef struct Byte {
    uint8_t val;
} byte_t;

static inline void byte_show(const char *lbl, uint8_t val);
static inline void byte_print(const void *ptr, size_t size);

static inline uint8_t byte_pack(uint8_t typ, uint8_t lvl, uint8_t mode);
static inline uint8_t unpack_type(uint8_t pb);
static inline uint8_t unpack_level(uint8_t pb);
static inline uint8_t unpack_mode(uint8_t pb);

/* endian-ness */
static inline int endian_isbig(void);
static inline uint16_t swap16(uint16_t x);
static inline uint32_t swap32(uint32_t x);

static inline uint8_t extract_window(uint8_t val, int s, int w);
static inline void show_window(uint8_t val, int w);

/* ByteView structure */
typedef union ByteView {
    uint8_t val;
    bitfield_t bits;
} byteview_t;

typedef struct TenByte {
    /* 80 bits ~ used in x86 - extended precision floats */

} tbyte_t;

/* Multi-Byte structures */
typedef struct Word {
    uint16_t val;
} word_t;

static inline void word_print(uint16_t word);

typedef struct DoubleWord {
    uint32_t val;
} dword_t;

typedef struct QuadWord {
    uint64_t val;
} qword_t;

typedef struct OctaWord {
    /* SIMD -> 128 bits -> SSE register; see 'jcpu.h' */
    uint8_t data[16];
} oword_t;



/* Mask structure
    value = 10110110
    mask  = 00001111
    ----------------
    result= 00000110 --> (result = value & mask)
*/
typedef struct Mask {
    uint64_t bits;
    uint8_t val;
} mask_t;

static inline mask_t mask_bit(int pos);
static inline void bitmask_print(uint8_t val);
static inline mask_t mask_range(int s, int w);
static inline uint8_t mask_set(uint8_t val, mask_t mask);
static inline uint8_t mask_apply(uint8_t val, mask_t mask);
static inline uint8_t mask_clear(uint8_t val, mask_t mask);
static inline uint8_t mask_toggle(uint8_t val, mask_t mask);




static inline uint8_t nand8(uint8_t a, uint8_t b);
static inline uint16_t nand16(uint16_t a, uint16_t b);
static inline uint32_t nand32(uint32_t a, uint32_t b);
static inline uint64_t nand64(uint64_t a, uint64_t b);
static inline kind_t nandK(kind_t a, kind_t b, kind_t kind);

static inline kind_t not_from_kind(kind_t a);
static inline kind_t and_from_kind(kind_t a, kind_t b);

static inline uint8_t not8_from_nand(uint8_t a);
static inline uint8_t and8_from_nand(uint8_t a, uint8_t b);
static inline uint8_t or8_from_nand(uint8_t a, uint8_t b);
static inline uint8_t xor8_from_nand(uint8_t a, uint8_t b);

static inline uint8_t not8(uint8_t a);
static inline uint8_t and8(uint8_t a, uint8_t b);
static inline uint8_t or8(uint8_t a, uint8_t b);
static inline uint8_t xor8(uint8_t a, uint8_t b);
static inline void demo_pair(uint8_t a, uint8_t b);
static inline void demo_nand(uint8_t a, uint8_t b);

static inline void show_value(const char *lbl, uint8_t val);


typedef struct DigitArray {
    int digits[MAX_DIGITS];
    int length;
    bool negative;
} digitarr_t;

static inline digitarr_t int2arr(int n);
static inline int arr2int(digitarr_t arr);
static inline digitarr_t digitsort_asc(digitarr_t arr);
static inline digitarr_t digitsort_desc(digitarr_t arr);

#endif /* JBITS_H */
#define JBITS_IMPL // #debug-mode
#ifdef JBITS_IMPL

#include <stdio.h>
#include "jstr.h"



/* Endian~ness
--------------
    big-endian == most significant bit first
    little-endian == least significant bit first
*/
static inline int endian_isbig(void){
    uint16_t x = 0x0102;
    uint8_t *p = (uint8_t *)&x;
    return !(p[0] == 0x02);
}

static inline uint16_t swap16(uint16_t x){ return (uint16_t)((x >> 8) | (x << 8)); }

static inline uint32_t swap32(uint32_t x){
    return ((x >> 24) & 0x000000FFu) | ((x >>  8) & 0x0000FF00u) |
           ((x <<  8) & 0x00FF0000u) | ((x << 24) & 0xFF000000u);
}

/* bits */
static inline void bit_set(uint8_t *byte, int pos){ *byte |= (1u << pos); }
static inline void bit_del(uint8_t *byte, int pos){ *byte &= ~(1u << pos); }
static inline void bit_tgl(uint8_t *byte, int pos){ *byte ^= (1u << pos); }
static inline int bit_get(uint8_t byte, int pos){ return (byte >> pos) & 1u; }

static inline void bit_scan(uint8_t val, uint8_t patt, int w){
    printf("value   = ");
    bits_print(val);
    printf("  (0x%02X)\n", val);

    printf("pattern = ");
    bits_printw(patt, w);
    printf("  (width=%d)\n\n", w);

    for (int s = 0; s <= 8 - w; ++s) {
        uint8_t window = extract_window(val, s, w);
        printf("start=%d  window=", s);
        bits_printw(window, w);
        if (window == patt) printf("   MATCH");
        printf("\n");
    }
}

int bit_find(uint8_t val, uint8_t patt, int w, int *pos, int max){
    int c = 0;
    for (int s = 0; s <= 8 - w; ++s) {
        uint8_t window = extract_window(val, s, w);
        if (window == patt) { if (c < max) pos[c] = s; ++c; }
    }
    return c;
}
/* printing bits in various ways */
static inline void bits_print(uint8_t val){
    for (int i = 7; i >= 0; --i) {printf("%u", (val >> i) & 1u);}
}

static inline void bits_printw(uint8_t val, int w){
    for (int i = w - 1; i <= 0; --i){ printf("%u", (val >> i) & 1u); }
}

static inline void bitmask_print(uint8_t val){
    for (int i = 7; i >= 0; --i) {printf("%u", (val >> i) & 1u);}
    printf("\n");
}

static inline void bits_printmax(uint64_t val, size_t sz){
    for (size_t i = 0; i < sz; i++) {
        size_t bitidx = sz - 1 - i;
        printf("%u ", get_bit(val, bitidx));
    }
}

/* bit-packing
--------------
    * Store multiple values inside one byte
 
    Layout:
    bits 7..5 = type   (3 bits)
    bits 4..2 = level  (3 bits)
    bits 1..0 = mode   (2 bits) 
*/
static inline uint8_t byte_pack(uint8_t type, uint8_t lvl, uint8_t mode){
    type  &= 0x07; /* 3 bits */
    lvl &= 0x07; /* 3 bits */
    mode  &= 0x03; /* 2 bits */
    return (type << 5) | (lvl << 2) | mode;
}
static inline uint8_t unpack_type(uint8_t pb){ return (pb >> 5) & 0x07; }
static inline uint8_t unpack_level(uint8_t pb){ return (pb >> 2) & 0x07; }
static inline uint8_t unpack_mode(uint8_t pb){ return pb & 0x03; }

/* Shifting windows of bits
 *  - extract a moving window from a byte
*/
static inline uint8_t extract_window(uint8_t val, int s, int w){
    if (s < 0 || s > 7 || w <= 0 || w > 8 || s + w > 8) {
        return 0;
    }
    /* Ex:
     * value = abcdefgh
     * start=2 width=3 -> extract cde
     * treat bit 0 as the `least-significant` bit.
     */
    uint8_t mask = (uint8_t)((1u << w) - 1u);
    return (uint8_t)(val >> s) & mask;
}

static inline void show_window(uint8_t val, int w){
    printf("val = 0x%02X  ", val);
    bits_print(val); printf("\n\n");

    for (int i = 0; i <= 8 - w; ++i){
        printf("        ");
        for (int j = 0; j < i; ++j) printf(" ");

        uint8_t window = extract_window(val, i, w);
        for (int k = w - 1; k >= 0; --k){
            printf("%u", (window >> i) & 1u);
        }
        printf("  (start=%d)\n", i);
    }
    // printf("window start=%d width=%d -> ", i, w);
    // bits_print(window);
    // printf(" (0x%02X / %u)\n", window, window);
}

/* bytes */
static inline void byte_set(byte_t *b, int pos) { b->val |= (1 << pos); }
static inline void byte_del(byte_t *b, int pos) { b->val &= ~(1 << pos); }
static inline int byte_get(byte_t b, int pos) { return (b.val >> pos) & 1; }
static inline void byte_show(const char *lbl, uint8_t val){
    printf("%s = %3u  0x%02X  ", lbl, val, val);
    bits_print(val); printf("\n");
}

static inline void byte_print(const void *ptr, size_t size){
    const uint8_t *p = (const uint8_t *)ptr;
    for (size_t i = 0; i < size; ++i) {
        printf("byte[%zu] = 0x%02X  ", i, p[i]);
        bits_print(p[i]);
        printf("\n");
    }
}

/*. byte-view 
-------------
    byte_view_t b = {0};
    b.bits.b0 = 1;
    b.bits.b7 = 1;
    printf("%u\n", b.value);// see result
*/

/* words */
static inline void word_print(uint16_t word){
    for (int i = 15; i >= 0; --i) {
        printf("%u", (word >> i) & 1u);
        // printf("%u", (word >> i) & 1);
        if (i % 8 == 0 && i != 0) printf(" ");
    }
}

/*  ~ Masks ~
    ===========================================
    mask_t m = { .mask = 0x0F }; --> lower 4 bits
    uint8_t result = byte.value & m.mask;

    *Bit packing
    (value & mask) >> shift
    *Setting fields
    value &= ~mask;        // clear field
    value |= (new << pos); // set field

*/
static inline uint8_t mask_apply(uint8_t val, mask_t mask){return val & mask.val;}
static inline uint8_t mask_set(uint8_t val, mask_t mask){return val | mask.val;}
static inline uint8_t mask_clear(uint8_t val, mask_t mask){return val & ~(mask.val);}
static inline uint8_t mask_toggle(uint8_t val, mask_t mask){return val ^ mask.val;}
static inline mask_t mask_bit(int pos){mask_t m; m.val = (1 << pos); return m;}
static inline mask_t mask_range(int s, int w){ mask_t m = {0};
    if (s < 0 || s > 7 || w <= 0 || w > 8 || s + w > 8) return m;
    // m.val = ((1 << w) - 1) << s;
    m.val = (uint8_t)(((1u << w) - 1u) << s);
    return m;
} /* ex --v--: mask_range(2, 3) [== 00011100] */

static inline uint8_t field_cap(int cap){
    if (cap <= 0 || cap > 8) return 0;
    return (uint8_t)((1u << cap) - 1u);
}

static inline uint8_t field_get(uint8_t val, int s, int w){
    mask_t mask = mask_range(s, w);
    return (uint8_t)((val & mask.val) >> s);
}

static inline uint8_t field_set(uint8_t old, int s, int w, uint8_t val){
    mask_t mask = mask_range(s, w);
    if (mask.val == 0) return old;

    val &= field_cap(w);
    old &= (uint8_t)(~mask.val);
    old |= (uint8_t)((val << s) & mask.val);
    return old;
}

static inline void demo_set(uint8_t old, int s, int w, uint8_t new){
    mask_t mask = mask_range(s, w);
    uint8_t res = field_set(old, s, w, new);
    uint8_t x = field_get(res, s, w);

    printf("\n--- set field start=%d width=%d value=%u ---\n", s, w, new);
    byte_show("original", old);
    byte_show("mask    ", mask.val);
    byte_show("result  ", res);
    printf("extracted field = %u\n", x);
}


static inline uint8_t nand8(uint8_t a, uint8_t b){return (uint8_t)(~(a & b));}
static inline uint16_t nand16(uint16_t a, uint16_t b){return (uint16_t)(~(a & b));}
static inline uint32_t nand32(uint32_t a, uint32_t b){return (uint32_t)(~(a & b));}
static inline uint64_t nand64(uint64_t a, uint64_t b){return (uint64_t)(~(a & b));}
static inline kind_t nandK(kind_t a, kind_t b, kind_t kind){
    if (!kind) return (unsigned int)(~(a & b));
    //TODO: need to figure out how to insert kind(type) in cast...
    return (kind_t)(~(a & b));
}

static inline kind_t not_from_kind(kind_t a){return ~(a & a);}
static inline kind_t and_from_kind(kind_t a, kind_t b){
    return ~(~(a & b) & ~(a & b));
    // kind_t nand = ~(a & b); return ~(nand & nand);
}
static inline kind_t or_from_kind(kind_t a, kind_t b){
    // kind_t k = not(a, b);
    return ~(~(a & a) | ~(b & b));
}

static inline kind_t xor_from_kind(kind_t a, kind_t b){

}



static inline uint8_t not8_from_nand(uint8_t a){
    return nand8(a, a);
}

uint8_t and8_from_nand(uint8_t a, uint8_t b){
    uint8_t t = nand8(a, b); return nand8(t, t);
}

static inline uint8_t or8_from_nand(uint8_t a, uint8_t b){
    uint8_t na = not8_from_nand(a);
    uint8_t nb = not8_from_nand(b);
    return nand8(na, nb);
}

static inline uint8_t xor8_from_nand(uint8_t a, uint8_t b){
    uint8_t t1 = nand8(a, b);
    uint8_t t2 = nand8(a, t1);
    uint8_t t3 = nand8(b, t1);
    return nand8(t2, t3);
}

static inline uint8_t not8(uint8_t a){
    return nand8(a, a);
}

static inline uint8_t and8(uint8_t a, uint8_t b){
    uint8_t t = nand8(a, b);
    return nand8(t, t);
}

static inline uint8_t or8(uint8_t a, uint8_t b){
    return nand8(not8(a), not8(b));
}

static inline uint8_t xor8(uint8_t a, uint8_t b){
    uint8_t t1 = nand8(a, b);
    uint8_t t2 = nand8(a, t1);
    uint8_t t3 = nand8(b, t1);
    return nand8(t2, t3);
}

static inline void show_value(const char *lbl, uint8_t val){
    printf("%-10s = %3u  0x%02X  ", lbl, val, val);
    bits_print(val);
    printf("\n");
}

static inline void demo_pair(uint8_t a, uint8_t b){
    printf("========================================\n");
    show_value("a", a);
    show_value("b", b);
    printf("\n");

    show_value("NAND(a,b)", nand8(a, b));
    show_value("NOT(a)",    not8_from_nand(a));
    show_value("NOT(b)",    not8_from_nand(b));
    show_value("AND(a,b)",  and8_from_nand(a, b));
    show_value("OR(a,b)",   or8_from_nand(a, b));
    show_value("XOR(a,b)",  xor8_from_nand(a, b));
    printf("\n");

    show_value("~a",     (uint8_t)(~a));
    show_value("a & b",  (uint8_t)(a & b));
    show_value("a | b",  (uint8_t)(a | b));
    show_value("a ^ b",  (uint8_t)(a ^ b));
    printf("========================================\n\n");
}

static inline void demo_nand(uint8_t a, uint8_t b){
    printf("========================================\n");
    printf("NAND-DERIVED GATE DEMO\n");
    printf("========================================\n");

    show_value("a", a);
    show_value("b", b);
    printf("\n");

    show_value("nand(a,b)", nand8(a, b));
    show_value("not(a)",    not8_from_nand(a));
    show_value("and(a,b)",  and8_from_nand(a, b));
    show_value("or(a,b)",   or8_from_nand(a, b));
    show_value("xor(a,b)",  xor8_from_nand(a, b));
    printf("\n");

    show_value("native ~a",   (uint8_t)(~a));
    show_value("native a&b",  (uint8_t)(a & b));
    show_value("native a|b",  (uint8_t)(a | b));
    show_value("native a^b",  (uint8_t)(a ^ b));
    printf("\n");

    printf("checks\n");
    printf("------\n");
    printf("NOT match: %s\n", not8_from_nand(a)     == (uint8_t)(~a)     ? "yes" : "no");
    printf("AND match: %s\n", and8_from_nand(a, b)  == (uint8_t)(a & b)  ? "yes" : "no");
    printf("OR  match: %s\n", or8_from_nand(a, b)   == (uint8_t)(a | b)  ? "yes" : "no");
    printf("XOR match: %s\n", xor8_from_nand(a, b)  == (uint8_t)(a ^ b)  ? "yes" : "no");
    printf("\n");
}

/* Digit Array Logic
--------------------

*/

static inline digitarr_t int2arr(int n){
    if (n < 0) n = -n;
    digitarr_t ret;
    ret.length = 0;
    if (n == 0) {
        ret.digits[0] = 0;
        ret.length = 1;
        ret.negative = false;
    }
    
    while (n > 0 && ret.length < MAX_DIGITS){
        ret.digits[ret.length++] = n % 10;
        n /= 10;
    }
    for (int i = 0; i < ret.length/2; i++){
        int tmp = ret.digits[i];
        ret.digits[i] = ret.digits[ret.length - i - 1];
        ret.digits[ret.length - i - 1] = tmp;
    }
    return ret;

}

static inline int arr2int(digitarr_t arr){
    int ret = 0;
    for (int i = 0; i < arr.length; i++){
        ret = ret * 10 + arr.digits[i];
    }
    return ret;
}

static inline digitarr_t digitsort_asc(digitarr_t arr){
    for (int i = 0; i < arr.length - 1; i++){
        for (int j = 0; j < arr.length - i - 1; j++){
            if (arr.digits[j] > arr .digits[j + 1]){
                int tmp = arr.digits[j];
                arr.digits[j] = arr.digits[j + 1];
                arr.digits[j + 1] = tmp;
            }
        }
    }
    return arr;
}

static inline digitarr_t digitsort_desc(digitarr_t arr){
    for (int i = 0; i < arr.length - 1; i++){
        for (int j = 0; j < arr.length - i - 1; j++){
            if (arr.digits[j] < arr .digits[j + 1]){
                int tmp = arr.digits[j];
                arr.digits[j] = arr.digits[j + 1];
                arr.digits[j + 1] = tmp;
            }
        }
    }
    return arr;
}

/*

AND

2 inputs: A, B

bitpattern_t compute_and(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 1);
    uint8_t b = get_bit(inputs, 0);

    uint8_t y = a & b;
    return y;
}
OR
bitpattern_t compute_or(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 1);
    uint8_t b = get_bit(inputs, 0);

    uint8_t y = a | b;
    return y;
}
XOR
bitpattern_t compute_xor(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 1);
    uint8_t b = get_bit(inputs, 0);

    uint8_t y = a ^ b;
    return y;
}
NAND
bitpattern_t compute_nand(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 1);
    uint8_t b = get_bit(inputs, 0);

    uint8_t y = (uint8_t)(!(a & b));
    return y;
}
NOR
bitpattern_t compute_nor(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 1);
    uint8_t b = get_bit(inputs, 0);

    uint8_t y = (uint8_t)(!(a | b));
    return y;
}
XNOR
bitpattern_t compute_xnor(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 1);
    uint8_t b = get_bit(inputs, 0);

    uint8_t y = (uint8_t)(!(a ^ b));
    return y;
}
NOT

1 input: A

bitpattern_t compute_not(bitpattern_t inputs) {
    uint8_t a = get_bit(inputs, 0);

    uint8_t y = (uint8_t)(!a);
    return y;
}

*/

#endif /* JBITS_IMPL */
