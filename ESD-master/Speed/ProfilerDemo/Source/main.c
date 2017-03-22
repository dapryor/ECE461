/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "timers.h"
#include "profile.h"
#include "math.h"

// #define USE_LCD

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
		uint16_t p;
		volatile float f1 = 0.43, f2 = 3.11, f3 = 1.11;
	
		Init_RGB_LEDs();
	
		// Test the profiling
		Init_Profiling();
		__enable_irq();
		Control_RGB_LEDs(1,0,0);
		Enable_Profiling();

		for (p=0; p<10000; p++) {
			f1 *= 1.02;
			f2 += f1;
			f3 += sin(f1)*cos(f2);
		}
		Disable_Profiling();
		Control_RGB_LEDs(0,1,0);
		
		while (1)
			;
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
