#pragma once
#ifndef JCHIP_H
#define JCHIP_H

#define PIN_MAX 64

typedef enum PinState {
    PIN_LOW = 0,
    PIN_HIGH = 1,
    PIN_NULL,
    PIN_ERR
} pinstate_t;

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
    CHIP_KIND_EPROM,
    CHIP_KIND_CLOCK,
    CHIP_KIND_TIMER,
    CHIP_KIND_RESET,
    CHIP_KIND_ANALOG,
    CHIP_KIND_COUNTER,


    CHIP_KIND_CUSTOM,


} chipkind_t;

typedef struct Pin {
    const char *name;
    pinstate_t state;
} pin_t;

typedef struct Chip {
    chipkind_t kind;
    pin_t pins[PIN_MAX];
} chip_t;

#endif /* JCHIP_H */
#define JCHIP_IMPL // #debug-mode
#ifdef JCHIP_IMPL



#endif /* JCHIP_IMPL */