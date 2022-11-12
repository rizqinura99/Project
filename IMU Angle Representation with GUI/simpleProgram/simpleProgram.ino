#include <Wire.h>
#include <ADXL345.h>  // ADXL345 Accelerometer Library
#include <HMC5883L.h> // HMC5883L Magnetometer Library
#include <ITG3200.h> // ITG3200 Gyroscope Library
#include <Servo.h> //Servo Library

union {
  struct {
    float degreeSent;
  } parameter;
  byte packet[4];
} data;

ADXL345 acc; 
Servo MoveServo;

int ax,ay,az;  
int rawX, rawY, rawZ;
float X, Y, Z;
float rollrad, pitchrad;
float rolldeg, pitchdeg;
float aoffsetX, aoffsetY, aoffsetZ;
int rotateData, callX, callY, callZ;

void setup(){
  Serial.begin(9600);
  MoveServo.attach(8);
  acc.powerOn();
  for (int i = 0; i <= 200; i++) {
  acc.readAccel(&ax, &ay, &az);
  if (i == 0) {
    aoffsetX = ax;
    aoffsetY = ay;
    aoffsetZ = az;
    }
  if (i > 1) {
    aoffsetX = (ax + aoffsetX) / 2;
    aoffsetY = (ay + aoffsetY) / 2;
    aoffsetZ = (az + aoffsetZ) / 2;
    }
  }
  delay(1000);
}

void loop(){
  ImuRead();
  ServoRotate();
  SendData();
}

void ServoRotate() {
  rotateData = map(rolldeg, -90, 90, 0, 180);
  MoveServo.write(rotateData);
}

void SendData() {
  data.parameter.degreeSent = rolldeg;
  Serial.write('$');
  Serial.write(data.packet, sizeof(data.packet));
}

void ImuRead() {
  // code fragment for Accelerometer angles (roll and pitch)
  acc.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z
  rawX = ax - aoffsetX;
  rawY = ay - aoffsetY;
  rawZ = az  - (255 - aoffsetZ);
//  X = rawX/256.00; // used for angle calculations
//  Y = rawY/256.00; // used for angle calculations
//  Z = rawZ/256.00; // used for angle calculations

  X = ax/256.00; // used for angle calculations
  Y = ay/256.00; // used for angle calculations
  Z = az/256.00; // used for angle calculations
//  Serial.print("X :");Serial.print(ax);
//  Serial.print(" Y :");Serial.print(ay);
//  Serial.print(" Z :");Serial.println(az);
  rollrad = atan(Y/sqrt(X*X+Z*Z));  // calculated angle in radians
  pitchrad = atan(X/sqrt(Y*Y+Z*Z)); // calculated angle in radians
  rolldeg = 180*(atan(Y/sqrt(X*X+Z*Z)))/PI; // calculated angle in degrees
  pitchdeg = 180*(atan(X/sqrt(Y*Y+Z*Z)))/PI; // calculated angle in degrees
//  rolldeg = rolldeg*1.097;
  if (rolldeg >= 90) rolldeg =90;
  else if (rolldeg <= -90) rolldeg = -90;
  
  Serial.println(rolldeg);
//  delay(100);  
}
