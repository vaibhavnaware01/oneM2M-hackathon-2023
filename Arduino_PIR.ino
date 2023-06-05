#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <IRremote.h>
IRsend irsend;
SoftwareSerial mySerial(7, 8);//rx,tx
int relayState;      // variable to store the read value
const String Node= "WN-LP02-02"; //node id
const String Node_OFF="AC02OFF";
const String Node_ON="AC02ON";
const long interval = 1000;           // interval at which to blink (milliseconds)
unsigned long previousMillis = 0;        // will store last time LED was updated
void setup()
{
delay(5000);
Serial.begin(9600);
mySerial.begin(9600);
mySerial.println("wisun connect");
Serial.println("sending connect command");
Serial.println(F("Initialize System"));
}
void loop()
{
String str="";
while(mySerial.available())
{
unsigned long currentMillis = millis();

str=mySerial.readStringUntil('\n');
Serial.println(str);
delay(5);
if(str.substring(1,13)=="IPv6 address")
{
Serial.println("working");
mySerial.println("wisun udp_server 5001");
mySerial.println("wisun udp_client fd12:3456::1 5005");
delay(1000);
mySerial.println("wisun socket_write 4 \""+Node+" is connected to BR\"");
delay(1000);
mySerial.println("wisun get wisun");
}
if(str.substring(1,19)=="isun.border_router")
{
str=mySerial.readStringUntil(']');
String data = str.substring(17);
delay(700);
Serial.println(data);
mySerial.println("wisun socket_write 4 "+Node+"_to_"+data);  //connected device status
}

if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
      mySerial.println("wisun socket_write 4 \""+Node+" is on\"");
    } else {
      ledState = LOW;
      mySerial.println("wisun socket_write 4 \""+Node+" is off\"");

    }
    // set the LED with the ledState of the variable:
  }

if (Serial.available()) 
{ 
mySerial.write(Serial.read());
}
}
}
