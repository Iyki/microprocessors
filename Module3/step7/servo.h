/*
 * servo.h
 */
#pragma once

#include <stdio.h>
#include "xtmrctr.h"
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */

// hardware 
#define CLOCK_FREQ 50000000

// timer 0 settings
#define PERIOD 1000000 // 20ms period (Timer 0)
#define PERIOD_MS 20 // 20ms period (Timer 0)

/*
 * Initialize the servo, setting the duty cycle to 7.5%
 */
void servo_init(void);

/*
 * Set the dutycycle of the servo
 */
void servo_set(double dutycycle);



