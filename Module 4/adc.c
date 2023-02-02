#include "adc.h"



static XAdcPs_Config *adc_config;
static XAdcPs adc; // always initialize pointers as variables so pointers have a value

/*
 * initialize the adc module
 */
void adc_init(void){

	// find xadc config
	if((adc_config = XAdcPs_LookupConfig(XPAR_XADCPS_0_DEVICE_ID)) == NULL) {
		printf("Unable to find Xadc config\n");
		fflush(stdout);
		return;
	}

	// Initialize xadc
	if(XAdcPs_CfgInitialize(&adc, adc_config, adc_config->BaseAddress) != XST_SUCCESS) {
		printf("Unable to initialize adc\n");
		fflush(stdout);
		return;
	}

	// self test
	if(XAdcPs_SelfTest(&adc) != XST_SUCCESS) {
		printf("self test failed\n");
		fflush(stdout);
		return;
	}

	// set sequencing mode to safe (disable sequencing as its being config)
	XAdcPs_SetSequencerMode(&adc, XADCPS_SEQ_MODE_SAFE);

	// disable alarms
	XAdcPs_SetAlarmEnables(&adc, 0x0);

	/* enable channels (Use XADCPS_SEQ_CH__* defined in xadcps_hw.h to specify the Channel
	 * numbers.):
	 * XADCPS_SEQ_CH_TEMP chip tempreturn channel
	 * VCCINT Channel channel
	 * XADCPS_SEQ_CH_VCCINT = XADCPS_SEQ_CH_AUX14 = 15th Aux Channel
	 */
	XAdcPs_SetSeqChEnables(&adc, XADCPS_SEQ_CH_TEMP | XADCPS_SEQ_CH_VCCINT | XADCPS_SEQ_CH_AUX14);

	// enable sequencer
	XAdcPs_SetSequencerMode(&adc, XADCPS_SEQ_MODE_CONTINPASS);



}

/*
 * get the internal temperature in degree's centigrade
 */
float adc_get_temp(void){
	u32 chip_temp_raw;
	float chip_temp;

	// read from chip temp channel
	chip_temp_raw = XAdcPs_GetAdcData(&adc, XADCPS_CH_TEMP);

	// convert raw data to readable data
	chip_temp = XAdcPs_RawToTemperature(chip_temp_raw);

	return chip_temp;

}

/*
 * get the internal vcc voltage (should be ~1.0v)
 */
float adc_get_vccint(void){
	u32 vcc_raw;
	float vcc;

	// read from vcc channel
	vcc_raw = XAdcPs_GetAdcData(&adc, XADCPS_CH_VCCINT);

	// convert raw data to readable form
	vcc = XAdcPs_RawToVoltage(vcc_raw);

	return vcc;


}

/*
 * get the **corrected** potentiometer voltage (should be between 0 and 1v)
 */
float adc_get_pot(void){
	u32 aux14_raw;
	float aux14;

	// read from aux 14 (potentiometer)
	aux14_raw = XAdcPs_GetAdcData(&adc, XADCPS_CH_AUX_MAX-1); // or XADCPS_CH_AUX_MAX-1);

	// convert raw data to readable form
	aux14 =  XAdcPs_RawToVoltage(aux14_raw);

	return aux14;
}
