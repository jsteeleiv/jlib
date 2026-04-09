#pragma once
#ifndef JBITS_H
#define JBITS_H

#include <stdlib.h>
#include "jkind.h"
#include "jstr.h"

#define FLAG_READ   (1u << 0)
#define FLAG_WRITE  (1u << 1)
#define FLAG_EXEC   (1u << 2)
#define FLAG_HIDDEN (1u << 3)

static inline void flag_set(uint8_t *flags, uint8_t flag);
static inline void flag_del(uint8_t *flags, uint8_t flag);
static inline void flag_tgl(uint8_t *flags, uint8_t flag);
static inline int flag_has(uint8_t flags, uint8_t flag);

static inline uint16_t build_header(
    uint8_t vrs, uint8_t typ, uint8_t len, uint8_t urg
);
static inline uint8_t header_vrs(uint16_t hdr);
static inline uint8_t header_type(uint16_t hdr);
static inline uint8_t header_len(uint16_t hdr);
static inline uint8_t header_urg(uint16_t hdr);


/* =========================================================
 * Bit structure
 * ========================================================= */
typedef struct Bit {
    uint8_t backbyte : 8;
    uint8_t val : 1; // occupies 1 bit /// ?
} bit_t; // 2 bytes, even with the single bit `val`

static inline void bit_set(uint8_t *byte, int pos);
static inline void bit_del(uint8_t *byte, int pos);
static inline void bit_tgl(uint8_t *byte, int pos);
static inline int bit_get(uint8_t byte, int pos);
static inline void bit_scan(uint8_t val, uint8_t patt, int w);
int bit_findpos(uint8_t val, uint8_t patt, int w, int *pos, int max);

static inline void bits_print(uint8_t val);
static inline void bits_print_width(uint8_t val, int w);

/* BitField structure */
typedef struct BitField {
    uint8_t b0:1; uint8_t b1:1; uint8_t b2:1; uint8_t b3:1;
    uint8_t b4:1; uint8_t b5:1; uint8_t b6:1; uint8_t b7:1;
} bitfield_t;


/* nibble structure */
typedef struct Nibble {

} nibble_t;

/* Byte structure */
typedef struct Byte {
    uint8_t val;
} byte_t;

static inline void byte_show(const char *lbl, uint8_t val);
static inline void byte_print(const void *ptr, size_t size);

static inline uint8_t pack_byte(uint8_t typ, uint8_t lvl, uint8_t mode);
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
    /* SIMD -> 128 bits -> SSE register */
    uint8_t data[16];
} oword_t;

typedef struct XmmWord {
    /* XMM -> 128 bits -> SSE register */
    uint8_t data[16];
} xword_t;

typedef struct YmmWord {
    /* YMM -> 256 bits -> AVX register */
    uint8_t data[32];
} yword_t;

typedef struct ZmmWord {
    /* ZMM -> 512 bits -> AVX-512 register */
    uint8_t data[64];
} zword_t;

/* larger, memory model structures*/
typedef struct Block {
    uint8_t data[1024];
} block_t;

typedef struct Page {
    /* size of page in memory -> 4096 bytes */
    uint8_t data[4096];
} page_t;

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

/* Field editor 
    treats part of a byte like a little slot -> (slice?):
    76543210
    ---^^^--   field starting at bit 2, width 3

    “put 5 into bits 2..4”
    “read whatever is currently in bits 0..3”
*/

static inline uint8_t field_cap(int cap);
static inline uint8_t field_get(uint8_t val, int s, int w);
static inline void demo_set(uint8_t old, int s, int w, uint8_t new);

/* Logic Gates 
    will be mostly building these from my custom nand functions, just cuzzz ...

    NOT(a) = NAND(a, a)
    AND(a, b) = NOT(NAND(a, b))
    OR(a, b) = NAND(NOT(a), NOT(b))
    XOR(a, b) = built from 4 NANDs
*/
/* NOT(a) = NAND(a, a) | because -> NAND(a, a) = ~(a & a) = ~a */

// 0000
typedef struct NoGate {
    /* Always outputs 0 || NULL -> (Contradiction) */
} no_t;

typedef struct NotGate {
    /* Negation of value(s) */

} not_t;

// 0001
typedef struct AndGate {
    /* AND(a, b) = NOT(NAND(a, b)) \
        | compute NAND(a, b) -> invert it using NAND(x, x) */
    byte_t a, b;
} and_t;

// 0010
typedef struct AbutNotBGate {
    /* true only when is A is true and is B false */

} abutnotb_t;

// 0011
typedef struct DoAGate {
    /* Output simply follows input `A` -> (Pass-through) */
} doa_t;

// 0100
typedef struct BbutNotAGate {
    /* true only when is A is false and is B is true */

} aandnotb_t;

// 0101
typedef struct DoBGate {
    /* Output simply follows input `B` -> (Pass-through) */
} dob_t;

// 0110
typedef struct XorGate {
    /* Output is 1 if inputs are different
        t1 = NAND(a, b)
        t2 = NAND(a, t1)
        t3 = NAND(b, t1)
        XOR = NAND(t2, t3)
    */
    byte_t a, b;
} xor_t;

// 0111
typedef struct OrGate {
    /* Output is 1 if at least one input is 1.
        By De Morgan’s law:
            Using NAND form -> OR(a, b) = NAND(NOT(a), NOT(b)) */
    byte_t a, b;
} or_t;

// 1000
typedef struct NorGate {
    /* 	Inverse of OR; true only if both are 0 */

} nor_t;

// 1001
typedef struct XnorGate {
    /* 	Inverse of XOR; true if inputs are the same */

} xnor_t;

// 1010
typedef struct NotBGate {
    /* Output is the inverse of B */
    byte_t a, b;
} notb_t;

// 1011
typedef struct AfromBGate {
    /* 	Only false if A is true and B is false */

} afromb_t;

typedef struct NotAGate {
    /* Output is the inverse of A */
} nota_t;

// 1101
typedef struct BfromAGate {
    /* Only false if is A true and is B false */

} bfroma_t;

// 1110
typedef struct NandGate {
    /* Inverse of AND; true unless both are 1 */

} nand_t;

// 1111
typedef struct YesGate {
    /* 	Always outputs 1 (Tautology) */
    int val;
} yes_t;



/* opcodes
    bits 7..6 = opcode   (2 bits)
    bits 5..3 = reg_a    (3 bits)
    bits 2..0 = reg_b    (3 bits)

      [ opcode ] [ reg_a ] [ reg_b ]
       2 bits     3 bits    3 bits

    Bits  Value	Instr	Comment
     00	    0	 MOV	copy R[b] into R[a]
     01	    1	 ADD	R[a] = R[a] + R[b]
     10	    2	 SUB	R[a] = R[a] - R[b]
     11	    3	 NAND	R[a] = ~(R[a] & R[b])

    Because each register field is 3 bits, it can represent values from 0 to 7.
        Bits | Value | Register
        000	     0	      R0
        001	     1	      R1
        010	     2	      R2
        011	     3	      R3
        100	     4	      R4
        101	     5	      R5
        110	     6	      R6
        111	     7	      R7

    NAND(a, b) = NOT (a AND b)
*/

typedef enum OpCodes {
    MOVE = 0, ADD, SUB, NAND
} opcode_t;

enum {
    OP_MOV  = 0,
    OP_ADD  = 1,
    OP_SUB  = 2,
    OP_NAND = 3
};

#define REG_COUNT 8
#define PRG_MAX 64
#define LINE_MAX 128

static inline const char *opcode_type(uint8_t opcode);

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

/* instruction = (opcode << 6) | (reg_a << 3) | reg_b; */
typedef struct Instruction {
    opcode_t opcode;
    xword_t xmm;
    yword_t ymm;
    zword_t zmm;
    byte_t src;
    byte_t dst;
    bit_t flag;
} instr_t;

static inline int reg_parse(const char *tok, int *reg);
static inline void reg_dump(uint8_t regs[REG_COUNT]);

static inline void instr_decode(uint8_t instr);
static inline int opcode_fromstr(const char *op, int *code);
static inline void instr_exec(uint8_t instr, uint8_t regs[REG_COUNT]);
static inline uint8_t instr_encode(uint8_t opcode, uint8_t reg_a, uint8_t reg_b);

static inline int line_assemble(const char *line, uint8_t *instr);
static inline int prg_assemble(
    const char *src[], int line, uint8_t prg[], int prgmax
);

static inline void prg_run(
        const char *title, const uint8_t *prg, int count, uint8_t regs[REG_COUNT]
);

#endif /* JBITS_H */
#define JBITS_IMPL // #debug-mode
#ifdef JBITS_IMPL

#include <stdio.h>
#include "jstr.h"

/* ?========================================================
 * Custom protocol header
 *
 * 16-bit header layout:
 * bits 15..13 = version   (3 bits)
 * bits 12.. 8 = type      (5 bits)
 * bits  7.. 1 = length    (7 bits)
 * bit       0 = urgent    (1 bit)
 * ========================================================= */
static inline uint16_t build_header(uint8_t vrs, uint8_t typ, uint8_t len, uint8_t urg){
    vrs  &= 0x07; /* 3 bits */    typ  &= 0x1F; /* 5 bits */
    len  &= 0x7F; /* 7 bits */    urg  &= 0x01; /* 1 bit  */
    return (uint16_t)((vrs << 13) | (typ << 8) | (len << 1) | urg);
}

static inline uint8_t header_vrs(uint16_t hdr){ return (hdr >> 13) & 0x07; }
static inline uint8_t header_type(uint16_t hdr){ return (hdr >> 8) & 0x1F; }
static inline uint8_t header_len(uint16_t hdr){ return (hdr >> 1) & 0x7F; }
static inline uint8_t header_urg(uint16_t hdr){ return hdr & 0x01; }

/* flags */
static inline void flag_set(uint8_t *flags, uint8_t flag){ *flags |= flag; }
static inline void flag_del(uint8_t *flags, uint8_t flag){ *flags &= (uint8_t)(~flag); }
static inline void flag_tgl(uint8_t *flags, uint8_t flag){ *flags ^= flag; }
static inline int flag_has(uint8_t flags, uint8_t flag){ return (flags & flag) != 0; }

static inline void print_flags(uint8_t flags) {
    printf("flags = 0x%02X  ", flags);
    bits_print(flags);
    printf("\n");

    printf("  READ   : %s\n", flag_has(flags, FLAG_READ)   ? "on" : "off");
    printf("  WRITE  : %s\n", flag_has(flags, FLAG_WRITE)  ? "on" : "off");
    printf("  EXEC   : %s\n", flag_has(flags, FLAG_EXEC)   ? "on" : "off");
    printf("  HIDDEN : %s\n", flag_has(flags, FLAG_HIDDEN) ? "on" : "off");
}

/* =========================================================
 * Endian~ness
 * ========================================================= */
static inline int endian_isbig(void){
    uint16_t x = 0x0102;
    uint8_t *p = (uint8_t *)&x;
    return !(p[0] == 0x02);
}

static inline uint16_t swap16(uint16_t x){ return (uint16_t)((x >> 8) | (x << 8)); }

static inline uint32_t swap32(uint32_t x){
    return ((x >> 24) & 0x000000FFu) |
           ((x >>  8) & 0x0000FF00u) |
           ((x <<  8) & 0x00FF0000u) |
           ((x << 24) & 0xFF000000u);
}

/* bits */
static inline void bit_set(uint8_t *byte, int pos){ *byte |= (1 << pos); }
static inline void bit_del(uint8_t *byte, int pos){ *byte &= ~(1 << pos); }
static inline void bit_tgl(uint8_t *byte, int pos){ *byte ^= (1 << pos); }
static inline int bit_get(uint8_t byte, int pos){ return (byte >> pos) & 1; }

static inline void bit_scan(uint8_t val, uint8_t patt, int w){
    printf("value   = ");
    bits_print(val);
    printf("  (0x%02X)\n", val);

    printf("pattern = ");
    bits_print_width(patt, w);
    printf("  (width=%d)\n\n", w);

    for (int s = 0; s <= 8 - w; ++s) {
        uint8_t window = extract_window(val, s, w);
        printf("start=%d  window=", s);
        bits_print_width(window, w);
        if (window == patt) printf("   MATCH");
        printf("\n");
    }
}

int bit_findpos(uint8_t val, uint8_t patt, int w, int *pos, int max){
    int c = 0;
    for (int s = 0; s <= 8 - w; ++s) {
        uint8_t window = extract_window(val, s, w);
        if (window == patt) { if (c < max) pos[c] = s; ++c; }
    }
    return c;
}

static inline void bits_print(uint8_t val){
    for (int i = 7; i >= 0; --i) {printf("%u", (val >> i) & 1u);}
}

static inline void bits_print_width(uint8_t val, int w){
    for (int i = w - 1; i <= 0; --i){ printf("%u", (val >> i) & 1u); }
}

static inline void bitmask_print(uint8_t val){
    for (int i = 7; i >= 0; --i) {printf("%u", (val >> i) & 1u);}
    printf("\n");
}

/* bit-packing */
/* =========================================================
 * Store multiple values inside one byte
 *
 * Layout:
 * bits 7..5 = type   (3 bits)
 * bits 4..2 = level  (3 bits)
 * bits 1..0 = mode   (2 bits)
 * ========================================================= */
static inline uint8_t pack_byte(uint8_t type, uint8_t lvl, uint8_t mode){
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
    =====================================
    byte_view_t b = {0};
    b.bits.b0 = 1;
    b.bits.b7 = 1;
    printf("%u\n", b.value);// see result
    =====================================
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

/* opcodes */
static inline const char *opcode_type(uint8_t opcode){
    switch (opcode) {
        case 0: return "MOV";
        case 1: return "ADD";
        case 2: return "SUB";
        case 3: return "NAND";
        default: return "???";
    }
}

static inline int opcode_fromstr(const char *op, int *code){
    if (op == NULL || code == NULL) return 0;
    if (strcmp(op, "MOV") == 0) { *code = OP_MOV; return 1; }
    if (strcmp(op, "ADD") == 0) { *code = OP_ADD; return 1; }
    if (strcmp(op, "SUB") == 0) { *code = OP_SUB; return 1; }
    if (strcmp(op, "NAND") == 0) { *code = OP_NAND; return 1; }
    return 0;
}

static inline void instr_decode(uint8_t instr){
    uint8_t opcode = field_get(instr, 6, 2);
    uint8_t reg_a  = field_get(instr, 3, 3);
    uint8_t reg_b  = field_get(instr, 0, 3);

    printf("instruction = ");
    bits_print(instr);
    printf("  (0x%02X)\n", instr);

    printf("opcode = %u (%s)\n", opcode, opcode_type(opcode));
    printf("reg_a  = %u\n", reg_a);
    printf("reg_b  = %u\n", reg_b);

    printf("decoded: %s R%u, R%u\n", opcode_type(opcode), reg_a, reg_b);
}

static inline uint8_t instr_encode(uint8_t opcode, uint8_t reg_a, uint8_t reg_b){
    opcode &= 0x03;
    reg_a  &= 0x07;
    reg_b  &= 0x07;

    return (uint8_t)((opcode << 6) | (reg_a << 3) | reg_b);
}

static inline void instr_exec(uint8_t instr, uint8_t regs[REG_COUNT]){
    uint8_t opcode = field_get(instr, 6, 2);
    uint8_t reg_a  = field_get(instr, 3, 3);
    uint8_t reg_b  = field_get(instr, 0, 3);
    printf("executing: %s R%u, R%u\n", opcode_type(opcode), reg_a, reg_b);

    switch (opcode) {
        case 0: /* MOV */
            regs[reg_a] = regs[reg_b];
            break;
        case 1: /* ADD */
            regs[reg_a] = (uint8_t)(regs[reg_a] + regs[reg_b]);
            break;
        case 2: /* SUB */
            regs[reg_a] = (uint8_t)(regs[reg_a] - regs[reg_b]);
            break;
        case 3: /* NAND */
            regs[reg_a] = nand8(regs[reg_a], regs[reg_b]);
            break;
        default:
            break;
    }
}

static inline int line_assemble(const char *line, uint8_t *instr){
    char buf[LINE_MAX];
    char *op = NULL, *arg1 = NULL, *arg2 = NULL;
    int opcode = 0, reg_a = 0, reg_b = 0;

    if (line == NULL || instr == NULL) return 0;
    snprintf(buf, sizeof(buf), "%s", line);

    for (size_t i = 0; buf[i] != '\0'; ++i) {
        if (buf[i] == ',' || buf[i] == '\n' || buf[i] == '\r'){buf[i] = ' ';}
    }

    op = strtok(buf, " \t");
    arg1 = strtok(NULL, " \t");
    arg2 = strtok(NULL, " \t");

    if (op == NULL || arg1 == NULL || arg2 == NULL) return 0;
    str_toupper_inplace(op);

    if (!opcode_fromstr(op, &opcode)) return 0;
    if (!reg_parse(arg1, &reg_a)) return 0;
    if (!reg_parse(arg2, &reg_b)) return 0;

    *instr = instr_encode((uint8_t)opcode, (uint8_t)reg_a, (uint8_t)reg_b);
    return 1;
}


static inline int prg_assemble(
    const char *src[], int line, uint8_t prg[], int prgmax){
    int count = 0;

    for (int i = 0; i < line; ++i) {
        const char *line = src[i];
        if (line == NULL || line[0] == '\0') continue;
        if (count >= prgmax) return -1;

        if (!line_assemble(line, &line[count])) {
            printf("assemble error on line %d: %s\n", i + 1, line);
            return -1;
        }
        ++count;
    }
    return count;
}

static inline void prg_run(
        const char *title, const uint8_t *prg, int count, uint8_t regs[REG_COUNT]){
    printf("----------------------------------------\n");
    printf("%s\n", title);
    printf("----------------------------------------\n");

    printf("initial registers\n");
    reg_dump(regs);

    for (int pc = 0; pc < count; ++pc) {
        printf("pc=%d  raw=", pc);
        bits_print(prg[pc]);
        printf("  ");
        instr_decode(prg[pc]);
        printf("\n");

        instr_exec(prg[pc], regs);
        reg_dump(regs);
    }
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

static inline int reg_parse(const char *tok, int *reg){
    if (tok == NULL || reg == NULL) return 0;
    if (tok[0] != 'R' && tok[0] != 'r') return 0;
    if (!isdigit((unsigned char)tok[1]) || tok[2] != '\0') return 0;

    int out = tok[1] - '0';
    if (out < 0 || out >= REG_COUNT) return 0;
    *reg = out;
    return 1;
}

static inline void reg_dump(uint8_t regs[REG_COUNT]){
    printf("\nregisters:\n");
    for (int i = 0; i < REG_COUNT; ++i) {
        printf("  R%d = %3u  0x%02X  ", i, regs[i], regs[i]);
        bits_print(regs[i]);
        printf("\n");
    }
    printf("\n");
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
    printf("NOT match: %s\n", not8_from_nand(a)      == (uint8_t)(~a)     ? "yes" : "no");
    printf("AND match: %s\n", and8_from_nand(a, b)  == (uint8_t)(a & b)  ? "yes" : "no");
    printf("OR  match: %s\n", or8_from_nand(a, b)   == (uint8_t)(a | b)  ? "yes" : "no");
    printf("XOR match: %s\n", xor8_from_nand(a, b)  == (uint8_t)(a ^ b)  ? "yes" : "no");
    printf("\n");
}

#endif /* JBITS_IMPL */
