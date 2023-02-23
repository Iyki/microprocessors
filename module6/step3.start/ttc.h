/*
 * ttc.h
 *
 * NOTE: The TTC hardware must be enabled (Timer 0 on the processing system) before it can be used!!
 *
 */
#pragma once

#include <stdio.h>
#include "xttcps.h"
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */

/*
 * ttc_init -- initialize the ttc freqency and callback
 */
void ttc_init(u32 freq, void (*ttc_callback)(void));

/*
 * ttc_start -- start the ttc
 */
void ttc_start(void);

/*
 * ttc_stop -- stop the ttc
 */
void ttc_stop(void);

/*
 * ttc_close -- close down the ttc
 */
void ttc_close(void);
