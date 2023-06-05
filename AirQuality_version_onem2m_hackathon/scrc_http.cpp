

#include <WiFi.h>
#include "scrc_constants.h"
#include "scrc_http.h"

bool connect_http() {

	debug_info("HTTP Connecting: http://");
	debug_info(CSE_IP);
	debug_info(":");
	debug_info(CSE_PORT);
	debug_info("/");

	WiFiClient client;
	delay(1000);
	int r = 0;                                                   //retry counter
	//client.setTimeout(WIFI_TIMEOUT);
	while ((!client.connect(CSE_IP, CSE_PORT)) && (r < 30)) {
		delay(1000);
		debug_info(".");
		r++;
	}

	debug_info("requesting URL: ");
	debug_info(CSE_IP);

	if (r == 30) {
		debug_info(": Connection failed");
		// return error
		return false;
	} else {
		debug_info(": Connected to web");
		//return success
		return true;
	}
}

void disconnect_http() {
	Serial.println("Network Disconnected");
}

int post_request(String req) {

	Serial.print("connecting to ");
	Serial.println(CSE_IP);

	WiFiClient client;

	if (!client.connect(CSE_IP, CSE_PORT)) {
		Serial.println("connection failed to server");
		return E_OM2M_CONNECTION;
	}

	Serial.println(req + "\n");

	// Send the HTTP request
	client.print(req);

	unsigned long timeout = millis();
	while (client.available() == 0) {
		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			client.stop();
			return E_OM2M_NO_RESPONSE;
		}
	}
	//client = server.available();
	if (!client) {
		return E_OM2M_EMPTY_RESPONSE;
	}

	// Wait until the client sends some data
	Serial.println("reply was:");
	Serial.println("==========");
	String line;
	while (client.available()) {
		line = client.readStringUntil('\n');  //Read Line by Line
		Serial.println(line); //Print response
	}
	Serial.println("==========");
	Serial.println("closing connection");
	return SUCCESS_OM2M;
}

