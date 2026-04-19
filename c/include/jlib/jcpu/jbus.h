#pragma once
#ifndef JBUS_H
#define JBUS_H

#include <stdlib.h>
#include <stdbool.h>

#include "jaddr.h"
#include "../jtype/jbyte.h"

typedef enum BusSignals {
    BUS_SIG_WRITE = 0,
    BUS_SIG_READ,
    BUS_SIG_SYNC,
    BUS_SIG_READY,
} bussig_t;

typedef enum jbus_dev_kind {
    BUS_DEV_NONE = 0,
    BUS_DEV_RAM,
    BUS_DEV_ROM,
    BUS_DEV_CPU,
    BUS_DEV_GPU,
    BUS_DEV_PPU,
    BUS_DEV_SSD,
    BUS_DEV_MMIO
} jbus_dev_kind_t;

typedef bool (*bus_read8fn)(void *ctx, addr_t addr, uint8_t *val);
typedef bool (*bus_write8fn)(void *ctx, addr_t addr, uint8_t val);

typedef struct BusDevice {
    const char *name;
    void *context;
    bus_read8fn read;
    bus_write8fn write;
    uint32_t flags;
} busdev_t;

typedef struct BusMap {
    addrrange_t range;
    busdev_t *device;
    char *console;
} busmap_t;

typedef struct Bus {
    busmap_t *maps;
    intptr_t addr;
    byte_t *data;
    size_t len;
    size_t cap;
    uint32_t flags;
    const char *name;
} bus_t;

typedef struct AddressBus {
    bus_t wire;
} addrbus_t;

typedef struct DataBus {
    bus_t wire;
}databus_t;

static inline bool bus_init(bus_t *bus, busmap_t *maps, size_t cap);
static inline void bus_reset(bus_t *bus);
static inline bool bus_map(bus_t *bus, addrrange_t rng, busdev_t dev);
static inline bool bud_unmap(bus_t *bus, addrrange_t rng);
static inline busmap_t bus_find(bus_t *bus, addr_t addr);
static inline const busmap_t bus_find_const(const bus_t *bus_t, addr_t addr);
static inline bool bus_overlaps(bus_t *bus, addrrange_t rng);
static inline bool bus_read8(bus_t *bus, addr_t addr, uint8_t *val);
static inline bool bus_write8(bus_t *bus, addr_t addr, uint8_t val);
static inline bool bus_can_read(const bus_t *bus, addr_t addr);
static inline bool bus_can_write(const bus_t *bus, addr_t addr);

#endif /* JBUS_H */
// #define JBUS_IMPL // #debug-mode
#ifdef JBUS_IMPL



#endif /* JBUS_IMPL */