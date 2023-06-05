
#ifndef SCRC_CONSTANTS_H_
#define SCRC_CONSTANTS_H_

/*
 uncomment to print debug statements
 */
#define DEBUG true

/*
 node type
 */

//#define NT_POLLUTION
#define NT_SMART_ROOM

/*
 Controller type
 */
//#define CT_ESP_8266
#define CT_ESP_32

/*
 Sensor PIN Definitions
 */
#define I2C_SDA                        21
#define I2C_SCL                        22

/*
 uncomment to publish to om2m
 */

#define PUB_OM2M

/*
 uncomment to publish to ThingSpeak
 */
#define PUB_THINGSPEAK

/*
 time interval in milliseconds
 */
//#define TIME_INTERVAL_5_MIN  300000L //5L * 60L * 1000L
#define TIME_INTERVAL_PRIMARY  60000 //5L * 60L * 1000L
#define TIME_INTERVAL_OM2M 60000L
#define TIME_INTERVAL_THINGSPEAK 60000

// count of sensor reading in 1 min intervals for 5 hrs = 1*60*5
#define PRIMARY_BUF_COUNT  300
#define DISPLAY_BUF_COUNT  10
/*
 WiFi credentials
 */


#define MAIN_SSID "SCRC_LAB"
#define MAIN_PASS "Scrciiith@123"


#define RETRY_WIFI_INTERVAL 60000  // in ms (1 min)
#define RETRY_WIFI_FACTOR 2     //multiplication factor user to increment the retry interval

#define STARTUP_WIFI_TIMEOUT  60000   //WiFi connection Timeout
#define STARTUP_WIFI_RETRY_DELAY 100  // WiFi reconnection delay

#define Timeoffset 19800
#define MAX_STRING_LEN 255

/*
 OneM2M connection details
 */
#define CSE_IP      "10.3.1.117"
#define CSE_PORT    8200
#define HTTPS     false
#define OM2M_ORGIN    "admin:admin"
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "Smart_spaces"
#define OM2M_DATA_CONT  "Ambience_monitor/Data"
#define OM2M_DATA_LBL "[\"AE-SR-AQ\", \"SR-AQ-KH95-03\", \"V1.0.05\", \"SR-AQ-V1.0.05\"]"

/*
 ThingSpeak connection details
 */
#define THING_SPEAK_ADDRESS ""
#define TIMEOUT  5000
#define NUM_FIELDS 3

#define NODE_NAME ""


#define THINGSPEAK_API_KEY ""



#define SHT21_POLLING_INTERVAL         10000
#define SHT21_ADDR                     0x40
#define si7021Addr                     0x40
#define MIN_HUM_READING				   0
#define MAX_HUM_READING				   100

#define MIN_TEMP_READING			   -40
#define MAX_TEMP_READING				80



/*
 CO2 Sensor Data
 */
#define CO2MIN_READING 0
#define CO2MAX_READING 2000
#define ADDR_6700  0x15 // default I2C slave address is 0x15

/*
 OLED Address
 */
#define OLED_ADDR 0x3C

/*
 Debug Function
 */
#ifdef DEBUG
#define debug_info(fmt, ...) Serial.println(fmt, ##__VA_ARGS__)
#define debug_err(fmt, ...) Serial.println(fmt, ##__VA_ARGS__)
#else
	#define debug_info(fmt, ...) ((void)0)
	#define debug_err(fmt, ...) ((void)0)
#endif

/*
 * Error Handelling Codes
 */

#define E_OM2M_NW -101
#define E_OM2M_CONNECT -102
#define E_OM2M_CONNECTION -103
#define E_OM2M_NO_RESPONSE -104
#define E_OM2M_EMPTY_RESPONSE -105

#define E_THINGSPEAK_NW -201
#define E_THINGSPEAK_CONNECT -202
#define E_THINGSPEAK_CONNECTION -203
#define E_THINGSPEAK_NO_RESPONSE -204
#define E_THINGSPEAK_EMPTY_RESPONSE -205

/*
 * Success Codes
 */
#define SUCCESS_OM2M 400
#define SUCCESS_THINGSPEAK 800

#endif /* SCRC_CONSTANTS_H_ */
