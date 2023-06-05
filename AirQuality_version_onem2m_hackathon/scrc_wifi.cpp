

#include <Arduino.h>

#include "scrc_wifi.h"
#include "scrc_constants.h"

#ifdef CT_ESP_8266
#include <ESP8266WiFi.h>
#endif
#ifdef CT_ESP_32
#include <WiFi.h>
#endif

static bool nw_connected = false;

void nw_setup() {
	//WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
	WiFi.mode(WIFI_STA); //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
}

// timeout in milliseconds

//Start a Network Connection and check for Connection as well
bool nw_start(unsigned long timeout, int retry_delay) {
	static unsigned long last_ms = millis();

	debug_info("nw_start...\n");

	debug_info("last_ms: " + String(last_ms));
	debug_info(", timeout: " + String(timeout));
	debug_info(", retry_delay: " + String(retry_delay));

	if (nw_connected) {
		//If already connected show IP address in serial monitor
		debug_info("");
		debug_info("Connected to ");
		debug_info(MAIN_SSID);
		debug_info("IP address: ");
		debug_info(WiFi.localIP());
		return nw_connected;
	}

	Serial.println("Establishing connection to WiFi...");
	// WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
	WiFi.mode(WIFI_STA); //Only Station No AP, This line hides the viewing of ESP as wifi hotspot

	WiFi.begin(MAIN_SSID, MAIN_PASS); //Connect to your WiFi router

	bool error = false;

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		if ((millis() - last_ms) > timeout) {
			debug_info("last_ms: " + String(millis()));
			debug_err("timed out!!!\n");
			error = true;
			break;
		}
		debug_info(".");
		delay(retry_delay);
		//WiFi.begin(MAIN_SSID, MAIN_PASS);
	}

	if (error)
		nw_connected = false;
	else
		nw_connected = true;

	if (nw_connected) {
		//If connection successful show IP address in serial monitor
		debug_info("");
		debug_info("Connected to ");
		debug_info(MAIN_SSID);
		debug_info("IP address: ");
		debug_info(WiFi.localIP());
	} else {
		debug_info("");
		debug_info("Unable to connect!!!");
	}

	return nw_connected;
}

//Stop The Network Connection
void nw_stop() {
	if (nw_connected) {
		//TODO: use WiFi.forceSleepBegin instead of WiFi.disconnect
		//WiFi.disconnect();
		//WiFi.forceSleepBegin(sleepUs)
		nw_connected = false;
	}
}

//Check if Network is Connected
bool nw_is_connected() {

	(WiFi.status() == WL_CONNECTED) ? nw_connected = true : nw_connected =
												false;

#ifdef DEBUG
	if (nw_connected) {
		debug_info("");
		debug_info("Connected to ");
		debug_info(MAIN_SSID);
		debug_info("IP address: ");
		debug_info(WiFi.localIP());
	}
#endif

	return nw_connected;
}

