/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "UART.h"
#include "LEDs.h"
#include "timers.h"		
#include "delay.h"

#define TRANSMIT_MODE 0
#define RECEIVE_MODE 1

extern void	UART0_Echo_Interrupt_Test(void);

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint8_t c='a', buffer[64];
	uint32_t n=0;
	
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,1,0);
	
#if 0 // UART 2 
	Init_UART2(4800);
	
#if USE_UART_INTERRUPTS
	__enable_irq();
	while (1) {
		Send_String("\r\nHello World!");
		Delay(200);
	}
#endif	

	#if TRANSMIT_MODE
	while (1) {
		for (c='a'; c<='z'; c++) {
			UART2_Transmit_Poll(c);
			Delay(10);
		}
	}
	#endif
	
	#if RECEIVE_MODE
	UART2_Transmit_Poll('?');
	UART2_Transmit_Poll(' ');
	while (1) {
		c = UART2_Receive_Poll();
		UART2_Transmit_Poll(c+1);
	}
	#endif
#endif
	
#if 1
	Delay(50);
	Init_UART0(115200);
	Control_RGB_LEDs(1,1,0);

	Send_String("\r\nHello World!\r\n");
	
	UART0_Echo_Interrupt_Test();
	
//	__disable_irq();
	
#if 0
	while (1) {
		for (c='a'; c <= 'z'; c++)
			UART0_Transmit_Poll(c);
	}
#endif
	
	// UART0 Echo
#if 0
	UART0_Transmit_Poll('?');
	UART0_Transmit_Poll(' ');
	while (1) {
		c = UART0_Receive_Poll();
		UART0_Transmit_Poll(c+1);
	}
#endif

	
	while(1){	
		Control_RGB_LEDs(1,0,0);
#if QUEUED_U0
		sprintf(buffer, "1_2_3_4_5_6_7_8_9_A_B_C_D_E_F_G %5d\n\r", n++);  
		Send_String(buffer);
#else
		printf("Hello World %d\n\r", n++);  
#endif
		/* Printf may crash if inadequate stack space allocated. 
		See startup_MKL25Z4.s to change stack space. */
		
		Control_RGB_LEDs(0,1,0);
		Delay(100);
	}

#endif	
	
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
