#define VERSION "V1.00.16"

// Do not remove the include below
#include "SR_AQ.h"
#include <Arduino.h>

namespace {

const int MAIN_LOOP_DELAY = 5000;

}

namespace {

const int SERIAL_BAUD_RATE = 115200;

}

//#include <Wstring.h>

// buffer to store above readings
struct sensors_data data_primary[PRIMARY_BUF_COUNT];

short int idx_data_primary = 0;

short int hist_idx_om2m = 0;

short int hist_idx_thngspk = 0;

static bool wifi_connected = false;

/*
 * Timing Variables
 */
static bool no_network = false;

static uint32_t last_om2m_pub = 0;

static uint32_t last_thngspk_pub = 0;

static uint32_t last_ms_read_data_primary = 0;

static uint32_t last_retry_wifi = 0;

static uint32_t retry_wifi_interval = RETRY_WIFI_INTERVAL;

int count_hw = 0;

/*
 * Function Definitions
 */
void read_data_from_hw();
int retry_wifi();
bool is_om2m_pub_due(unsigned long milli);
bool is_thngspk_pub_due(unsigned long milli);

//The setup function is called once at startup of the sketch
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.print("***** Setup (from Eclipse ");
  Serial.print(VERSION);
  Serial.println(") *****");
  Serial.println(" ");

  //primary_buf_init(data_primary);

  nw_setup();
  wifi_connected = nw_start(STARTUP_WIFI_TIMEOUT, STARTUP_WIFI_RETRY_DELAY);

  //check if connected
  if (wifi_connected) {
    Serial.println("\n WiFi connected.");

    debug_info("Inside Time");

    // syn ts from ntp server
    sync_time();

    //get local ts
    uint64_t ts = get_timestamp();
    Serial.println(
        "Local timestamp: " + get_timestamp_str(ts, Timeoffset) + "\t"
            + "Date: " + get_datestamp_str(ts, Timeoffset));
    //Serial.print("timestamp uint64_t: ");
    String time_stamp = get_date_timestamp_str(ts, Timeoffset);
    Serial.println("UTC :-: Time :-: Date: " + time_stamp); // For UTC, take timeoffset as 0
    Serial.println("");
  }

  else
    Serial.println("\n WiFi not connected.");

  /*
   *   setup for various sensors connected to hw
   */

  hw_setup_sht();

   hw_setup_oled();

  /*
   Thing Speak Setup
   */

  if (wifi_connected) {
    //TODO: when Internet is not available, this function hangs. have a timeout to fix the issue. address all other thingspeak n/w issues
    // pub_setup_thingspeak();
  }

  Serial.println("***** End of setup *****");

}

void update_hist_idx_om2m(int pub_count) {
  //some data is published, update the hist_idx_om2m
  if (pub_count > 0) {
    //check if hist idx overflows the max buf
    if (hist_idx_om2m + pub_count < PRIMARY_BUF_COUNT) {
      hist_idx_om2m += pub_count;
    } else {
      hist_idx_om2m = hist_idx_om2m + pub_count - PRIMARY_BUF_COUNT;
    }
  }
}

void update_hist_idx_thingspk(int pub_count) {
  //some data is published, update the hist_idx_thngspk
  if (pub_count > 0) {
    if (hist_idx_thngspk + pub_count < PRIMARY_BUF_COUNT) {
      hist_idx_thngspk += pub_count;
    } else {
      hist_idx_thngspk = hist_idx_thngspk + pub_count - PRIMARY_BUF_COUNT;
    }
  }
}

/*
 *The loop function is called in an endless loop
 */
void loop() {
  /*
   * 1. read data from hardware
   * 2. publish to om2m
   * 3. publish to thingspeak
   * 4. display to OLED
   */

  debug_info(" Inside loop");

  // read data from hardware
  read_data_from_hw();

  if (nw_is_connected())
    wifi_connected = true;
  else
    wifi_connected = false;

   if (wifi_connected && is_om2m_pub_due(millis())) {

    int pub_count = pub_pri_buf_data(data_primary, idx_data_primary,
        hist_idx_om2m, true);

    //some data is published, update the hist_idx_om2m
    update_hist_idx_om2m(pub_count);

    //update the last pub time
    last_om2m_pub = millis();
  }

  if (wifi_connected && is_thngspk_pub_due(millis())) {

    int pub_count = pub_thingspeak(data_primary, idx_data_primary,
        hist_idx_thngspk, true);

    //some data is published, update the hist_idx_thngspk
    update_hist_idx_thingspk(pub_count);

    //update the last pub time
    last_thngspk_pub = millis();
  }

  display_oled(data_primary, idx_data_primary-1);

  if (!wifi_connected)
    retry_wifi();

  delay(MAIN_LOOP_DELAY);
}

/*
 * Check if OM2M is Due
 */
bool is_om2m_pub_due(unsigned long milli) {

  if (millis() - last_om2m_pub > TIME_INTERVAL_OM2M) {
    debug_info("Time interval : ");
    debug_info(milli);

    return (true);
  } else
    return (false);
}
/*
 * Check if Thingspeak is Due
 */
bool is_thngspk_pub_due(unsigned long milli) {
  if (millis() - last_thngspk_pub > TIME_INTERVAL_THINGSPEAK) {

    return (true);
  } else
    return (false);
}

/*
 * Retry Wi-Fi Connection Again
 */
int retry_wifi() {
  //regular interval with incremental back-off try to reconnect WiFi

  unsigned long milli = millis();
  if (milli - last_retry_wifi > retry_wifi_interval) {

    debug_info(
        get_date_timestamp_str(get_timestamp(milli), Timeoffset) + ": retrying WiFi...");
    wifi_connected = nw_start(STARTUP_WIFI_TIMEOUT,
    STARTUP_WIFI_RETRY_DELAY);
    last_retry_wifi = millis();
    retry_wifi_interval += retry_wifi_interval * RETRY_WIFI_FACTOR;
    wifi_connected = nw_start(STARTUP_WIFI_TIMEOUT,
    STARTUP_WIFI_RETRY_DELAY);
  }

  // if successfully connected to WiFi
  if (wifi_connected) {
    debug_info(
        get_date_timestamp_str(get_timestamp(millis()), Timeoffset) + ": WiFi connected");
    no_network = false;
  }
  return (0);
}
/*
 * Read Hardware Data
 */
void read_data_from_hw() {

  if (millis() - last_ms_read_data_primary > TIME_INTERVAL_PRIMARY) {
    uint64_t time_stamp = get_timestamp();
    String timestamp = get_date_timestamp_str(time_stamp, 0);
    data_primary[idx_data_primary % PRIMARY_BUF_COUNT].time_stamp =
        time_stamp;

    debug_info(
        get_date_timestamp_str(time_stamp, Timeoffset) + ": read_data_from_hw...\n");

    /*
     read data from hardware
     */
    // read data: Temperature and Relative Humidity from sht21
    hw_read_sht(&data_primary[idx_data_primary % PRIMARY_BUF_COUNT].temp,
        &data_primary[idx_data_primary % PRIMARY_BUF_COUNT].rh);

    // read data from co2 Telaire T6713
    hw_read_co2(&data_primary[idx_data_primary % PRIMARY_BUF_COUNT].co2);

    debug_info();
    debug_info("Count of Hardware outputs:  ");
    debug_info(count_hw++);
    last_ms_read_data_primary = millis();

    debug_info(
        "Temp: " + String(data_primary[idx_data_primary % PRIMARY_BUF_COUNT].temp ) +
        "Humidity: " + String(data_primary[idx_data_primary % PRIMARY_BUF_COUNT].rh) +
        "CO2 :" + String(data_primary[idx_data_primary % PRIMARY_BUF_COUNT].co2 ));


    if (idx_data_primary == PRIMARY_BUF_COUNT - 1) {
      idx_data_primary = 0;
    } else{
      idx_data_primary++;
    }
    if (!wifi_connected) {
      if (idx_data_primary == hist_idx_om2m) {
        update_hist_idx_om2m(1);
      }
      if (idx_data_primary == hist_idx_thngspk) {
        update_hist_idx_thingspk(1);
      }
    }
  }
}
