
#include "scrc_pub_onem2m.h"

#include "scrc_node.h"
#include "scrc_http.h"
#include "scrc_wifi.h"
#include "scrc_time.h"
#include "scrc_constants.h"

int publish_onem2m(const struct sensors_data *ptr_sensors_data);

int pub_pri_buf_data(const struct sensors_data *ptr_buf,
		const short int latest_buf_idx, const short int hist_buf,
		bool pub_hist) {

	// Local Variables
	int pub_count = 0;
	int status = 0;
	int no_records = 0;
	int end_idx = 0;

	//timing handling values
	unsigned long hw_time_interval = TIME_INTERVAL_PRIMARY;
	unsigned long om2m_pub_interval = TIME_INTERVAL_OM2M;

	debug_info("....sizeof(buffer): ");
	debug_info(sizeof(ptr_buf));
	debug_info(", sizeof(typeof(buffer)): ");
	debug_info(sizeof(typeof(ptr_buf[0])));
	debug_info("\n");

	int max = PRIMARY_BUF_COUNT;
	//TODO: dynamically calculate max
	//int max = sizeof(ptr_buf) / sizeof(typeof(ptr_buf[0]));

	debug_info("Start pub primary data...\n");
	debug_info("latest_buf_idx : " + String(latest_buf_idx));
	debug_info("hist_buf : " + String(hist_buf));

	// Calculating Number of records based on hist-idx and  latest-idx
	if (!pub_hist) {
		no_records = om2m_pub_interval / hw_time_interval;
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

	//Iterating over the entire data
	for (int i = start_idx; i <= end_idx; i++) {
		//check if WiFi available
		if (!nw_is_connected()) {
			debug_info("no n/w, do nothing!!!\n");
			status = E_OM2M_NW;
		}

		else if (!connect_http()) {
			debug_info("connecting onem2m failed!!!\n");
			status = E_OM2M_CONNECT;

		} else {
			status = publish_onem2m(&ptr_buf[i % max]);
		}

		//OM2M Error Handling
		if (status == SUCCESS_OM2M)
			pub_count++;
		else if (status == E_OM2M_NW) {
			break;
		} else if (status == E_OM2M_CONNECT) {
			//om2m connectivity error Handling
			break;
		} else if (status == E_OM2M_NO_RESPONSE) {

			//om2m no response error Handling
			break;
		} else if (status == E_OM2M_NO_RESPONSE) {

			//om2m empty response error Handling
			break;
		} else if (status == E_OM2M_EMPTY_RESPONSE) {

			//om2m empty response error Handling
			break;
		}
	}

	return (pub_count);
}

/*
 * Create request and publish function
 */
int publish_onem2m(const struct sensors_data *ptr_sensors_data) {
	char data[MAX_STRING_LEN];
	char name[30];
	//Format Data
	sprintf(name, "cin_%lld", ptr_sensors_data->time_stamp);
	sprintf(data, "[%lld, %.2f, %.2f, %.2f]", ptr_sensors_data->time_stamp,
			ptr_sensors_data->co2, ptr_sensors_data->temp,
			ptr_sensors_data->rh);

	debug_info(data);
	debug_info("\n");

	int ty = 4;

	// Build request in OM2M format
	//String req_data = String() + "{\"m2m:cin\":{\"con\":\"" + data + "\"}}";
String req_data =String() + "{\"m2m:cin\": {"

	+ "\"con\": \"" + data + "\","

	+ "\"lbl\": " + OM2M_DATA_LBL + ","

	+ "\"cnf\": \"text\""

	+ "}}";

	Serial.println(req_data);

	String link = String() + OM2M_MN + OM2M_AE + "/" + OM2M_DATA_CONT;
	Serial.print("onem2m ae/mn/node: ");
	Serial.println(link);

	// prepare the HTTP request
	String req = String() + "POST " + link + " HTTP/1.1\r\n" + "Host: " + CSE_IP
			+ "\r\n" + "X-M2M-Origin: " + OM2M_ORGIN + "\r\n"
			+ "Content-Type: application/json;ty=" + ty + "\r\n"
			+ "Content-Length: " + req_data.length() + "\r\n"
			+ "Connection: close\r\n\n" + req_data;

	return (post_request(req));
}

