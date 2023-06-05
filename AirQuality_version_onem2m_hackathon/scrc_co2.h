
#ifndef SCRC_CO2_H_
#define SCRC_CO2_H_

#include <Arduino.h>
#include "HardwareSerial.h"
#include <Wire.h>
#include "scrc_constants.h"

void hw_read_co2(float *buf_co2);

#endif /* SCRC_CO2_H_ */
