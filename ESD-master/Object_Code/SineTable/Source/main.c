#include <MKL25Z4.h>
#include <math.h>

#define NUM_STEPS (512)
#define MAX_DAC_CODE (4095)
#define PI (3.14159265)

uint8_t SineTable[NUM_STEPS];
	
void Init_SineTable(void) {
	unsigned n;
	for (n=0; n<NUM_STEPS; n++) {
		SineTable[n] = (MAX_DAC_CODE/2)*(1+sin(n*2*PI/NUM_STEPS));
	}
}

void Init_SineTable2(void) {
	unsigned n;
	float angle;
	for (n=0; n<NUM_STEPS; n++) {
		angle = n*2*PI/NUM_STEPS;
		SineTable[n] = (MAX_DAC_CODE/2)*
		(1 + sin(angle));
	}
}

int main (void) {
	Init_SineTable();
	while (1)
		;
}
