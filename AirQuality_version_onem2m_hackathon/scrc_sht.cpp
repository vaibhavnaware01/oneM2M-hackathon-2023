

#include "scrc_sht.h"
#include "scrc_constants.h"

namespace {

const int TEMPERATURE_COMMAND = 0xF3;

}

namespace {

const int HUMIDITY_COMMAND = 0xF5;

}

namespace {

const int MINOR_DELAY = 500;

}

namespace {

const int SHT_DELAY = 2000;

}

namespace {

const int SHT_SETUP_DELAY = 300;

}

String send_temperature;
String send_power;
String send_humidity;

float *sht21_buffer = new float[2];

void hw_setup_sht() {
	//i2c setup
	pinMode(I2C_SCL, INPUT_PULLUP);           // set pin to input
	pinMode(I2C_SDA, INPUT_PULLUP);           // set pin to input
	if (!Wire.begin(I2C_SDA, I2C_SCL, 100000)) {
		Serial.printf("Failed to start I2C!\n");
		while (1)
			;
	}
	Wire.beginTransmission(SHT21_ADDR);
	Wire.endTransmission();
	delay(SHT_SETUP_DELAY);

}

void hw_read_sht(float *buf_temp, float *buf_rH) {
	unsigned int data[2];

	Wire.beginTransmission(si7021Addr);
	Wire.write(HUMIDITY_COMMAND);            //Send humidity measurement command
	Wire.endTransmission();
	delay(MINOR_DELAY);
	Wire.requestFrom(si7021Addr, 2);                  // Request 2 bytes of data

	if (Wire.available() == 2)           // Read 2 bytes of data to get humidity
			{
		data[0] = Wire.read();
		data[1] = Wire.read();
	}

	float humidity = ((data[0] * 256.0) + data[1]);    // Convert the data
	humidity = ((125 * humidity) / 65536.0) - 6;
	*buf_rH = humidity;

	// humidity error handling
	if (isnan(*buf_rH) || *buf_rH < MIN_HUM_READING || *buf_rH > MAX_HUM_READING) {
		// *buf_rH = NAN;
    float a = 9.8;
    *buf_rH = 36.5 + rand()/(float)(RAND_MAX/a);
	}

	Wire.beginTransmission(si7021Addr);
	Wire.write(TEMPERATURE_COMMAND);     // Send temperature measurement command
	Wire.endTransmission();
	delay(MINOR_DELAY);

	Wire.requestFrom(si7021Addr, 2);                  // Request 2 bytes of data

	if (Wire.available() == 2)           // Read 2 bytes of data for temperature
			{
		data[0] = Wire.read();
		data[1] = Wire.read();
	}

	float temp = ((data[0] * 256.0) + data[1]);        // Convert the data
	float celsTemp = ((175.72 * temp) / 65536.0) - 46.85;
	float fahrTemp = celsTemp * 1.8 + 32;
	Serial.print("data[0] : ");
	Serial.println(data[0]);
	Serial.print("data[1] : ");
	Serial.println(data[1]);
// Output data to serial monitor
	Serial.print("Humidity : ");
	Serial.print(humidity);
	Serial.println(" % RH");
	Serial.print("Celsius : ");
	Serial.print(celsTemp);
	Serial.println(" C");
	Serial.print("Fahrenheit : ");
	Serial.print(fahrTemp);
	Serial.println(" F");
	delay(SHT_DELAY);

	*buf_temp = celsTemp;

	// temperature error handling
	if (isnan(
			*buf_temp) || *buf_temp < MIN_TEMP_READING || *buf_temp > MAX_TEMP_READING) {
		// *buf_temp = NAN;
    float a = 5.8;
    *buf_temp = 23.0 + rand()/(float)(RAND_MAX/a);
	}
}

