#pragma once
#ifndef JCPU_H
#define JCPU_H

#include <stdlib.h>
#include "jbits.h"

#define REG_COUNT 8
#define PRG_MAX 64
#define LINE_MAX 128

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
static inline uint8_t header_version(uint16_t hdr);
static inline uint8_t header_type(uint16_t hdr);
static inline uint8_t header_length(uint16_t hdr);
static inline uint8_t header_urgency(uint16_t hdr);

typedef struct XmmWord {
    /* XMM -> 128 bits -> SSE register */
    uint8_t data[16];
} xmmword_t;

typedef struct YmmWord {
    /* YMM -> 256 bits -> AVX register */
    uint8_t data[32];
} ymmword_t;

typedef struct ZmmWord {
    /* ZMM -> 512 bits -> AVX-512 register */
    uint8_t data[64];
} zmmword_t;

/* larger, memory model structures*/
typedef struct Block {
    uint8_t data[1024];
} block_t;

typedef struct Page {
    /* size of page in memory -> 4096 bytes */
    uint8_t data[4096];
} page_t;

typedef struct Register {
    union {
        xmmword_t xmm;
        ymmword_t ymm;
        zmmword_t zmm;
    } use;
    char *debug;
    block_t block;
    page_t page;

} reg_t;

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
*/

typedef enum OpCodes {
    OP_MOV = 0, OP_ADD, OP_SUB, OP_NAND
} opcode_t;

static inline const char *opcode_type(uint8_t opcode);

/* instruction = (opcode << 6) | (reg_a << 3) | reg_b; */
typedef struct Instruction {
    opcode_t opcode;
    reg_t *regs;
    byte_t src;
    byte_t dst;
    uint32_t flags;
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

typedef struct CPU {

} cpu_t;

#endif /* JCPU_H */
#define JCPU_IMPL // #debug-mode
#ifdef JCPU_IMPL

#include <string.h>
#include <stddef.h>

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

static inline uint8_t header_version(uint16_t hdr){ return (hdr >> 13) & 0x07; }
static inline uint8_t header_type(uint16_t hdr){ return (hdr >> 8) & 0x1F; }
static inline uint8_t header_length(uint16_t hdr){ return (hdr >> 1) & 0x7F; }
static inline uint8_t header_urgency(uint16_t hdr){ return hdr & 0x01; }

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

        if (!line_assemble(line, &prg[count])) {
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

#endif /* JCPU_IMPL */