

#ifndef SCRC_NODE_H_
#define SCRC_NODE_H_

#define NT_SMARTROOMAQ

#include <stdint.h>
#include "scrc_constants.h"

struct sensors_data {
	uint64_t time_stamp;  // timestamp number

	// DHT22 or SHT21
	float temp;
	float rh;

#ifdef NT_POLLUTION

	// SDS011
	float pm25;
	float pm10;

	// T6713-5K
	float CO2;

	// Multi Channel Groove Gas
	float no2;
	float nh3;
	float co;
	float so2;

	// soft sensors
	float aqi;
	short int aql;
	short int aqlmp;

	// MAX4466
	short int noise;

#endif

#ifdef NT_WATER
  /*
     Water monitoring related sensor
  */
	float temp;
	double tdsvoltage;
	float tdswithouttemp;
	float tdswithtemp;
	double level;

#endif

#ifdef NT_SMARTROOMAQ
	// Smart Room Related Sensor
	float co2;

#endif

	short int_padding_1; // padding is based on the no of parameters and sizeof each parameter

};

#endif /* SCRC_NODE_H_ */
