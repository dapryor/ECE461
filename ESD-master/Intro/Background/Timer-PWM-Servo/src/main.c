/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "timers.h"
#include "i2c.h"
#include "mma8451.h"
#include "delay.h"

#define SWEEP_DELAY (2)
 


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint16_t d=0;
	int32_t p=0;

#if 0 // Show SysTick with RGB LEDs
	Init_RGB_LEDs();
	Init_SysTick();
	while (1)
		;
#endif
	
	
#if 0 // Show PIT with RGB LEDs
	Init_RGB_LEDs();
	Init_PIT(2399999);
	Start_PIT();	
	while (1)
		;
#endif

	Init_RGB_LEDs();
	Init_PWM();
	Control_RGB_LEDs(1,1,0);

	Set_Servo_Position(2, POS_MAX_CCW);
	Set_Servo_Position(4, POS_MAX_CCW);
	
	Delay(100);
	Control_RGB_LEDs(0,1,0);
		
	// Square
	while (1) {
		Control_RGB_LEDs(0,0,1);
		for (p = POS_MAX_CCW; p < POS_MAX_CW; p++) {
			Set_Servo_Position(2, p);
			Delay(SWEEP_DELAY);
		}
		Control_RGB_LEDs(1,0,1);
		for (p = POS_MAX_CCW; p < POS_MAX_CW; p++) {
			Set_Servo_Position(4, p);
			Delay(SWEEP_DELAY);
		}
		Control_RGB_LEDs(1,1,0);
		for (p = POS_MAX_CW; p > POS_MAX_CCW; p--) {
			Set_Servo_Position(2, p);
			Delay(SWEEP_DELAY);
		}
		Control_RGB_LEDs(0,1,1);
		for (p = POS_MAX_CW; p > POS_MAX_CCW; p--) {
			Set_Servo_Position(4, p);
			Delay(SWEEP_DELAY);
		}
	}
	
	// Diagonal
	while (1) {
		for (p = POS_MAX_CCW; p < POS_MAX_CW; p++) {
			Set_Servo_Position(2, p);
			Set_Servo_Position(4, p);
		}
		for (p = POS_MAX_CW; p > POS_MAX_CCW; p--) {
			Set_Servo_Position(2, p);
			Set_Servo_Position(4, p);
		}
	}

	
	while (1) {
		for (d = T_MAX_CCW; d < T_MAX_CW; d++) {
			Set_PWM_Value(2, d);
			Set_PWM_Value(4, d);
			Delay(1);
		}
		for (d = T_MAX_CW; d > T_MAX_CCW; d--) {
			Set_PWM_Value(2, d);
			Set_PWM_Value(4, d);
			Delay(1);
		}
	}

#if 0
	i2c_init();																/* init i2c	*/
	res = init_mma();													/* init mma peripheral */
	
	Delay(1000);
	// __enable_irq();
	
	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		
		Delay(100);
		
		Set_PWM_Values(r++, p++);
		
		if (r>=100)
			r = 0;
		if (p>=100)
			p = 0;
	}
#endif
	
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
