/*
 * io.c -- switch and button module
 *
 */

#include "io.h"
#include "gic.h"		/* interrupt controller interface */

#define CHANNEL1 1							/* channel 1 of the GPIO port */
#define INPUT 0xFFFFFFFF

static void (* btn_saved_callback_function)(u32 btn);
static void (* sw_saved_callback_function)(u32 sw);
static XGpio btnport;	       /* btn GPIO port instance */
static XGpio swport;
static bool pushed = false; 			// true when a button goes from low to high


static void btn_handler(void *devicep) {
	/* coerce the generic pointer into a gpio */
	XGpio *dev = (XGpio*)devicep;

	u32 btn_num;

	if (!pushed){
		btn_num =  XGpio_DiscreteRead(dev,CHANNEL1);
		pushed = true;
		/*
		 * btn 0 = 0x1 = 1
		 * btn 1 = 0x10 = 2
		 * btn 2 = 0x100 = 4
		 * btn 3 = 0x1000 = 8
		 */
		btn_saved_callback_function(btn_num);
	}else{
		pushed = false;
	}

	XGpio_InterruptClear(dev, XGPIO_IR_CH1_MASK);
}




/*
 * initialize the btns providing a callback
 */
void io_btn_init(void (*btn_callback)(u32 btn)) {

	btn_saved_callback_function = btn_callback;
	gic_init();

	/* initialize btnport (c.f. module 1) and immediately dissable interrupts */
	if(XGpio_Initialize(&btnport, XPAR_AXI_GPIO_1_DEVICE_ID)!=XST_SUCCESS) {
		printf("Unable to initialize btn GPIO(1)");
		return;
	}
	// disable global interrupt to the processor
	XGpio_InterruptGlobalDisable(&btnport);

	/*
	* Mask is the mask to disable. Bits set to 1 are disabled. This
	* mask is formed by OR'ing bits from XGPIO_IR* bits which are
	* contained in xgpio_l.h.
	*/
	XGpio_InterruptDisable(&btnport, XGPIO_IR_CH1_MASK);

	//set tri state buffer to input
	XGpio_SetDataDirection(&btnport, CHANNEL1, INPUT);

	/* connect handler to the gic (c.f. gic.h) */
	if( gic_connect(XPAR_FABRIC_GPIO_1_VEC_ID, btn_handler, &btnport) != XST_SUCCESS) {
	  printf("Unable to connect btn int to gic");
	  return;
	}
	/* enable interrupts on channel (c.f. table 2.1) */
	XGpio_InterruptEnable(&btnport, XGPIO_IR_CH1_MASK);

	/* enable interrupt to processor (c.f. table 2.1) */
	XGpio_InterruptGlobalEnable(&btnport);
}



/*
 * close the btns
 */
void io_btn_close(void) {
	  /* disconnect the interrupts (c.f. gic.h) */
	  gic_disconnect(XPAR_FABRIC_GPIO_1_VEC_ID);
	  gic_close();
}


/*
 * initialize the switches providing a callback
 */
void io_sw_init(void (*sw_callback)(u32 sw)) {
//	sw_saved_callback_function = sw_callback;
//
//	/* initialize btnport (c.f. module 1) and immediately dissable interrupts */
//	if(XGpio_Initialize(&swport, XPAR_AXI_GPIO_2_DEVICE_ID)!=XST_SUCCESS) {
//		printf("Unable to initialize sw GPIO(2)");
//		return;
//	}
//	// disable global interrupt to the processor
//	XGpio_InterruptGlobalDisable(&swport);
//
//	/*
//	* Mask is the mask to disable. Bits set to 1 are disabled. This
//	* mask is formed by OR'ing bits from XGPIO_IR* bits which are
//	* contained in xgpio_l.h.
//	*/
//	XGpio_InterruptDisable(&swport, XGPIO_IR_CH1_MASK);
//
//	//set tri state buffer to input
//	XGpio_SetDataDirection(&swport, CHANNEL1, INPUT);
//
//	/* connect handler to the gic (c.f. gic.h) */
//	if( gic_connect(XPAR_FABRIC_GPIO_2_VEC_ID, btn_handler, &btnport) != XST_SUCCESS) {
//	  printf("Unable to connect btn int to gic");
//	  return;
//	}
//	/* enable interrupts on channel (c.f. table 2.1) */
//	XGpio_InterruptEnable(&swport, XGPIO_IR_CH1_MASK);
//
//	/* enable interrupt to processor (c.f. table 2.1) */
//	XGpio_InterruptGlobalEnable(&swport);
	return;
}

/*
 * close the switches
 */
void io_sw_close(void) {
	/* disconnect the interrupts (c.f. gic.h) */
	//gic_disconnect(XPAR_FABRIC_GPIO_2_VEC_ID);
	return;
}
