

#include "scrc_pub_thingspeak.h"

#include "scrc_constants.h"
#include "scrc_http.h"
#include "scrc_wifi.h"
#include "scrc_time.h"
#include "scrc_node.h"

#include <WiFi.h>
#include <ThingSpeak.h>

// Get thingspeak API key from scrc_constants
String writeAPIKey = String() + THINGSPEAK_API_KEY;

WiFiClient client; // @suppress("Abstract class cannot be instantiated")

int HTTPPost(int numFields, String fieldData[], String time_stamp);

String getResponse();

void pub_setup_thingspeak() {
	ThingSpeak.begin(client);  // Initialize ThingSpeak
}

int pub_thingspeak(const struct sensors_data *ptr_buf,
		const short int latest_buf_idx, const short int hist_buf,
		bool pub_hist) {

	int pub_count = 0;
	int status = 0;
	int no_records = 0;
	int end_idx = 0;

	String fieldData[ NUM_FIELDS];

	uint32_t hw_time_interval = TIME_INTERVAL_PRIMARY;
	uint32_t thngspk_pub_interval = TIME_INTERVAL_THINGSPEAK;

	debug_info("....sizeof(buffer): ");
	debug_info(sizeof(ptr_buf));
	debug_info(", sizeof(typeof(buffer)): ");
	debug_info(sizeof(typeof(ptr_buf[0])));
	debug_info("\n");

	int max = PRIMARY_BUF_COUNT;
	//TODO: dynamically calculate max
	//int max = sizeof(ptr_buf) / sizeof(typeof(ptr_buf[0]));

	debug_info("Start pub thingspeak data...\n");
	debug_info("latest_buf_idx : " + String(latest_buf_idx));
	debug_info("hist_buf : " + String(hist_buf));

	// Calculating number of records based on hist-idx and latest-idx

	if (!pub_hist) {
		no_records = thngspk_pub_interval / hw_time_interval;
	} else {
		if (latest_buf_idx > hist_buf) {
			no_records = latest_buf_idx - hist_buf;
		} else {
			no_records = max - hist_buf + latest_buf_idx;
		}
	}

	// Updating the start and end index
	int start_idx = latest_buf_idx + (max - no_records);
	if (latest_buf_idx == 0)
		end_idx = PRIMARY_BUF_COUNT - 1;
	else
		end_idx = max + latest_buf_idx - 1;

	debug_info("no_records: " + String(no_records) + "\n");
	debug_info("start_idx: " + String(start_idx % max) + "\n");
	debug_info("end_idx: " + String(end_idx % max) + "\n");
	debug_info("max_record_count: " + String(max) + "\n");

	for (int i = start_idx; i <= end_idx; i++) {

		//check if WiFi available
		if (!nw_is_connected()) {
			debug_info("no n/w, do nothing!!!\n");
			status = E_THINGSPEAK_NW;
		}

		int idx = i % max;

		debug_info("CO2 Data on ThingSpeak: ");
		debug_info(ptr_buf[i % max].co2);
		debug_info("Temperature Data on ThingSpeak: ");
		debug_info(ptr_buf[i % max].temp);
		debug_info("RH Data on ThingSpeak: ");
		debug_info(ptr_buf[i % max].rh);

		// Formatting data for publishing to thingspeak
		fieldData[1] = String(ptr_buf[idx].co2);
		fieldData[2] = String(ptr_buf[idx].temp);
		fieldData[3] = String(ptr_buf[idx].rh);

		String timestamp = get_date_timestamp_str(ptr_buf[idx].time_stamp, 0);
		status = HTTPPost( NUM_FIELDS, fieldData, timestamp);

		//updating and returning the number of data publishes
		if (status == SUCCESS_THINGSPEAK) {
			pub_count++;
			delay(200);
		}
		// Thingspeak Error Handling
		else if (status == E_THINGSPEAK_NW) {
			break;
		} else if (status == E_THINGSPEAK_CONNECT) {
			//Thingspeak connectivity error Handling
			break;
		} else if (status == E_THINGSPEAK_CONNECTION) {

			//Thingspeak no response error Handling
			break;
		} else if (status == E_THINGSPEAK_NO_RESPONSE) {

			//Thingspeak empty response error Handling
			break;
		} else if (status == E_THINGSPEAK_EMPTY_RESPONSE) {
			break;
		}

	}
	return (pub_count);
}

int HTTPPost(int numFields, String fieldData[], String time_stamp) {

	if (client.connect( THING_SPEAK_ADDRESS, 80)) {

		// Build the postData string.
		// If you have multiple fields, make sure the string does not exceed 1440 characters.
		String postData = "api_key=" + writeAPIKey + "&" + "created_at="
				+ time_stamp;
		for (int fieldNumber = 1; fieldNumber < numFields + 1; fieldNumber++) {
			String fieldName = "field" + String(fieldNumber);
			postData += "&" + fieldName + "=" + fieldData[fieldNumber];
		}

		// POST data via HTTP.
		debug_info("Connecting to ThingSpeak for update...");
		debug_info();

		//POST the data to thingspeak
		client.println("POST /update HTTP/1.1");
		client.println("Host: api.thingspeak.com");
		client.println("Connection: close");
		client.println("Content-Type: application/x-www-form-urlencoded");
		client.println("Content-Length: " + String(postData.length()));
		client.println();
		client.println(postData);

		debug_info(postData);

		// Printing the response
		String answer = getResponse();
		debug_info(answer);
	} else {

		debug_info("Connection Failed");
		return (E_THINGSPEAK_CONNECT);
	}

	return (SUCCESS_THINGSPEAK);
}

/*
 * Response handling function
 */
String getResponse() {
	String response;
	long startTime = millis();

	delay(200);
	while (client.available() < 1 && ((millis() - startTime) < TIMEOUT)) {
		delay(5);
	}

	if (client.available() > 0) { // Get response from server.
		char charIn;
		do {
			charIn = client.read(); // Read a char from the buffer.
			response += charIn;     // Append the char to the string response.
		} while (client.available() > 0);
	}
	client.stop();

	return (response);
}
