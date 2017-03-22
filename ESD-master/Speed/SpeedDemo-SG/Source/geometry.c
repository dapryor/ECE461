#include "geometry.h"
#include <math.h>
#include <string.h>
#define PI 3.14159265
#define PI_OVER_180 (0.017453293) // (3.1415927/180.0)

extern const PT_T waypoints[];

#define OPT_VERSION 6

#if OPT_VERSION == 0	// Double precision mathlib functions
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
  return acos(sin(p1->Lat*PI/180)*sin(p2->Lat*PI/180) + 
					cos(p1->Lat*PI/180)*cos(p2->Lat*PI/180)*
							cos(p2->Lon*PI/180 - p1->Lon*PI/180)) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float angle = atan2(
		sin(p1->Lon*PI/180 - p2->Lon*PI/180)*cosf(p2->Lat*PI/180),
		cos(p1->Lat*PI/180)*sin(p2->Lat*PI/180) - 
		sin(p1->Lat*PI/180)*cos(p2->Lat*PI/180)*cos(p1->Lon*PI/180 - p2->Lon*PI/180)
		) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}

#endif

#if OPT_VERSION == 1 // Single precision mathlib functions
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
  return acosf(sinf(p1->Lat*PI/180)*sinf(p2->Lat*PI/180) + 
					cosf(p1->Lat*PI/180)*cosf(p2->Lat*PI/180)*
							cosf(p2->Lon*PI/180 - p1->Lon*PI/180)) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float angle = atan2f(
		sinf(p1->Lon*PI/180 - p2->Lon*PI/180)*cosf(p2->Lat*PI/180),
		cosf(p1->Lat*PI/180)*sinf(p2->Lat*PI/180) - 
		sinf(p1->Lat*PI/180)*cosf(p2->Lat*PI/180)*cosf(p1->Lon*PI/180 - p2->Lon*PI/180)
		) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}

#endif
//////////////////////////////////////////////////
#if OPT_VERSION == 2 // Compile-time evaluation of expressions - Parenthesize PI/180 

float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
  return acosf(sinf(p1->Lat*(PI/180))*sinf(p2->Lat*(PI/180)) + 
					cosf(p1->Lat*(PI/180))*cosf(p2->Lat*(PI/180))*
							cosf(p2->Lon*(PI/180) - p1->Lon*(PI/180))) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float angle = atan2f(
		sinf(p1->Lon*(PI/180) - p2->Lon*(PI/180))*cosf(p2->Lat*(PI/180)),
		cosf(p1->Lat*(PI/180))*sinf(p2->Lat*(PI/180)) - 
		sinf(p1->Lat*(PI/180))*cosf(p2->Lat*(PI/180))*cosf(p1->Lon*(PI/180) - p2->Lon*(PI/180))
		) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}
#endif

//////////////////////////////////////////////////
#if OPT_VERSION == 3 // force compile-time eval of PI/180
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
  return acosf(sinf(p1->Lat*PI_OVER_180)*sinf(p2->Lat*PI_OVER_180) + 
					cosf(p1->Lat*PI_OVER_180)*cosf(p2->Lat*PI_OVER_180)*
							cosf(p2->Lon*PI_OVER_180 - p1->Lon*PI_OVER_180)) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float angle = atan2f(
		sinf(p1->Lon*(PI_OVER_180) - p2->Lon*(PI_OVER_180))*cosf(p2->Lat*(PI_OVER_180)),
		cosf(p1->Lat*(PI_OVER_180))*sinf(p2->Lat*(PI_OVER_180)) - 
		sinf(p1->Lat*(PI_OVER_180))*cosf(p2->Lat*(PI_OVER_180))*cosf(p1->Lon*(PI_OVER_180) - p2->Lon*(PI_OVER_180))
		) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}
#endif

//////////////////////////////////////////////////
#if OPT_VERSION == 4 // Copy lat and lon to local variables to reduce scope, enable some CSE
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
	float p1Lon, p1Lat, p2Lon, p2Lat;

	p1Lon = p1->Lon;
	p2Lon = p2->Lon;
	p1Lat = p1->Lat;
	p2Lat = p2->Lat;

  return acosf(sinf(p1Lat*PI_OVER_180)*sinf(p2Lat*PI_OVER_180) + 
					cosf(p1Lat*PI_OVER_180)*cosf(p2Lat*PI_OVER_180)*
							cosf(p2Lon*PI_OVER_180 - p1Lon*PI_OVER_180)) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float p1Lon, p1Lat, p2Lon, p2Lat;
	float angle;

	p1Lon = p1->Lon;
	p2Lon = p2->Lon;
	p1Lat = p1->Lat;
	p2Lat = p2->Lat;

	angle = atan2f(
		sinf(p1Lon*(PI/180) - p2Lon*(PI/180))*cosf(p2Lat*(PI/180)),
		cosf(p1Lat*(PI/180))*sinf(p2Lat*(PI/180)) - 
		sinf(p1Lat*(PI/180))*cosf(p2Lat*(PI/180))*cosf(p1Lon*(PI/180) - p2Lon*(PI/180))
		) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}
#endif

//////////////////////////////////////////////////

#if OPT_VERSION == 5 // Force some CSE by converting to radians early
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;

	p1LonRad = p1->Lon*PI_OVER_180;
	p2LonRad = p2->Lon*PI_OVER_180;
	p1LatRad = p1->Lat*PI_OVER_180;
	p2LatRad = p2->Lat*PI_OVER_180;

  return acosf(sinf(p1LatRad)*sinf(p2LatRad) + 
					cosf(p1LatRad)*cosf(p2LatRad)*
							cosf(p2LonRad - p1LonRad)) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;
	float angle;

	p1LonRad = p1->Lon*(PI/180);
	p2LonRad = p2->Lon*(PI/180);
	p1LatRad = p1->Lat*(PI/180);
	p2LatRad = p2->Lat*(PI/180);

	angle = atan2f(
		sinf(p1LonRad - p2LonRad)*cosf(p2LatRad),
		cosf(p1LatRad)*sinf(p2LatRad) - 
		sinf(p1LatRad)*cosf(p2LatRad)*cosf(p1LonRad - p2LonRad)
		) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}
#endif

//////////////////////////////////////////////////

#if OPT_VERSION == 6 // CB: manual CSE with intermediate terms to see if compiler does better
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;

	p1LonRad = p1->Lon*PI_OVER_180;
	p2LonRad = p2->Lon*PI_OVER_180;
	p1LatRad = p1->Lat*PI_OVER_180;
	p2LatRad = p2->Lat*PI_OVER_180;

  return acosf(sinf(p1LatRad)*sinf(p2LatRad) + 
					cosf(p1LatRad)*cosf(p2LatRad)*
							cosf(p2LonRad - p1LonRad)) * 6371;
}

float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;
	float angle;

	float term1, term2;
	
	p1LonRad = p1->Lon*(PI/180);
	p2LonRad = p2->Lon*(PI/180);
	p1LatRad = p1->Lat*(PI/180);
	p2LatRad = p2->Lat*(PI/180);

	term1 = sinf(p1LonRad - p2LonRad)*cosf(p2LatRad);
	term2 = cosf(p1LatRad)*sinf(p2LatRad) - 
					sinf(p1LatRad)*cosf(p2LatRad)*cosf(p1LonRad - p2LonRad);
	angle = atan2f(term1, term2) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}
#endif

//////////////////////////////////////////////////

#if OPT_VERSION == 7 // CB: manual CSE of cos(p2->Lat in radians)
float Calc_Distance( PT_T * p1,  const PT_T * p2) { 
// calculates distance in kilometers between locations (represented in degrees)
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;

	p1LonRad = p1->Lon*PI_OVER_180;
	p2LonRad = p2->Lon*PI_OVER_180;
	p1LatRad = p1->Lat*PI_OVER_180;
	p2LatRad = p2->Lat*PI_OVER_180;

  return acosf(sinf(p1LatRad)*sinf(p2LatRad) + 
					cosf(p1LatRad)*cosf(p2LatRad)*
							cosf(p2LonRad - p1LonRad)) * 6371;
}
float Calc_Bearing( PT_T * p1,  const PT_T * p2){
// calculates bearing in degrees between locations (represented in degrees)	
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;
	float cosp2LatRad;
	float term1, term2;
	
	float angle;
	
	p1LonRad = p1->Lon*(PI/180);
	p2LonRad = p2->Lon*(PI/180);
	p1LatRad = p1->Lat*(PI/180);
	p2LatRad = p2->Lat*(PI/180);
	cosp2LatRad = cosf(p2LatRad);
	
	term1 = sinf(p1LonRad - p2LonRad)*cosp2LatRad;
	term2 = cosf(p1LatRad)*sinf(p2LatRad) - 
					sinf(p1LatRad)*cosp2LatRad*cosf(p1LonRad - p2LonRad);
	angle = atan2f(term1, term2) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	return angle;
}
#endif

//////////////////////////////////////////////////

#if OPT_VERSION == 8 // Merged functions with manual CSE 
void Calc_Distance_Bearing(float * distance, float * bearing, PT_T * p1,  const PT_T * p2){
// calculates distance and bearing in degrees between locations (represented in degrees)	
	float p1LonRad, p1LatRad, p2LonRad, p2LatRad;
	float cosp2LatRad;
	float term1, term2;
	float angle;
	
	p1LonRad = p1->Lon*(PI/180);
	p2LonRad = p2->Lon*(PI/180);
	p1LatRad = p1->Lat*(PI/180);
	p2LatRad = p2->Lat*(PI/180);
	cosp2LatRad = cosf(p2LatRad);
	
  *distance = acosf(sinf(p1LatRad)*sinf(p2LatRad) + 
							cosf(p1LatRad)*cosf(p2LatRad)*
							cosf(p2LonRad - p1LonRad)) * 6371;
	
	term1 = sinf(p1LonRad - p2LonRad)*cosp2LatRad;
	term2 = cosf(p1LatRad)*sinf(p2LatRad) - 
					sinf(p1LatRad)*cosp2LatRad*cosf(p1LonRad - p2LonRad);
	angle = atan2f(term1, term2) * (180/PI);
	if (angle < 0.0)
		angle += 360;
	*bearing = angle;
}
#endif

void Find_Nearest_Waypoint(float cur_pos_lat, float cur_pos_lon, float * distance, float * bearing, 
	char  * * name) {
	// cur_pos_lat and cur_pos_lon are in degrees
	// distance is in kilometers
	// bearing is in degrees
		
	int i=0, closest_i;
	PT_T ref;
	float d, b, closest_d=1E10;

	*distance = *bearing = NULL;
	*name = NULL;

	ref.Lat = cur_pos_lat;
	ref.Lon = cur_pos_lon;
	strcpy(ref.Name, "Reference");

	while (strcmp(waypoints[i].Name, "END")) {
#if OPT_VERSION != 8
		d = Calc_Distance(&ref, &(waypoints[i]) );
		// b = Calc_Bearing(&ref, &(waypoints[i]) );
#else
		Calc_Distance_Bearing(&d, &b, &ref, &(waypoints[i]));
#endif
			
		// if we found a closer waypoint, remember it and display it
		if (d<closest_d) {
			closest_d = d;
			closest_i = i;
		}
		i++;
	}

#if OPT_VERSION != 8
	d = closest_d; // Calc_Distance(&ref, &(waypoints[closest_i]) );
	b = Calc_Bearing(&ref, &(waypoints[closest_i]) );
#else
	Calc_Distance_Bearing(&d, &b, &ref, &(waypoints[closest_i]));
#endif	
	// return information to calling function about closest waypoint 
	*distance = d;
	*bearing = b;
	*name = (char * ) (waypoints[closest_i].Name);
}
