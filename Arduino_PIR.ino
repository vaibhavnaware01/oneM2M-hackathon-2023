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
#define timeSeconds 10
const int motionSensor = 27;
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
boolean motion = false;

void IRAM_ATTR detectsMovement()
{
  digitalWrite(led, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup()
{
delay(5000);
Serial.begin(9600);
mySerial.begin(9600);
mySerial.println("wisun connect");
Serial.println("sending connect command");
Serial.println(F("Initialize System"));
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
  // Set LED to LOW
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
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

now = millis();
  if((digitalRead(led) == HIGH) && (motion == false)) {
    Serial.println("MOTION DETECTED!!!");
    motion = true;
  }
  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    Serial.println("Motion stopped...");
    mySerial.println("wisun socket_write 4 "+Node+"_to_"+data);  //connected device status
    startTimer = false;
    motion = false;
  }





if (Serial.available()) 
{ 
mySerial.write(Serial.read());
}
}
}
