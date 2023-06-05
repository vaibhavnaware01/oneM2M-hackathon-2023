#include <SPI.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <time.h>
#include "WiFiClient.h"
#include <WiFi.h>
String a;
int keyIndex = 0;  // your network key Index number (needed only for WEP)
WiFiClient client;
#define MAIN_SSID "SCRC_LAB"       // replace MySSID with your WiFi network name
#define MAIN_PASS "Scrciiith@123"  // replace MyPassword with your WiFi password
#include <HTTPClient.h>
#include <WiFi.h>
#include <time.h>
#define CSE_IP "10.3.1.117"
#define HTTPS true
#define CSE_PORT 8200
#define OM2M_ORGIN "admin:admin"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "Smart_spaces"
#define OM2M_Node_ID "Presence_detector"
#define OM2M_DATA_CONT "/Data"

// HTTP Client Object
HTTPClient http;

// Server URL
String server = "http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String() + OM2M_MN;
String path = server + String() + OM2M_AE + "/" + String() + OM2M_Node_ID + String() + OM2M_DATA_CONT;

int ledPin = 13;     // choose the pin for the LED
int inputPin = 15;    // choose the input pin (for PIR sensor)
int pirState = LOW;  // we start, assuming no motion detected
int val = 0;         // variable for reading the pin status


void getData() {
  val = digitalRead(inputPin);   // read input value
  if (val == HIGH) {             // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW);  // turn LED OFF
    if (pirState == HIGH) {
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  delay(500);
  WiFi.begin(MAIN_SSID, MAIN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("#");
  }
}
void loop() {
getData();
  String receive_data = "";  // read the incoming data as string
  if (pirState == HIGH) {
    receive_data = "1";
  } else {
    receive_data = "0";
  }
  Serial.println(a);
  // URL Connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(MAIN_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(MAIN_SSID, MAIN_PASS);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  delay(1000);
  http.begin(path);
  Serial.println(path);
  // Required Headers
  http.addHeader("X-M2M-Origin", OM2M_ORGIN);
  http.addHeader("Content-Type", "application/json;ty=4");
  String data = receive_data;
  Serial.println(data);
  String req_data = String() + "{\"m2m:cin\": {"
                    + "\"con\": \"" + data + "\","
                    + "\"cnf\": \"text\""
                    + "}}";
  int code = http.POST(req_data);
  http.end();
  if (code == 201) {
    Serial.println("Data Posted Successfully");
  } else {
    Serial.println("Data posting failed with http code-" + String(code));
  }
  delay(2000);
}