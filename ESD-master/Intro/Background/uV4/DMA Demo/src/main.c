#include <MKL25Z4.H>
#include <math.h>
#include "user_defs.h"
#include "LEDs.h"
#include "queue.h"
#include "timers.h"
#include "switches.h"
#include "DMA.h"

uint16_t SineTable[NUM_STEPS];
Q_T queue;

void Init_DAC(void) {
  // Init DAC output
	
	SIM->SCGC6 |= (1UL << SIM_SCGC6_DAC0_SHIFT); 
	SIM->SCGC5 |= (1UL << SIM_SCGC5_PORTE_SHIFT); 
	
	PORTE->PCR[DAC_POS] &= ~(PORT_PCR_MUX(7));	// Select analog 
		
	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;
	
	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = (1 << DAC_C0_DACEN_SHIFT) | 
							(1 << DAC_C0_DACRFS_SHIFT);

}

void Delay_us(volatile unsigned int time_del) {
	// This is a very imprecise and fragile implementation!
	time_del = 9*time_del + time_del/2; 
	while (time_del--) {
		;
	}
}

/*------------------------------------------------------------------------------
	Code for driving DAC
	period (of output waveform): microseconds
	duration: cycles (of output waveform)
*------------------------------------------------------------------------------*/
void Play_Tone_with_Busy_Waiting(unsigned int period, unsigned int num_cycles, unsigned wave_type) {
	unsigned step, out_data;
	
	while (num_cycles>0) {
		num_cycles--;
		for (step = 0; step < NUM_STEPS; step++) {
			switch (wave_type) {
				case SQUARE: 
					if (step < NUM_STEPS/2)
						out_data = 0;	
					else
						out_data = MAX_DAC_CODE;
					break;
				case RAMP:
					out_data = (step*MAX_DAC_CODE)/NUM_STEPS;
					break;
				case SINE:
					out_data = SineTable[step];
					break;
			default:
					break;
			}
			
			// Simulate interference from other code - wait until switch is released
			while (!(PTD->PDIR & MASK(SW_POS)))
				;

			DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
			DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);
			Delay_us(period/NUM_STEPS);
		}
	}
}

void Play_Tone_with_Interrupt(unsigned int period, unsigned int num_cycles, unsigned wave_type) {
	unsigned step;
	unsigned short out_data;

	Init_PIT(period/NUM_STEPS, 1);
	Start_PIT();
	
	while (num_cycles>0) {
		num_cycles--;
		for (step = 0; step < NUM_STEPS; step++) {
			switch (wave_type) {
				case SQUARE: 
					if (step < NUM_STEPS/2)
						out_data = 0;	
					else
						out_data = MAX_DAC_CODE;
					break;
				case RAMP:
					out_data = (step*MAX_DAC_CODE)/NUM_STEPS;
					break;
				case SINE:
					out_data = SineTable[step];
					break;
				default:
					break;
			}

			
			// Simulate interference from other code - wait until switch is released
			while (!(PTD->PDIR & MASK(SW_POS)))
				;

			while (Q_Full(&queue)) {
				// if queue is full, then wait for it to empty
				;
			}
			Q_Enqueue(&queue, out_data);
		}
	}
	Stop_PIT();
}

void Play_Tone_with_DMA(unsigned int period, unsigned int num_cycles) {
	Init_TPM(period/NUM_STEPS);
	Init_DMA_For_Playback(SineTable, NUM_STEPS, num_cycles);
	Start_DMA_Playback();
}


void Init_SineTable(void) {
	unsigned n;
	
	for (n=0; n<NUM_STEPS; n++) {
		SineTable[n] = (MAX_DAC_CODE/2)*(1+sin(n*2*3.1415927/NUM_STEPS));
	}
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	Init_RGB_LEDs();
	Init_Switch();
	Init_DAC();
	Init_SineTable();

//	Control_RGB_LEDs(0,0,1);
//	Test_DMA_Copy();
//	Control_RGB_LEDs(1,0,1);
	
	Q_Init(&queue);
	__enable_irq();
	Control_RGB_LEDs(1,0,0);
	
	// set to 1 to use DMA to play back tone
#if 1
	Play_Tone_with_DMA(40000, 100000);
	Control_RGB_LEDs(0,0,0);
	while (1)
		;
#else // using other methods
	while (1) {
//		Play_Tone_with_Busy_Waiting(40000, 1000, SINE);
		Play_Tone_with_Interrupt(40000, 1000, SINE);
	}
#endif
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
