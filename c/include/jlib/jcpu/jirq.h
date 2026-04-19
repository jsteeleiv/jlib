#pragma once
#ifndef JIRQ_H
#define JIRQ_H

#include "../jtype.h"

/*
jirq.h should be about the layer between:

devices/peripherals/timers
and
the CPU interrupt input pins

external interrupt sources
interrupt lines
masking
priorities
acknowledgement
pending state
controller behavior


interrupt sources
interrupt lines
interrupt controllers
interrupt routing
interrupt pending bits
interrupt masks
interrupt acknowledge/clear operations


IRQ line state
NMI line state
source assertion/deassertion
mask registers
pending registers
in-service state
interrupt priority
edge-triggered vs level-triggered behavior
acknowledge/end-of-interrupt signaling
dispatch selection
controller-side vector selection
fan-in from multiple sources to fewer CPU lines

1. Interrupt source

    A thing that can request service.

    Examples:

    timer
    UART
    keyboard interface
    VIA/PIA
    external button
    DMA engine

evel-triggered (IRQ)
Active as long as LOW
Edge-triggered (NMI)
Triggered on transition:

*/


/* IrqLine Kind ...*/
typedef enum IrqSignals {
    IRQ_SIG_IRQ = 0, /* (Interrupt Request) – maskable | level triggered */
    IRQ_SIG_NMI,     /* (Non-Maskable Interrupt) – edge-triggered, NO disabling */
    IRQ_SIG_RES,     /* (Reset) – forces CPU into known startup state */
    IRQ_SIG_FIQ,     /* fast interrupt request */
    IRQ_SIG_IRQ0,
    IRQ_SIG_IRQ1,
    IRQ_SIG_MYN,     /* my custom signal | to be used later... */
} irqsig_t;

/* Interrupt line
~~~~~~~~~~~~~~~~~
    A physical/logical line carrying an interrupt condition.

    Examples:
        IRQ
        NMI
        IRQ0
        IRQ1
        shared interrupt bus line
*/
typedef struct IrqLine {
    irqsig_t signal;
    uint asserted : 1;
    uint masked   : 1;
    uint latched  : 1;
    uint trigger;
} irqline_t;

typedef enum IrqTrigger {
    IRQ_TRIG_LVL_LOW = 0,
    IRQ_TRIG_LVL_HIGH,
    IRQ_TRIG_EDGE_FALL,
    IRQ_TRIG_EDGE_RISE,
} irqtrig_t;

/* Interrupt source
~~~~~~~~~~~~~~~~~~~
    A thing that can request service.

    Examples:
        timer
        UART
        keyboard interface
        VIA/PIA
        external button
        DMA engine
*/
typedef struct IrqSource {
    irqtrig_t trigger;
    irqline_t target;
    uint identity;
    uint priority : 8;
    uint vector   :16;
    /* bitfields */
    uint enabled  : 1;
    uint pending  : 1;
    uint active   : 1;
    uint level    : 1;
    uint edge     : 1;
    uint state    : 1;
    void *user;
} irqsrc_t;

/* Interrupt controller
~~~~~~~~~~~~~~~~~~~~~~~
    The thing that collects requests and presents them to the CPU-facing line(s).

    Examples:
        discrete glue logic
        a PIC-like chip
        custom priority encoder
        6522-style interrupt flag/mask arrangement
        your own breadboard interrupt manager
*/
typedef struct IrqController {
    irqsrc_t *sources;
    uint source_sz;
    irqline_t irq;
    irqline_t nmi;
    uint irq_asserted;
    uint nmi_asserted;
} irqctrl_t;

// uint (*select_vec)(struct irqctrl_t *ctl);
// void (*ack)(struct irqctrl_t *ctl, uint src_id);
/*
void jirq_ack(jirq_controller *ctl, unsigned source_id);
void jirq_clear(jirq_controller *ctl, unsigned source_id);
void jirq_eoi(jirq_controller *ctl, unsigned source_id);
*/

#endif /* JIRQ_H */

#ifdef JIRQ_IMPL



#endif