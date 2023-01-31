/*
 * gic.c  -- interrupt controller module
 *
 * Caroline Vanacore
 */
#include "gic.h"

/*
 * Private Variables hidden by this module
 */
static XScuGic gic;					/* the gic instance */
static XScuGic_Config *gic_config;	/* the gic configuration */


/*
 * Public Interface
 */

/*
 * Initialize the gic
 */
s32 gic_init(void) {
	/* lookup the gic */
	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	/* initialize it */
	if(XScuGic_CfgInitialize(&gic,gic_config,gic_config->CpuBaseAddress) != XST_SUCCESS)
		return XST_FAILURE;
	/* register the exception handler */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&gic);
	/* enable exceptions */
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

/*
 * Connect an interrupt id to handler and device
 */
s32 gic_connect(u32 id, Xil_InterruptHandler handler,  void *devp) {
	/* associate handler with the interrupt id */
	if(XScuGic_Connect(&gic,id,handler,devp) != XST_SUCCESS)
		return XST_FAILURE;
	/* enable the interrupt at the gic */
	XScuGic_Enable(&gic, id);
	return XST_SUCCESS;
}

/*
 * Disconnect an interrupt id
 */
void gic_disconnect(u32 id) {
	XScuGic_Disconnect(&gic,id);
	XScuGic_Disable(&gic,id);
}

/*
 * Close the gic
 */
void gic_close(void) {
	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);
	XScuGic_Stop(&gic);
}

