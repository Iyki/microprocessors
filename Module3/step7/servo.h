/*
 * servo.h
 */
#pragma once

#include <stdio.h>
#include "xtmrctr.h"
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */

/*
 * Initialize the servo, setting the duty cycle to 7.5%
 */
void servo_init(void);

/*
 * Set the dutycycle of the servo
 */
void servo_set(double dutycycle);



