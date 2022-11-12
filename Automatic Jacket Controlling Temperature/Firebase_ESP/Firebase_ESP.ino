//#include <Servo.h>

#include <ESP8266WiFi.h>
//#include <Firebase.h>

//#include "FirebaseArduino.h"

#define FIREBASE_HOST "xxx.com"
#define FIREBASE_AUTH "token"
#define WIFI_SSID "Cobadulu"
#define WIFI_PASSWORD "rizqi1234"
#define DEBUG true

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
//  sendData("AT+CWJAP",2000,DEBUG);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void InitWifiModule() {
  sendData("AT+RST\r\n", 2000, DEBUG);                                                  
  sendData("AT+CWJAP=\"Cobadulu\",\"rizqi1234\"\r\n", 2000, DEBUG);        
  delay (3000);
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG);                                             
  delay (1500);
  sendData("AT+CIFSR\r\n", 1500, DEBUG);                                             
  delay (1500);
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG);                                             
  delay (1500);
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);
}

String sendData(String command, const int timeout, boolean debug){
  String response ="";
  Serial1.print(command);
//  Serial.print(command);
  long int time = millis();
  while((time+timeout) > millis()){
    while(Serial1.available()){
      char c = Serial1.read();
      response += c;
    }
  }
  if (debug) Serial.print(response);
  return response;
}
