

#include "scrc_time.h"
#include "scrc_constants.h"
#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) )

WiFiUDP ntpUDP; // @suppress("Abstract class cannot be instantiated")
NTPClient timeClient(ntpUDP, "time.iiit.ac.in");

static uint64_t ntp_epoch_time = 0;
static uint64_t last_update = 0;
/*
 * Sync time with ntp server
 */
void sync_time() {

	timeClient.begin();

	timeClient.setTimeOffset(0);

	timeClient.update();
	// while (!timeClient.update()) {
	// 	timeClient.forceUpdate();
	// 	debug_info("In the while loop");
	// }

	Serial.print("Universal time: " + timeClient.getFormattedTime() + "\t");

	ntp_epoch_time = timeClient.getEpochTime();
	last_update = millis();

}

/*
 *  return timestamp in seconds
 */
uint64_t get_timestamp() {
	return ntp_epoch_time + ((millis() - last_update) / 1000);
}

/*
 *  return NTP epoch time
 */
uint64_t get_timestamp(unsigned long milli) {
	return ntp_epoch_time + ((milli - last_update) / 1000);
}

/*
 * return timestamp in HH:MM:SS format
 */
String get_timestamp_str(uint64_t timestamp, uint16_t timeoffset) {
	/*
	 adapted from https://github.com/arduino-libraries/NTPClient/blob/master/NTPClient.cpp
	 */
	unsigned long hours = ((timestamp + timeoffset) % 86400L) / 3600;
	String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

	unsigned long minutes = ((timestamp + timeoffset) % 3600) / 60;
	String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

	unsigned long seconds = (timestamp + timeoffset) % 60;
	String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

	return hoursStr + ":" + minuteStr + ":" + secondStr;
}

/*
 *  Return return timestamp in YY:MM:DD format
 */
String get_datestamp_str(uint64_t timestamp, uint16_t timeoffset) {
	/*
	 adapted from https://github.com/taranais/NTPClient/blob/master/NTPClient.cpp
	 */
	unsigned long rawtime = ((timestamp + timeoffset) / 86400L);

	unsigned long days = 0, year = 1970;
	uint8_t month;

	static const uint8_t monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
			30, 31 };

	while ((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawtime)
		year++;

	rawtime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
	days = 0;

	for (month = 0; month < 12; month++) {
		uint8_t monthLength;
		if (month == 1) { // February
			monthLength = LEAP_YEAR(year) ? 29 : 28;
		} else {
			monthLength = monthDays[month];
		}
		if (rawtime < monthLength)
			break;
		rawtime -= monthLength;
	}
	String monthStr = ++month < 10 ? "0" + String(month) : String(month); // January is month 1
	String dayStr = ++rawtime < 10 ? "0" + String(rawtime) : String(rawtime); // day of month
	return String(year) + "-" + monthStr + "-" + dayStr;
}

/*
 * return current timestamp by adding offset
 */
String get_date_timestamp_str(uint64_t timestamp, uint16_t timeoffset) {
	return (get_datestamp_str(timestamp, timeoffset) + " "
			+ get_timestamp_str(timestamp, timeoffset));
}
