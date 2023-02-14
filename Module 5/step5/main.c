#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

#include "platform.h"		/* ZYBO board interface */
#include "xil_types.h"		/* u32, s32 etc */
#include "xparameters.h"	/* constants used by hardware */
#include "xuartps.h"

#include "gic.h"		/* interrupt controller interface */
#include "xgpio.h"		/* axi gpio interface */
#include "led.h" 		// led interface
#include "io.h" //




static XUartPs uart_ps_1;
static XUartPs uart_ps_0;
static bool done; // end of program


// forward anything recieved to uart 1
static void Uart0_Handler( void *CallBackRef, u32 Event, unsigned int EventData) {

	XUartPs *uart_ptr = (XUartPs *) CallBackRef;
	u8 recieve_buffer;

	if (Event == XUARTPS_EVENT_RECV_DATA) {
		XUartPs_Recv(uart_ptr, &recieve_buffer, 1); // recieve normally
		XUartPs_Send(&uart_ps_1, &recieve_buffer, 1); // send to uart1
	}

}


static void Uart1_Handler( void *CallBackRef, u32 Event, unsigned int EventData) {

	XUartPs *uart_ptr = (XUartPs *) CallBackRef;
	u8 recieve_buffer_ptr;

	if (Event == XUARTPS_EVENT_RECV_DATA) {

		XUartPs_Recv(uart_ptr, &recieve_buffer_ptr, 1);
		if(recieve_buffer_ptr == (u8)'\r') {
			// we still need to send it as it but also on screen print a new line
			//XUartPs_Send(uart_ptr, &recieve_buffer_ptr, 1); // send to screen
			XUartPs_Send(&uart_ps_0, &recieve_buffer_ptr, 1); //send uart 0

			recieve_buffer_ptr = (u8)'\n';
			XUartPs_Send(uart_ptr, &recieve_buffer_ptr, 1);
		} else{
			//XUartPs_Send(uart_ptr, &recieve_buffer_ptr, 1); // send to screen
			XUartPs_Send(&uart_ps_0, &recieve_buffer_ptr, 1); //send uart 0
		}
	}
}

static void btn_callback(u32 btn) {
	if (btn == 2) {
		done = true;
	}
	led_toggle(btn);
	//pushes++;
}

int main(void) {

	XUartPs_Config *Uart1_config;
	XUartPs_Config *Uart0_config;

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

	//########################Uart 1 setup
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

	//############################uart0 setup
	// find uart0 config
	Uart0_config = XUartPs_LookupConfig(XPAR_PS7_UART_0_DEVICE_ID);
	// initialize uart0
	XUartPs_CfgInitialize(&uart_ps_0, Uart0_config, Uart0_config->BaseAddress);
	// enable RX FIFO trigger interrupt
	XUartPs_SetInterruptMask(&uart_ps_0, XUARTPS_IXR_RXOVR);
	// set threshold
	XUartPs_SetFifoThreshold(&uart_ps_0, 1);
	// set handler
	XUartPs_SetHandler(&uart_ps_0, (XUartPs_Handler)Uart0_Handler, &uart_ps_0);
	//set baud rate
	XUartPs_SetBaudRate(&uart_ps_0, 9600);
	// connect to interrupt controller
	gic_connect(XPAR_XUARTPS_0_INTR, (Xil_ExceptionHandler)XUartPs_InterruptHandler, (void *) &uart_ps_0);





	/* do some initialization here */
	printf("[hello]\n");
	done = false;
	while(!done); /* do nothing waiting for interrupts */

	printf("[done]\n");
	sleep(1); /* included from <unistd.h> */

	/* do some cleanup here */
	XUartPs_DisableUart(&uart_ps_0);
	gic_disconnect(XPAR_XUARTPS_0_INTR);

	XUartPs_DisableUart(&uart_ps_1);
	gic_disconnect(XPAR_XUARTPS_1_INTR);
	gic_close();
	cleanup_platform();

}
