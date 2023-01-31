/*
 * gic.h -- The GIC module interface
 */
#pragma once

#include "xparameters.h"    /* device details */
#include "xil_exception.h"  /* exception handling */
#include "xil_types.h"		/* types used by xilinx */
#include "xscugic.h"		/* gic details */
#include "xgpio.h"			/* axi gpio details */
#include "xuartps.h"		/* ps uart details */

/*
 * Initialize the gic
 *
 * returns XST_SUCCESS on success; otherwise XST_FAILURE
 */
s32 gic_init(void);

/*
 * Connect an interrupt id to a handler and device
 *
 * returns XST_SUCCESS on success; otherwise XST_FAILURE
 */
s32 gic_connect(u32 id, Xil_InterruptHandler handler,  void *devp);

/*
 * Disconnect an interrupt id
 *
 * id - an interrupt id
 */
void gic_disconnect(u32 id);

/*
 * Close the gic
 */
void gic_close(void);
