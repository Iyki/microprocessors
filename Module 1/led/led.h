/*
 * led.h -- led module interface
 *
 */
#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <xgpio.h>		  	/* axi gpio */
#include <xgpiops.h>		/* processor gpio */
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */

/* led states */
#define LED_ON true
#define LED_OFF false

#define ALL 0xFFFFFFFF		/* A value designating ALL leds */

/*
 * Initialize the led module
 */
void led_init(void);

/*
 * Set <led> to one of {LED_ON,LED_OFF,...}
 *
 * <led> is either ALL or a number >= 0
 * Does nothing if <led> is invalid
 */
void led_set(u32 led, bool tostate);

/*
 * Get the status of <led>
 *
 * <led> is a number >= 0
 * returns {LED_ON,LED_OFF,...}; LED_OFF if <led> is invalid
 */
bool led_get(u32 led);

/*
 * Toggle <led>
 *
 * <led> is a value >= 0
 * Does nothing if <led> is invalid
 */
void led_toggle(u32 led);

