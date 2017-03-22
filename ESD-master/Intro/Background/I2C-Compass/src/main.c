/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "hmc5883.h"
#include "delay.h"
#include "UART.h"
#include "queue.h"

#define USE_ACCEL (0)

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/

int main (void) {
	int16_t mag_field[3];
	float heading=0.0;
	uint8_t color;
	
	Init_RGB_LEDs();
	Init_UART0(115200);
	i2c_init();							

	printf("\r\nHello, World!\r\n");

	Delay(300);
#if USE_ACCEL// accelerometer
	/* init i2c	*/
	if (!init_mma()) {												/* init mma peripheral */
		Control_RGB_LEDs(1, 0, 0);							/* Light red error LED */
		while (1)																/* not able to initialize mma */
			;
	}
	Delay(1000);
	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		// Light green LED if pitch > 10 degrees
		// Light blue LED if roll > 10 degrees
		Control_RGB_LEDs(0, (fabs(roll) > 10)? 1:0, (fabs(pitch) > 10)? 1:0);
	}
#else
	init_compass();
	while (1) {
		Delay(10);
		read_compass_xzy(mag_field);
		heading = calc_heading(mag_field);
		printf("Heading = %6.2f%c M\n\r", heading, DEGREE_CODE);
//		printf("Heading = %d\n\r", (int) heading);
		color = heading/45;
		Control_RGB_LEDs(color & 4, color & 2, color & 1);
	}
#endif
}

