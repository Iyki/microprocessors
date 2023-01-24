/*
 * ttc.c
 * NOTE: The TTC hardware must be enabled (Timer 0 on the processing system) before it can be used!!
 * XPAR_XTTCPS_0_DEVICE_ID = device id
 * XPAR_XTTCPS_0_INTR = interrupt id
 * XTTCPS_IXR_INTERVAL_MASK = interrupt mask for interval mode
 * XTTCPS_OPTION_INTERVAL_MODE = interval mode mask for setting mode
 */

#include "ttc.h"
#include "gic.h"
#include <stdio.h>


static XTtcPs ttc;
static XTtcPs_Config *ttc_config;
static void (*saved_ttc_callback)(void);



// called when timer reach counter
static void ttc_handler(void *devicep) {

	u32 interrupt_status;

	XTtcPs *ttc = (XTtcPs*)devicep;
	interrupt_status = XTtcPs_GetInterruptStatus(ttc);

	saved_ttc_callback();

	XTtcPs_ClearInterruptStatus(ttc, interrupt_status);
}


/*
 * ttc_init -- initialize the ttc freqency and callback
 */
void ttc_init(u32 freq, void (*ttc_callback)(void)){

	u8 prescaler; // initilized in
	XInterval interval;

	// save call back function
	saved_ttc_callback = ttc_callback;

	// find configs for timer
	ttc_config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_DEVICE_ID);

	//initialize ttc
	if (XTtcPs_CfgInitialize(&ttc, ttc_config, ttc_config -> BaseAddress) != XST_SUCCESS){
		printf("Unable to initialize ttc");
		fflush(stdout);
		return;
	}

	// disable interrupts to not encounter any interrupts before setting up interrupts handler
	/*
	 * From xttcps.h
	 * InterruptMask defines which interrupt should be enabled.
	 * Constants are defined in xttcps_hw.h as XTTCPS_IXR_*.
	 * This is a bit mask, all set bits will be enabled, cleared bits
	 * will not be disabled.
	 */
	XTtcPs_DisableInterrupts(&ttc, XTTCPS_IXR_INTERVAL_MASK);

	// connect to interrupts controller
		// XPAR_XTTCPS_0_INTR from xparameters_ps.h
	if (gic_connect(XPAR_XTTCPS_0_INTR, ttc_handler,  (void *)&ttc) != XST_SUCCESS){
		printf("Unable to connect ttc to gic");
		fflush(stdout);
		return;
	}

	// find and set prescaler and interval
	XTtcPs_CalcIntervalFromFreq(&ttc, freq, &interval, &prescaler);
	XTtcPs_SetPrescaler(&ttc,prescaler);
	XTtcPs_SetInterval(&ttc,interval);

	// set to interval mode
	XTtcPs_SetOptions(&ttc, XTTCPS_OPTION_INTERVAL_MODE);
}

/*
 * ttc_start -- start the ttc
 */
void ttc_start(void){

	// enable interrupts
	XTtcPs_EnableInterrupts(&ttc, XTTCPS_IXR_INTERVAL_MASK);

	// start times
	XTtcPs_Start(&ttc);
}

/*
 * ttc_stop -- stop the ttc
 */
void ttc_stop(void){

	// disable interrupts
	XTtcPs_DisableInterrupts(&ttc,XTTCPS_IXR_INTERVAL_MASK);

	// stop timer
	XTtcPs_Stop(&ttc);
}

/*
 * ttc_close -- close down the ttc
 */
void ttc_close(void){

	// disconnect from the giv
	gic_disconnect(XPAR_XTTCPS_0_INTR);
}
