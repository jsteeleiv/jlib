#pragma once
#ifndef JTRUTH_H
#define JTRUTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "jbits.h"
typedef uint64_t ttbitpattern_t;


#define TT_MAX_INPUTS  8
#define TT_MAX_OUTPUTS 8
#define TT_MAX_ROWS (1u << TT_MAX_INPUTS)

typedef enum GateType {
    GATE_NO = 0,  // 0000
    GATE_AND,     // 0001
    GATE_ANOTB,   // 0010
    GATE_DO_A,    // 0011
    GATE_BNOTA,   // 0100
    GATE_DO_B,    // 0101
    GATE_XOR,     // 0110
    GATE_OR,      // 0111
    GATE_NOR,     // 1000
    GATE_XNOR,    // 1001 
    GATE_NOTB,    // 1010
    GATE_AFROMB,  // 1011
    GATE_NOTA,    // 1110
    GATE_BFROMA,  // 1101
    GATE_NAND,    // 1110
    GATE_YES,     // 1111
    GATE_NOT,     // 00.00
    GATE_BUFF,    // 00.01  
    GATE_CUSTOM   // 00.11
} gatetype_t;

typedef struct BuiltinGateInfo {
    char *name;
    char *symbol;        // e.g. "&", "|", "^", "~"
    size_t arity_in;
    size_t arity_out;
    uint64_t truth_bits; // single-output optimization
} bigateinfo_t;

/* Logic Gates
--------------
    The goal is to build practically everything out of nand gates
        ... at some point at least
    ------------------------------
    NOT(a) = NAND(a, a)
    AND(a, b) = NOT(NAND(a, b))
    OR(a, b) = NAND(NOT(a), NOT(b))
    XOR(a, b) = built from 4 NANDs

    NAND(a, b) = NOT (a AND b)
    NOT(a) = NAND(a, a) | because -> NAND(a, a) = ~(a & a) = ~a 
*/

// 0000
typedef struct NoGate {
    /* Always outputs 0 || NULL -> (Contradiction) */
} no_t;

// 00.00
typedef struct NotGate {
    /* Negation of value(s) */

} not_t;

// 0001
typedef struct AndGate {
    /* AND(a, b) = NOT(NAND(a, b)) \s
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

typedef enum TruthTableClass {
    TTCLASS_UNKNOWN = 0,
    TTCLASS_CONST_ZERO,
    TTCLASS_CONST_ONE,
    TTCLASS_IDENTITY,
    TTCLASS_NEGATION,
    TTCLASS_STANDARD_GATE,
    TTCLASS_CUSTOM_LOGIC
} ttclass_t;


typedef struct ExpressionInfo {
    char *expr; // e.g. "(A & B) | C"
    size_t expr_sz;
} ttexprinfo_t;

typedef struct ExpressionNode {
    ttexprinfo_t info;
} ttexprnode_t;

/*
    Each row stores the input combination and output combination as bitfields.

    Example:
      inputs  = 0b011  means A=0, B=1, C=1
      outputs = 0b10   means Y0=1, Y1=0 depending on your chosen bit ordering
*/


typedef enum LogicKind {
    LGC_KIND_BUILTIN = 0,
    LGC_KIND_EXPR,
    LGC_KIND_TABLE
} logickind_t;

typedef struct LogicFunction {
    char *name;
    gatetype_t type;
    size_t arity;       // number of inputs expected
    uint64_t truth_bits; 
    /*
        For single-output gates only:
        bit i = output for input combination i

        Example for 2-input AND:
        inputs:
          00 -> 0
          01 -> 0
          10 -> 0
          11 -> 1

        truth_bits = 0b1000 if row index increases from 0 to 3
    */
} logicfunc_t;

typedef struct LogicDefinition {
    char *name;
    size_t input_sz;
    size_t output_sz;
    char **labels_in;
    char **labels_out;
    logickind_t srckind;
    union {
        bigateinfo_t builtin;
        // ExpressionInfo expr;
        ttinfo_t table;
    } source;
} lgcdef_t;

typedef struct LogicInformation {

} lgcinfo_t;

// logic compute function
typedef ttbitpattern_t (*lgc_compfn)(ttbitpattern_t inputs);
// typedef ttbitpattern_t (*LogicComputeFn)(ttbitpattern_t inputs);

typedef struct LogicDevice {
    lgc_compfn compute;
    const char **labels_in;
    const char **labels_out;
    const char *name;
    size_t input_sz;
    size_t output_sz;
    union {
        bigateinfo_t builtin;
        // exprinfo_t expr;
        ttinfo_t table;
    } source;
} lgcdev_t;

typedef struct TruthTableRow {
    ttbitpattern_t input;  // bits: A B Cin
    ttbitpattern_t output; // bits: Sum Cout
} ttrow_t;

typedef struct TruthTableInfo {
    size_t row_sz;
    ttrow_t *rows;
} ttinfo_t;

typedef struct TruthTable {
    const char *name;              // e.g. "Half Adder", "AND", "CustomLogic1"
    gatetype_t gatetype;
    ttclass_t tt_class;
    size_t input_sz;      // number of input variables
    size_t output_sz;     // number of output variables
    size_t row_sz;        // usually 1 << input_sz
    const char **labels_in;     // e.g. {"A", "B", "Cin"}
    const char **labels_out;    // e.g. {"Sum", "Carry"}
    ttrow_t *rows;           // row_sz rows

    /* metadata:
    ------------
        - minterms for a single-output function
        - maxterms
        - don't-care terms
    */
    uint64_t *minterms;
    size_t minterm_sz;
    uint64_t *maxterms;
    size_t maxterm_sz;
    uint64_t *dont_cares;
    size_t dontcare_sz;

    /* expression string:
    --------------------
        Example: "Y = (A & B) | (~C)"
    */
    char *expression;
} truthtbl_t;

/*
    TruthTable *tt_create(const char *name, size_t input_sz, size_t output_sz);
    void tt_destroy(TruthTable *tt);

    bool tt_set_input_label(TruthTable *tt, size_t index, const char *label);
    bool tt_set_output_label(TruthTable *tt, size_t index, const char *label);

    bool tt_set_row(TruthTable *tt, size_t row_index, uint64_t input_bits, uint64_t output_bits);
    bool tt_get_output_for_input(const TruthTable *tt, uint64_t input_bits, uint64_t *output_bits);

    void tt_print(const TruthTable *tt);
    bool tt_is_equivalent(const TruthTable *a, const TruthTable *b);

    uint64_t tt_compute_truth_bits_single_output(const TruthTable *tt);

*/


/* loop from 0 to 7, extract A/B/Cin, compute output, and fill rows[i]. */
static inline uint8_t fulladder_compute(uint8_t a, uint8_t b, uint8_t cin);
static inline void truthtbl_print(const truthtbl_t *tt);

#endif /* JTRUTH_H */
#define JTRUTH_IMPL // #COMPILER debug-mode
#ifdef JTRUTH_IMPL

/* Example: AND gate
--------------------
    LogicDefinition and_gate = {
        .name = "AND",
        .input_sz = 2,
        .output_sz = 1,
        .source_kind = GATE_KIND_BUILTIN,
        .source.builtin = {
            .name = "AND",
            .symbol = "&",
            .input_arity = 2,
            .output_arity = 1,
            .truth_bits = 0b1000
        }
    };

    ADDER LOGIC
    -----------
    | A | B | Cin | Sum | Cout |
    | - | - | --- | --- | ---- |
    | 0 | 0 | 0   | 0   | 0    |
    | 0 | 0 | 1   | 1   | 0    |
    | 0 | 1 | 0   | 1   | 0    |
    | 0 | 1 | 1   | 0   | 1    |
    | 1 | 0 | 0   | 1   | 0    |
    | 1 | 0 | 1   | 0   | 1    |
    | 1 | 1 | 0   | 0   | 1    |
    | 1 | 1 | 1   | 1   | 1    |


    Bit layout

    The Consistent Rule.

    Inputs
    We will store inputs as:

    bit 2 = A
    bit 1 = B
    bit 0 = Cin

    So:
    0b000 means A=0 B=0 Cin=0
    0b101 means A=1 B=0 Cin=1

    Outputs
    We will store outputs as:

    bit 1 = Sum
    bit 0 = Cout

    So:
    0b10 means Sum=1 Cout=0
    0b01 means Sum=0 Cout=1
    0b11 means Sum=1 Cout=1
*/

ttrow_t fulladder_rows[] = {
    {.input = 0b000, .output = 0b00},// 0+0+0=Sum 0, Cout 0
    {.input = 0b001, .output = 0b10},// 0+0+1=Sum 1, Cout 0
    {.input = 0b010, .output = 0b10},// 0+1+0=Sum 1, Cout 0
    {.input = 0b011, .output = 0b01},// 0+1+1=Sum 0, Cout 1
    {.input = 0b100, .output = 0b10},// 1+0+0=Sum 1, Cout 0
    {.input = 0b101, .output = 0b01},// 1+0+1=Sum 0, Cout 1
    {.input = 0b110, .output = 0b01},// 1+1+0=Sum 0, Cout 1
    {.input = 0b111, .output = 0b11} // 1+1+1=Sum 1, Cout 1
};

char *fa_inputs[] = { "A", "B", "Cin" };
char *fa_outputs[] = { "Sum", "Cout" };

truthtbl_t fulladder_table = {
    .name = "Full Adder",
    .input_sz = 3,
    .output_sz = 2,
    .row_sz = 8,
    .labels_in = fa_inputs,
    .labels_out = fa_outputs,
    .rows = fulladder_rows
};

static inline uint8_t fulladder_compute(uint8_t a, uint8_t b, uint8_t cin){
    uint8_t sum  = a ^ b ^ cin;
    uint8_t cout = (a & b) | (cin & (a ^ b));
    return (sum << 1) | cout;
}

static inline truthtbl_t *truthtbl_init(const lgcdev_t *dev){
    if (!dev || !dev->compute) return;
    if (dev->input_sz >= 63 || dev->output_sz > 63) return;
    
    truthtbl_t *tt = (truthtbl_t*)malloc(sizeof(truthtbl_t));
    if (!tt) return;
    tt->name = dev->name;
    tt->input_sz = dev->input_sz;
    tt->output_sz = dev->output_sz;
    tt->row_sz = 1u << dev->input_sz;
    tt->labels_in = dev->labels_in;
    tt->labels_out = dev->labels_out;

    tt->rows = (ttrow_t*)(tt->row_sz * sizeof(truthtbl_t));
    if (!tt->rows) free(tt); return;

    ttbitpattern_t mask = (dev->output_sz == 64) ?
        0xFFFFFFFFu : ((1u << dev->output_sz) - 1u);
    
    for (size_t i = 0; i < tt->row_sz; ++i){
        tt->rows[i].input = (ttbitpattern_t)i;
        tt->rows[i].output = dev->compute((ttbitpattern_t)i) & mask;
    }
    return tt;
}

void ttable_free(truthtbl_t *tt){ if (!tt) return; free(tt->rows); free(tt); }

static inline void truthtbl_print(const truthtbl_t *tt){
    if (!tt) return;
    printf("%s\n", tt->name);
    printf("---------------------------------\n");

    for (size_t i = 0; i < tt->input_sz; i++) {
        printf("%s ", tt->labels_in[i]);
    }
    printf("| ");

    for (size_t i = 0; i < tt->output_sz; i++) {
        printf("%s ", tt->labels_out[i]);
    }
    printf("\n");
    printf("---------------------------------\n");

    for (size_t r = 0; r < tt->row_sz; r++) {
        uint8_t in  = tt->rows[r].input;
        uint8_t out = tt->rows[r].output;

        for (size_t i = 0; i < tt->input_sz; i++) {
            size_t bit_index = tt->input_sz - 1 - i;
            printf("%u ", (in >> bit_index) & 1);
        }
        printf("| ");

        for (size_t i = 0; i < tt->output_sz; i++) {
            size_t bit_index = tt->output_sz - 1 - i;
            printf("%u ", (out >> bit_index) & 1);
        }
        printf("\n");
    }
}

#endif /* JTRUTH_IMPL */