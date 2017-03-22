#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>

#include "Drift_Calculation.h"
#include "trig_approx.h"
#include <arm_math.h>


//#define PI (3.14159265)
//const float deg2rad = ((PI)/(180));
//const float rad2deg = ((180)/(PI));
 
void Compute_Current(float speed_water, float angle_heading, q15_t speed_ground, float angle_track, 
	float * speed_current, float * angle_current){
	float angle_drift_rad;
	float angle_drift_deg;
	float  local_angle_current;
	float temp;
	q15_t sg_2;
		
	if (angle_heading < 0)
		angle_heading += 360;
	if (angle_heading >= 360)
		angle_heading -= 360;

	angle_drift_deg = (angle_track - angle_heading);//*PI/180;
	
	
	if (fabs(speed_ground) < MIN_SPEED_TOLERANCE) { 		// Check for special cases - no relative motion
		// not moving relative to ground, so current direction is opposite of heading through water
		local_angle_current = angle_heading-180;
		*speed_current = speed_water;
	}	else if (fabs(speed_water) < MIN_SPEED_TOLERANCE) { 
		// not moving relative to water, so current direction is track over ground
		*angle_current = angle_heading;
		*speed_current = speed_ground;
		return;
	}	else if (fabs(angle_drift_deg) < MIN_ANGLE_TOLERANCE) { 	// Check for special cases - no angular difference
		// track and heading are same: case IV
		*angle_current = angle_track;
		*speed_current = speed_ground - speed_water;
		return;
	} else if (fabs(angle_drift_deg - (float)180) < MIN_ANGLE_TOLERANCE) { 
		// track and heading are opposite: case III
		local_angle_current = angle_heading-180;
		*speed_current = speed_ground + speed_water;
	} else {
		angle_drift_rad = (angle_track - angle_heading)*PI/180;
		*speed_current = sqrt(speed_ground * speed_ground + speed_water*speed_water - 2*speed_water*speed_ground*cos_32(angle_drift_rad));
		
		
		
		temp = sin_32(angle_drift_rad)*speed_ground/(*speed_current);
		
		if (temp > 1){
			local_angle_current = 90 + angle_heading;
		}else if (temp < -1){
			local_angle_current = 270 + angle_heading;
		}else{
			local_angle_current = (180-asin(temp)*180/PI) + angle_heading;
		}
	}	
	if (local_angle_current < 0){
		*angle_current = local_angle_current + 360;
		return;
	}	else if (local_angle_current >= 360){
		*angle_current = local_angle_current - 360;
		return;
	} else{
		*angle_current = local_angle_current;
	}
	
}

