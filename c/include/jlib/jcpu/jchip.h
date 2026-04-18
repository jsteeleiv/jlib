#pragma once
#ifndef JCHIP_H
#define JCHIP_H

#include <stdlib.h>

#include "../jdata/jmap.h"
#include "../jtype.h"

#define PIN_MAX 64

typedef enum PinType {
    PIN_VCC = 0,
    PIN_GND,
    PIN_A,
    PIN_B,
    PIN_Y
} pintype_t;

typedef enum PinState {
    PIN_LOW = 0,
    PIN_HIGH = 1,
    PIN_X,
    PIN_Z,
    PIN_IRQ,
    PIN_ERR,
    PIN_NULL, /* passive || dead || unused */
} pinstate_t;

typedef enum PinDirection {
    PIN_DIR_NONE = 0,
    PIN_DIR_IN,
    PIN_DIR_OUT,
    PIN_DIR_BI,  /* bi-directional -> in|out */
    PIN_DIR_NO,  /* inactive drive logic, for oscillator/passive nodes */
    PIN_DIR_PWR, /* VCC, GND, maybe VEE */
    PIN_DIR_TAIL
} pindir_t;

typedef enum PinFlags {
    PIN_FLAG_NONE       = 0,
    PIN_FLAG_BAR        = 1 << 0, /* active low */
    PIN_FLAG_CLOCK      = 1 << 1, /* clock input/output */
    PIN_FLAG_LATCHED    = 1 << 2, /* sampled/latching significance */
    PIN_FLAG_TRISTATE   = 1 << 3, /* can enter Z state */
    PIN_FLAG_PULLUP     = 1 << 4, /* weak pull-up */
    PIN_FLAG_PULLDOWN   = 1 << 5, /* weak pull-down */
    PIN_FLAG_OPTIONAL   = 1 << 6, /* not always used */
    PIN_FLAG_NC         = 1 << 7, /* no-connect */
    PIN_FLAG_ANALOG     = 1 << 8, /* analog-capable */
} pinflags_t;

typedef struct Pin {
    pinflags_t flags;  /* pin flags [PIN_FLAG_*] */
    const char *name;  /* "A0", "D7", "RW", "RESET" */
    pinstate_t state;  /* current visible signal state */
    pinstate_t next;   /* next state for staged updates */
    uint64_t number;   /* physical package pin number */
    // uint64_t mask;     /* bitmask [?] */
    pintype_t type;
    pindir_t dir;      /* IN / OUT / BI / etc */
    void *wire;        /* optional external connection */
} pin_t;

#define PIN_IS_BAR(pin) (((pin)->mask & PIN_FLAG_BAR) != 0)
#define PIN_IS_IN(pin)  ((pin)->dir == PIN_DIR_IN)
#define PIN_IS_OUT(pin) ((pin)->dir == PIN_DIR_OUT)
#define PIN_IS_BI(pin)  ((pin)->dir == PIN_DIR_BI)

static inline void pin_commit(pin_t *pin);
static inline int pin_read_assert(pin_t *pin);
static inline int pin_asserted(const pin_t *pin);
static inline pinstate_t pin_invert(pinstate_t s);
static inline pinstate_t pin_read(const pin_t *pin);
static inline void pin_set(pin_t *pin, pinstate_t state);
static inline void pin_write_assert(pin_t *pin, int asserted);
static inline void pin_init(pin_t *pin, const char *name, uint64_t num,
    pindir_t dir, pinflags_t flags
);

typedef enum ChipKind {
    /* DOIT: sort these by category ... */
    CHIP_KIND_NONE = 0,
    CHIP_KIND_IO,
    CHIP_KIND_CPU,
    CHIP_KIND_APU,
    CHIP_KIND_MCU,
    CHIP_KIND_NPU,
    CHIP_KIND_TTL,
    CHIP_KIND_RAM,
    CHIP_KIND_ROM,
    CHIP_KIND_LCD,
    CHIP_KIND_I2C,
    CHIP_KIND_USB,
    CHIP_KIND_PLD,  // PAL, PLA
    CHIP_KIND_BLUE,
    CHIP_KIND_WIFI,
    CHIP_KIND_CMOS,
    CHIP_KIND_FPGA,
    CHIP_KIND_PMIC,
    CHIP_KIND_UART,
    CHIP_KIND_SATA,
    CHIP_KIND_CLOCK,
    CHIP_KIND_TIMER,
    CHIP_KIND_RESET,
    CHIP_KIND_ANALOG,
    /* ~~~~~~~~~~~ */
    CHIP_KIND_CUSTOM,
    /* ~~~~~~~~~~~ */
    CHIP_KIND_TAIL
} chipkind_t;

typedef enum ChipType {
    /* TTL */
    CHIP_TYPE_NONE = 0,
    CHIP_TYPE_NAND,
    CHIP_TYPE_NOR,
    CHIP_TYPE_AND,
    CHIP_TYPE_OR,
    CHIP_TYPE_XOR,
    CHIP_TYPE_NOT,
    /* flip flops , etc ... */
    CHIP_TYPE_DFLIP,
    CHIP_TYPE_JKFLIP,
    CHIP_TYPE_LATCH,
    CHIP_TYPE_SHIFTREG,
    /* counters */
    CHIP_TYPE_BINARYCOUNTER,
    CHIP_TYPE_RINGCOUNTER,
    /* memory types */
    CHIP_TYPE_EPROM,
    CHIP_TYPE_DRAM,
    CHIP_TYPE_SRAM,
    CHIP_TYPE_NVRAM,
    /* cpu families */
    CHIP_TYPE_6502,
    CHIP_TYPE_Z80,
    CHIP_TYPE_X86,
    CHIP_TYPE_X64,
    CHIP_TYPE_ARM,
    CHIP_TYPE_RASP,
    CHIP_TYPE_RISCV,
    /* displays || LCD's */
    CHIP_TYPE_VGA,
    CHIP_TYPE_HD44780,
    CHIP_TYPE_SEVENSEG,
    /* bus helpers */
    CHIP_TYPE_MUX,
    CHIP_TYPE_DEMUX,
    CHIP_TYPE_TRANSX,
    CHIP_TYPE_DECODER,
    /* ~~~~~~~~~~~ */
    CHIP_TYPE_CUSTOM,
    /* my types */
    CHIP_TYPE_JNES,
    /* ~~~~~~~~~~~ */
    CHIP_TYPE_TAIL,
} chiptype_t;

typedef enum ChipSeries {
    CHIP_SRES_NONE = 0,
    CHIP_SRES_4XXX,
    CHIP_SRES_555X,
    CHIP_SRES_65XX,
    CHIP_SRES_68XX,
    CHIP_SRES_74XX,
    CHIP_SRES_HDXX,
    CHIP_SRES_VIRT,
    /* ~~~~~~~~~ */
    CHIP_SRES_TAIL,
} chipsrs_t;

typedef enum ChipPackage {
    CHIP_PKG_NONE = 0,
    CHIP_PKG_DIP8,
    CHIP_PKG_DIP14,
    CHIP_PKG_DIP16,
    CHIP_PKG_DIP20,
    CHIP_PKG_DIP24,
    CHIP_PKG_DIP28,
    CHIP_PKG_DIP40,
    CHIP_PKG_SOIC,
    CHIP_PKG_SSOP,
    CHIP_PKG_TSSOP,
    CHIP_PKG_PLCC,
    CHIP_PKG_QFP,
    CHIP_PKG_TQFP,
    CHIP_PKG_QFN,
    CHIP_PKG_BGA,
    CHIP_PKG_MODULE,
    CHIP_PKG_VIRTUAL,
    CHIP_PKG_TAIL,
} chippkg_t;

typedef enum ChipAttributes {
    JCHIP_ATTR_NONE        = 0,
    JCHIP_ATTR_TRISTATE    = 1 << 0,
    JCHIP_ATTR_CLOCKED     = 1 << 1,
    JCHIP_ATTR_EDGETRIGGER = 1 << 2,
    JCHIP_ATTR_LEVELSENSE  = 1 << 3,
    JCHIP_ATTR_BIDIRBUS    = 1 << 4,
    JCHIP_ATTR_ANALOG      = 1 << 5
} chipattr_t;

typedef struct ChipVoltage {
    float vcc_nom; /* nominal */
    float vcc_min;
    float vcc_max;
    /* voltage in, low/high */
    float vil_max; 
    float vih_min;
    /* voltage out, low/high */
    float vol_max;
    float voh_min;
    typedef union {
        uint64_t rising;
        int64_t falling;
    } edge;
} chipvolt_t;

typedef enum ChipTiming {
    CHIP_TIME_NONE = 0,
    CHIP_TIME_TICK,
    CHIP_TIME_CYCLE,
    CHIP_TIME_SEQUE,
    CHIP_TIME_SYNC,
    CHIP_TIME_ASYNC,
    CHIP_TIME_COMBO,
    CHIP_TIME_FUNCTIONAL,
    CHIP_TIME_ANALOG_APPROX,
    CHIP_TIME_EDGE_TRIGGERED,
    CHIP_TIME_PROP_DELAY,
    /* ~~~~~~~~~~~~~~~~~~~~~ */

    CHIP_TIME_TAIL,
} chiptime_t;

typedef enum ChipLogic {
    CHIP_LOGIC_DEASSERTED = 0,
    CHIP_LOGIC_ASSERTED   = 1,
    CHIP_LOGIC_FLOATING   = 2,
    CHIP_LOGIC_UNKNOWN    = 3
} chiplgc_t;

static inline chiplgc_t chip_read_logic(const pin_t *pin);

typedef struct ChipInformation {
    chipkind_t kind;
    chiptype_t type;
    chipsrs_t srs;
    chippkg_t pkg;
    chipattr_t attr;
    chipvolt_t volt;
    chiptime_t time;
    string_t string;
} chipinfo_t;

typedef struct ChipConfiguration {
    pin_t pins[PIN_MAX];
    chipinfo_t info;
    map_t pinout;
    
} chipconf_t;


typedef struct Chip {
    chipconf_t conf;
    cstr_t partnum;
    cstr_t output;
    void *impl;
    // void (*init)(struct jchip *chip);
    // void (*reset)(struct jchip *chip);
    // void (*tick)(struct jchip *chip);
    // jchip_pin_state (*read_pin)(struct jchip *chip, int pin);
    // void (*write_pin)(struct jchip *chip, int pin, jchip_pin_state state);
} chip_t;

#endif /* JCHIP_H */
#define JCHIP_IMPL // #debug-mode
#ifdef JCHIP_IMPL


/* Pin Usage:
---------
    pin_t pins[PIN_TAIL];

    pin_init(&pins[PIN_A],   "A",   1, PIN_DIR_IN,  PIN_FLAG_NONE);
    pin_init(&pins[PIN_B],   "B",   2, PIN_DIR_IN,  PIN_FLAG_NONE);
    pin_init(&pins[PIN_Y],   "Y",   3, PIN_DIR_OUT, PIN_FLAG_NONE);
    pin_init(&pins[PIN_VCC], "VCC", 14, PIN_DIR_PWR, PIN_FLAG_NONE);
    pin_init(&pins[PIN_GND], "GND", 7,  PIN_DIR_PWR, PIN_FLAG_NONE);

    If one pin were /RESET:

    chip_pin_init(&pins[PIN_RESET], "RESET", 40, CHIP_PDIR_IN, CHIP_PFL_BAR);

    and then:

    if (chip_pin_is_asserted(&pins[PIN_RESET])) {
        ... reset is active 
    }
*/

/* if
-----
    (pin_asserted(&chip->pins[PIN_RESET])) {
       do stuff ...

    INSTEAD OF:
    if (reset_is_bar ? state == LOW : state == HIGH)
}*/
static inline int pin_asserted(const pin_t *pin){
    if (!pin) return 0;
    if (pin->flags & PIN_FLAG_BAR) {
        return pin->state == PIN_LOW;
    }
    return pin->state == PIN_HIGH;
}

/* write
--------
    pin_write_assert(&chip->pins[PIN_IRQ], 1);
*/
static inline void pin_write_assert(pin_t *pin, int asserted){
    if (!pin) return;
    if (pin->state != PIN_LOW && pin->state != PIN_HIGH) return;
    // if (pin->flags & PIN_FLAG_BAR) {
    //     pin->next = asserted ? PIN_LOW : PIN_HIGH;
    // } else {
    //     pin->next = asserted ? PIN_HIGH : PIN_LOW;
    // }
    /* ^ branching
    OH NO!
       v branchless ... oh yea ... from like 5 lines -> 2 lines */
    unsigned int isbar = (pin->flags & PIN_FLAG_BAR) != 0;
    pin->next = (pinstate_t)(asserted ^ isbar);
    /* why this works ^ 
    ... XOR truth table
    -------------------
    | asserted | isbar  | result   |
    | -------- | ------ | -------- |
    | 0        | 0      | 0 (LOW)  |
    | 1        | 0      | 1 (HIGH) |
    | 0        | 1      | 1 (HIGH) |
    | 1        | 1      | 0 (LOW)  |

    */


}

static inline void pin_init(pin_t *pin, const char *name, uint64_t num,
  pindir_t dir, pinflags_t flags){

    if (!pin) return;

    pin->name   = name;
    pin->number = num;
    pin->dir    = dir;
    pin->flags  = flags;
    pin->state  = PIN_X;
    pin->next   = PIN_X;
    pin->wire   = NULL;
}

static inline pinstate_t pin_invert(pinstate_t s){
    return (pinstate_t)(s ^ 1);
}

static inline void pin_commit(pin_t *pin){
    if (!pin) return;
    pin->state = pin->next;
}

static inline void pin_set(pin_t *pin, pinstate_t state){
    if (!pin) return;
    pin->next = state;
}

static inline pinstate_t pin_read(const pin_t *pin){
    if (!pin) return PIN_X;
    return pin->state;
}

static inline int pin_read_assert(pin_t *pin){
    if (!pin) return 0;
    if (pin->state != PIN_LOW && pin->state != PIN_HIGH) return 0; 
    uint isbar = (pin->flags & PIN_FLAG_BAR) != 0;
    return ((uint)pin->state ^ isbar) & 1u;
}




static inline chiplgc_t chip_read_logic(const pin_t *pin){
    if (!pin) return CHIP_LOGIC_UNKNOWN;

    switch (pin->state) {
    case PIN_LOW:
    case PIN_HIGH: {
        uint is_bar = (pin->flags & PIN_FLAG_BAR) != 0;
        return (chiplgc_t)(((uint)pin->state ^ is_bar) & 1u);
    }
    case PIN_Z:
        return CHIP_LOGIC_FLOATING;
    case PIN_X:
    default:
        return CHIP_LOGIC_UNKNOWN;
    }
}




#endif /* JCHIP_IMPL */
