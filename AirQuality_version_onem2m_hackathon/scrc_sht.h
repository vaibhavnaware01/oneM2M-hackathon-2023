

#ifndef SCRC_SHT_H_
#define SCRC_SHT_H_

#include <HardwareSerial.h>
#include <Arduino.h>
#include <Wire.h>
//#include "scrc_constants.h"

void hw_setup_sht();
void hw_read_sht(float *buf_temp, float *buf_rH);

#endif /* SCRC_SHT_H_ */
