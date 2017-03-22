/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "gpio_defs.h"
#include "UART.h"
#include "LEDs.h"
#include "timers.h"		
#include "delay.h"
#include "vector.h"
#include "profile.h"
#include "region.h"
#include "Drift_Calculation.h"

#define NUM_TESTS 100
#define MAX_MAG_ERROR 0.1
#define MAX_ANGLE_ERROR 1.0

typedef struct {
	VECTOR_T BtW; // Boat motion relative to water
	VECTOR_T BtG;	// Boat motion relative to ground
	VECTOR_T WtG; // Water motion relative to ground
} TEST_CASE;

TEST_CASE Tests[] = {
{{	1.000	,	0.000	},	{	1.414	,	45.000	},	{	1.000	,	90.000	}},
{{	1.000	,	0.000	},	{	1.414	,	315.000	},	{	1.000	,	270.000	}},
{{	1.000	,	0.000	},	{	0.000	,	0.000	},	{	1.000	,	180.000	}},
{{	1.000	,	0.000	},	{	2.000	,	0.000	},	{	1.000	,	0.000	}},
{{	1.000	,	90.000	},	{	1.414	,	45.000	},	{	1.000	,	0.000	}},
{{	1.000	,	-90.000	},	{	1.414	,	-45.000	},	{	1.000	,	0.000	}},
{{	1.000	,	180.000	},	{	1.414	,	225.000	},	{	1.000	,	270.000	}},
{{	1.000	,	45.000	},	{	1.414	,	0.000	},	{	1.000	,	315.000	}},
{{	1.000	,	135.000	},	{	1.414	,	180.000	},	{	1.000	,	225.000	}},
{{	4.000	,	0.000	},	{	5.000	,	-36.870	},	{	3.000	,	270.000	}},
{{	4.000	,	120.000	},	{	5.000	,	156.870	},	{	3.000	,	210.000	}},
{{	3.000	,	135.000	},	{	3.200	,	77.276	},	{	2.998	,	19.487	}},
{{	3.000	,	180.000	},	{	2.300	,	114.000	},	{	2.946	,	45.504	}}
};

int Get_Data(float * STW, float * HDG, float * TRK, float * SOG) {
	char id[4], buffer[100];
	char * p;
	int i,n;
	char debug = 0;
	float value=0.0;
	
	printf("\r\nEnter the input data with this format: \r\nSTW:1.2345,HDG:124.23,SOG:1.3525,TRK:155.33\r\n");
	// Order doesn't matter
	scanf("%100[^\r]", buffer);
	printf("\r\nReceived: %s\r\n", buffer);

	p = buffer;
	while (!isalpha(*p)) // advance to start of buffer 
			p++;

	for (i=0; i<4; i++) {
		sscanf(p, "%3s:%f,%n", id, &value, &n);
		if (debug)
			printf("\r\nGot it: %s and %f. Read %d chars.\r\n", id, value, n);
		p += n;
		if (debug)
			printf("Remainder is %s\r\n", p);
		if (!strcmp(id, "STW")) {
			*STW = value;
		} else if (!strcmp(id, "HDG")) {
			*HDG = value;
		} else if (!strcmp(id, "TRK")) {
			*TRK = value;
		} else if (!strcmp(id, "SOG")) {
			*SOG = value;
		} else {
			printf("\r\nUnknown identifier: %s\r\n", id);
			return 0;
		}		
	}
	return 1;
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	float stw = 0;
	float sog = 0;
	float hdg = 0;
	float trk = 0;
	float cspd = 0;
	float cang = 0;
	int i, t, print_approx_results=1;
	
	// Phase 1: initialization
	Init_Profiling();
	Init_RGB_LEDs();
	__disable_irq();
	Init_UART0(115200);
	__enable_irq();
	printf("\r\n\nProject 3 Base Code\r\n");
	Control_RGB_LEDs(1,1,0);

	// Phase 2: Known test cases for profiling and optimizing Compute_Current
	Control_RGB_LEDs(0,1,0);
	for (t=0; t<NUM_TESTS; t++) {
		for (i=0; i<13; i++) {
			stw = Tests[i].BtW.magnitude;
			hdg = Tests[i].BtW.angle;
			sog = Tests[i].BtG.magnitude;
			trk = Tests[i].BtG.angle;
	
			Enable_Profiling();
			TOGGLE_BLUE_LED
			Compute_Current(stw, hdg, sog, trk, &cspd, &cang);
			TOGGLE_BLUE_LED
			Disable_Profiling();
		
			if (print_approx_results) {
				printf("\r\nTest %d", i);
				printf("\r\nCorrect Speed: %f kts, Angle: %f deg", Tests[i].WtG.magnitude, Tests[i].WtG.angle);	
				printf("\r\nCalculated Speed: %f kts, Angle: %f deg", cspd, cang);
				if (fabs(cspd-Tests[i].WtG.magnitude) > MAX_MAG_ERROR)
					printf("\r\nXXX Magnitude error!!! XXX");
				if (fabs(cang-Tests[i].WtG.angle) > MAX_ANGLE_ERROR)
					printf("\r\n                XXX Angle error!!! XXX");
			}
		}
		print_approx_results = 0; // Don't print out approximation results for any tests after the first set
	}
	printf("\r\n");
	Sort_Profile_Regions();
	Print_Sorted_Profile();
	Control_RGB_LEDs(0,1,0);

	// Phase 3: Process test cases from serial port
	// This code allows testing with input data from the serial port (115.200 kbaud, 8N1)
	while (1) {
		if (Get_Data(&stw, &hdg, &trk, &sog)) {
			printf("STW:%f, HDG:%f, TRK:%f, SOG:%f\r\n", stw, hdg, trk, sog);
			cspd = 0;
			cang = 0;
			TOGGLE_BLUE_LED // Do not delete - used for grading
			Compute_Current(stw, hdg, sog, trk, &cspd, &cang);
			TOGGLE_BLUE_LED	// Do not delete - used for grading
			printf("Current speed: %f, Current direction: %f\r\n", cspd, cang);
		} else {
			printf("Input data format error.\r\n"); 
		}
	}
}
