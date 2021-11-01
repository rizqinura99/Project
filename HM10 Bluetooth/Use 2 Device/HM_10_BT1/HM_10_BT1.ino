#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

//int ldr = A0;
//bool led_state = false;
//int led_pin = 2;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
//  mySerial.print("AT");
//  mySerial.print("AT+ROLE1");
//  pinMode(ldr, INPUT);
//  pinMode(led_pin, OUTPUT);
}

void loop() { // run over and over
  if (mySerial.available() > 0) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()){
    mySerial.write(Serial.read());
  }
}
