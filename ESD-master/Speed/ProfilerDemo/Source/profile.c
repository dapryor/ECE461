#include <stdint.h>
#include "timers.h"
#include "region.h"
#include "profile.h"

volatile unsigned int adx_lost=0, num_lost=0; 
volatile unsigned long profile_ticks=0;
unsigned char profiling_enabled = 0;

void Init_Profiling(void) {
	unsigned i;
	
	// Clear region counts
  for (i=0; i<NumProfileRegions; i++) {
	  RegionCount[i]=0;
  }
	
	// Initialize and start timer
	Init_PIT(2399);
	Start_PIT();
}

void Disable_Profiling(void) {
  profiling_enabled = 0;
}

void Enable_Profiling(void) {
  profiling_enabled = 1;
}
