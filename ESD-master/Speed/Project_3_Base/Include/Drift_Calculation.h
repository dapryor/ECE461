#ifndef DRIFT_CALCULATION_H
#define DRIFT_CALCULATION_H

extern void Compute_Current(float speed_water, float angle_heading, float speed_ground, float angle_track, 
	float * speed_current, float * angle_current);
#define MIN_SPEED_TOLERANCE 0.01
#define MIN_ANGLE_TOLERANCE 0.1

#endif // DRIFT_CALCULATION_H
