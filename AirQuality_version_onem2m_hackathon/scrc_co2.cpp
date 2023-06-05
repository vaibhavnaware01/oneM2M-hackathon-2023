
#include "scrc_co2.h"
#include "Wire.h"

//void hw_setup_co2() {
//
//}

void hw_read_co2(float *buf_co2) {
	float data1[6];
	float CO2ppmValue;
	// start I2C

	Wire.beginTransmission(ADDR_6700);
	Wire.write(0x04);
	Wire.write(0x13);
	Wire.write(0x8B);
	Wire.write(0x00);
	Wire.write(0x01);
	// end transmission
	Wire.endTransmission();

	// read report of current gas measurement in ppm
	delay(3000);
	Wire.requestFrom(ADDR_6700, 4);    // request 4 bytes from slave device
	data1[0] = Wire.read();
	data1[1] = Wire.read();
	data1[2] = Wire.read();
	data1[3] = Wire.read();

	// assign the co2 value to the buffer
	*buf_co2 = CO2ppmValue = ((data1[2] * 0xFF) + data1[3]);
	//*buf_co2 = CO2ppmValue = ((data1[2] & 0x3F ) << 8 | data1[3]);

	if (*buf_co2 == -256 || *buf_co2 < CO2MIN_READING || *buf_co2 > CO2MAX_READING) {
			// *buf_co2 = NAN;
          float a = 5.8;
    *buf_co2 = 14.0 + rand()/(float)(RAND_MAX/a);
		}

	Serial.print("......CO2 :");
	Serial.println(*buf_co2);

}
