#include "timers.h"
#include "MKL25Z4.h"
#include "LEDs.h"

void Init_PWM()
{
	//turn on clock to TPM, Port E
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Testing Servos
	// Servo 1 is PTE29. 
	// Set mux to connect TPM0 Ch 2 to PTE29
	PORTE->PCR[29] &= ~PORT_PCR_MUX(7);
	PORTE->PCR[29] |= PORT_PCR_MUX(3);
	// PTE->PDDR

	// Servo 2 is PTE31. 
	// Set mux to connect TPM0 Ch 4 to PTE31
	PORTE->PCR[31] &= ~PORT_PCR_MUX(7);
	PORTE->PCR[31] |= PORT_PCR_MUX(3);
	
	//set clock source for tpm
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

	//load the counter and mod
	TPM0->MOD = PWM_PERIOD-1;
		
	//set channels to edge-aligned high-true PWM
	TPM0->CONTROLS[2].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	TPM0->CONTROLS[4].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	
	//set trigger mode
	TPM0->CONF |= TPM_CONF_TRGSEL(0xA);
	
	TPM0->CONTROLS[2].CnV = T_NEUTRAL;
	TPM0->CONTROLS[4].CnV = T_NEUTRAL;
	
	//set TPM to divide by 32 prescaler and enable counting (CMOD)
	TPM0->SC = (TPM_SC_CMOD(1) | TPM_SC_PS(5));}

void Set_PWM_Value(uint16_t channel, uint16_t duty) {
	if (duty > PWM_PERIOD)
		duty = PWM_PERIOD;
	
	TPM0->CONTROLS[channel].CnV = duty;
}

void Set_Servo_Position(uint16_t channel, int32_t pos) {
	uint16_t t=0;
	int temp;
	
	if (pos < POS_MAX_CCW)
			pos = POS_MAX_CCW;
	else if (pos > POS_MAX_CW)
			pos = POS_MAX_CW;
	
	temp	= PWM_WIDTH_RANGE*(pos-POS_MAX_CCW)/POS_RES + T_MAX_CCW;
	t = temp;
	Set_PWM_Value(channel, t);
}

void Init_PIT(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK;
	
#if 1
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
#endif
}


void Start_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


void PIT_IRQHandler() {
	static int n=0;
	//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	
	n++;
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		// Do ISR work here
		Control_RGB_LEDs(n&1, n&2, n&4);
		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
		// Do ISR work here
		
	} 
}

void Init_SysTick(void) {
	SysTick->LOAD  = (48000000L/16);              // Set reload to get 1 s interrupts
	NVIC_SetPriority (SysTick_IRQn, 3);           // Set interrupt priority 
	SysTick->VAL   = 0;                           // Force load of the SysTick reload value 
	SysTick->CTRL  = SysTick_CTRL_TICKINT_Msk |   // Enable interrupt, alternate clock source
                   SysTick_CTRL_ENABLE_Msk;     // Enable SysTick timer
}
void SysTick_Handler() {
	static int n=0;
	Control_RGB_LEDs(n&1,n&1,n&1);
	n++;
}


// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
