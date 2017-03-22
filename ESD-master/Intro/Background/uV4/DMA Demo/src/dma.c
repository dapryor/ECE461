#include <stdint.h>
#include <MKL25Z4.h>
#include "timers.h"
#include "LEDs.h"

#define ARR_SIZE (256)
uint32_t s[ARR_SIZE], d[ARR_SIZE];

uint16_t * Reload_DMA_Source=0;
uint32_t Reload_DMA_Byte_Count=0;
uint32_t DMA_Playback_Count=0;


void Init_DMA_To_Copy(void) {
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	DMA0->DMA[0].DCR = DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(0) |	DMA_DCR_DINC_MASK |	DMA_DCR_DSIZE(0);
		
}

void Copy_Longwords(uint32_t * source, uint32_t * dest, uint32_t count) {
	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) source);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) dest);
	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(count*4);
	// verify done flag is cleared
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; 
	
	// start transfer
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;
	// wait until it is done
	while (!(DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK))
		;
}

void Test_DMA_Copy(void) {
	uint16_t i;
	
	Init_DMA_To_Copy();
	
	for (i=0; i<ARR_SIZE; i++) {
		s[i] = i;
		d[i] = 0;
	}
	
	Copy_Longwords(s, d, ARR_SIZE);
		
}


void Init_DMA_For_Playback(uint16_t * source, uint32_t count, uint32_t num_playbacks) {
	
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	// Disable DMA channel
	DMAMUX0->CHCFG[0] = 0;

	Reload_DMA_Source = source;
	Reload_DMA_Byte_Count = count*2;
	DMA_Playback_Count = num_playbacks;
	
	// Generate DMA interrupt when done
	// Increment source, transfer words (16 bits)
	// Enable peripheral request
	DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK | 
											DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) |
											DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;

	
	// Configure NVIC for DMA ISR
	NVIC_SetPriority(DMA0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(DMA0_IRQn); 
	NVIC_EnableIRQ(DMA0_IRQn);	

	// Enable DMA MUX channel without periodic triggering
	// select TPM0 overflow as trigger
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(54);   
}

void Start_DMA_Playback() {
	if (DMA_Playback_Count == 0)
		return;
	
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(54);   

	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));
	
	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(Reload_DMA_Byte_Count);
	
	// verify done flag is cleared
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; 
	
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;

	// start the timer running
	Start_TPM();
}

void DMA0_IRQHandler(void) {
	// Light blue LED in DMA IRQ handler
	Control_RGB_LEDs(0,0,1);

	NVIC_ClearPendingIRQ(DMA0_IRQn);

	// Clear done flag 
	DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK; 
	
	if (--DMA_Playback_Count) {
		// Start the next DMA playback cycle
		Start_DMA_Playback();
	} else {
		// Disable DMA
		DMAMUX0->CHCFG[0] &= ~DMAMUX_CHCFG_ENBL_MASK;
	}
	Control_RGB_LEDs(0,0,0);
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
