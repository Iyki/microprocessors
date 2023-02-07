
#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

#include "platform.h"		/* ZYBO board interface */
#include "xil_types.h"		/* u32, s32 etc */
#include "xparameters.h"	/* constants used by hardware */

#include "gic.h"		/* interrupt controller interface */
#include "xgpio.h"		/* axi gpio interface */
#include "led.h" 		// led interface
#include "io.h" //




static XUartPs uart_ps_1;
static bool done; // end of program



static void Uart1_Handler( void *CallBackRef, u32 Event, unsigned int EventData) {

	XUartPs *uart1_ptr = (XUartPs *) CallBackRef;
	u8 recieve_buffer_ptr;

	if (Event == XUARTPS_EVENT_RECV_DATA) {

		XUartPs_Recv(uart1_ptr, &recieve_buffer_ptr, 1);
		if(recieve_buffer_ptr == (u8)'\r') {
			recieve_buffer_ptr = (u8)'\n';
			XUartPs_Send(uart1_ptr, &recieve_buffer_ptr, 1);
		} else if (recieve_buffer_ptr == (u8)'3'){
			done = true;
		}else{
			XUartPs_Send(uart1_ptr, &recieve_buffer_ptr, 1);
		}
	}
}

static void btn_callback(u32 btn) {
	led_toggle(btn);
	//pushes++;
}

int main(void) {

	XUartPs_Config *Uart1_config;

	// initialize platform
	init_platform();
	// initialize gic
	gic_init();
	// initialize buttons
	io_btn_init(btn_callback);
	// initialize switches
	io_sw_init(btn_callback);
	// initialize leds
	led_init();


	// find uart1 config
	Uart1_config = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
	// initialize uart1
	XUartPs_CfgInitialize(&uart_ps_1, Uart1_config, Uart1_config->BaseAddress);
	// enable RX FIFO trigger interrupt
	XUartPs_SetInterruptMask(&uart_ps_1, XUARTPS_IXR_RXOVR);
	// set FIFO threshold
	/*
	 * This functions sets the receive FIFO trigger level. The receive trigger
	 * level specifies the number of bytes in the receive FIFO that cause a receive
	 * data event (interrupt) to be generated.
	 */
	XUartPs_SetFifoThreshold(&uart_ps_1, 1);
	// set handler
	XUartPs_SetHandler(&uart_ps_1, (XUartPs_Handler)Uart1_Handler, &uart_ps_1);
	// connect to interrupt controller
	gic_connect(XPAR_XUARTPS_1_INTR, (Xil_ExceptionHandler)XUartPs_InterruptHandler, (void *) &uart_ps_1);





	/* do some initialization here */
	printf("[hello]\n");
	done = false;
	while(!done) /* do nothing waiting for interrupts */


		sleep(1); /* included from <unistd.h> */
	printf("[done]\n");
	gic_disconnect(XPAR_XUARTPS_1_INTR);
	gic_close();
	cleanup_platform();
	sleep(1);
	/* do some cleanup here */
}
