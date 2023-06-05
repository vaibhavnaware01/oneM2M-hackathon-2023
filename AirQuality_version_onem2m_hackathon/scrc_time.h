
#ifndef SCRC_TIME_H_
#define SCRC_TIME_H_

#include <Arduino.h>

void sync_time();
uint64_t get_timestamp();
uint64_t get_timestamp(unsigned long milli);

String get_timestamp_str(uint64_t timestamp, uint16_t timeoffset);
String get_datestamp_str(uint64_t timestamp, uint16_t timeoffset);

String get_date_timestamp_str(uint64_t timestamp, uint16_t timeoffset);

#endif /* SCRC_TIME_H_ */
