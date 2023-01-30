/* servo.c -- module implementing servo.h
*  
*/

#include "servo.h"

// hardware 
#define CLOCK_FREQ 50000000

// timer 0 settings
#define PERIOD 1000000 // 20ms period (Timer 0)
#define PERIOD_MS 20 // 20ms period (Timer 0)

XTmrCtr* psTmr;

/*
 * Initialize the servo, setting the duty cycle to 7.5%
 */
void servo_init(void) {
    // initialize the timer
    // psTmr = XTmrCtr_LookupConfig(XPAR_TMRCTR_0_DEVICE_ID);
    if (XTmrCtr_Initialize(psTmr, XPAR_AXI_TIMER_0_DEVICE_ID) != XST_SUCCESS){
		printf("Unable to initilize PS timer");
		fflush(stdout);
	}

    // stop both timers
    XTmrCtr_Stop(psTmr, 0);
	XTmrCtr_Stop(psTmr, 1);

    // set options for both timers
    XTmrCtr_SetOptions(psTmr, 0, XTC_PWM_ENABLE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_EXT_COMPARE_OPTION);
	XTmrCtr_SetOptions(psTmr, 1, XTC_PWM_ENABLE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_EXT_COMPARE_OPTION);

    // set reset value for both timers
    XTmrCtr_SetResetValue(psTmr, 0, PERIOD);
    XTmrCtr_SetResetValue(psTmr, 1, HighTime);
    servo_set(7.5);

    // start both timers
    XTmrCtr_Start(psTmr, 0);
	XTmrCtr_Start(psTmr, 1);
}

/*
 * Set the dutycycle of the servo
 */
void servo_set(double dutycycle) {
    // update the high time based on the duty cycle
    time_ms = (dutycycle/100) * PERIOD_MS;
    u32 HighTime = CLOCK_FREQ * (time_ms/1000);
    XTmrCtr_SetResetValue(psTmr, 1, HighTime);
}
