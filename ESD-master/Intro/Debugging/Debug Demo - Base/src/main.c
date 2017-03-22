/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "LCD_4bit.h"

#include "delay.h"

void Init_ADC(void) {
	
	SIM->SCGC6 |= (1UL << SIM_SCGC6_ADC0_SHIFT); 
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(0) | 
	ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3);
	ADC0->SC2 = ADC_SC2_REFSEL(0); // VREFHL selection, software trigger
	
	// PMC->REGSC |= PMC_REGSC_BGBE_MASK; // bug 2
}

float Measure_VRail(void) {
	volatile float vrail;
	unsigned res=0;
	
	ADC0->SC1[0] = ADC_SC1_ADCH(27); // start conversion on channel 27 (Bandgap reference)
	
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		;
	res = ADC0->R[0];
	vrail = (VBG_VALUE/res)*65536;
	return vrail;
}

/*----------------------------------------------------------------------------
  MAIN function

Be sure to configure USE_LCD in lcd_4bit.h correctly or else code may hang
1: Use external Character LCD module (HD44780-based)
0: Don't use LCD
 *----------------------------------------------------------------------------*/
int main (void) {
	float voltage = 0.0;
	char buff[17];
	
	Init_RGB_LEDs();
	Control_RGB_LEDs(0, 0, 0);

	// Init_ADC(); // bug 1 - Need to enable clock to ADC module and initialize it
	
	Init_LCD();
	Clear_LCD();

	Set_Cursor(0,0);
	Print_LCD(" Debug ");
	Set_Cursor(0,1);
	Print_LCD(" Demo  ");
	
	Set_Cursor(0,0);
	Print_LCD("3V3 Rail");
	while (1) {
		voltage = Measure_VRail();
		sprintf (buff, "%6.4d V", voltage); // bug 3, format string should be "%6.4f V"
		Set_Cursor(0,1);
		Print_LCD(buff);
	
		Delay(5000);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
