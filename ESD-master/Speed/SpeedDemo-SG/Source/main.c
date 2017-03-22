/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "timers.h"
#include "delay.h"
#include "profile.h"
#include "math.h"
#include "geometry.h"
#include "proto.h"

#define TEST1_LAT (41.31723)
#define TEST1_LON (38.19134)


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	float dist, bearing, cur_pos_lat, cur_pos_lon;
	char * name;

	cur_pos_lat = TEST1_LAT;
	cur_pos_lon = TEST1_LON;
	
	Init_RGB_LEDs();

	Init_Profiling();

	Control_RGB_LEDs(1,0,1); // Purple: running

	Enable_Profiling();
	Find_Nearest_Waypoint(cur_pos_lat, cur_pos_lon, &dist, &bearing, &name);
	Disable_Profiling();
	Sort_Profile();
	
	Control_RGB_LEDs(0,0,1);	// Blue: done
	while (1)
		;
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
