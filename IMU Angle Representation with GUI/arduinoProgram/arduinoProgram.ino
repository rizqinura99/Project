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

ADXL345 acc; //variable adxl is an instance of the ADXL345 library
HMC5883L compass;
ITG3200 gyro = ITG3200();
Servo MoveServo;

float  gx,gy,gz;
float  gx_rate, gy_rate, gz_rate;
int ix, iy, iz;
float anglegx=0.0, anglegy=0.0, anglegz=0.0;
int ax,ay,az;  
int rawX, rawY, rawZ;
float X, Y, Z;
float rollrad, pitchrad;
float rolldeg, pitchdeg;
int error = 0; 
float aoffsetX, aoffsetY, aoffsetZ;
float goffsetX, goffsetY, goffsetZ;
unsigned long time, looptime;
int rotateData;

void setup(){
  Serial.begin(9600);
  MoveServo.attach(8);
  acc.powerOn();
  compass = HMC5883L();
  error = compass.SetScale(1.3); // Set the scale to +/- 1.3 Ga of the compass
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
  
 // Serial.println("Setting measurement mode to continous");
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
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
  for (int i = 0; i <= 200; i++) {
    gyro.readGyro(&gx,&gy,&gz); 
    if (i == 0) {
      goffsetX = gx;
      goffsetY = gy;
      goffsetZ = gz;
    }
    if (i > 1) {
      goffsetX = (gx + goffsetX) / 2;
      goffsetY = (gy + goffsetY) / 2;
      goffsetZ = (gz + goffsetZ) / 2;
    }
  }
  
  
  delay(1000);
  gyro.init(ITG3200_ADDR_AD0_LOW); 
 // Serial.print("zero Calibrating...");
 // gyro.zeroCalibrate(2500, 2);
 // Serial.println("done.");
}

void loop(){
  ImuRead();
//  ServoRotate();
//  SendData();
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
  time = millis();
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
  Serial.print("X :");Serial.print(X);
  Serial.print(" Y :");Serial.print(Y);
  Serial.print(" Z :");Serial.println(Z);
  rollrad = atan(Y/sqrt(X*X+Z*Z));  // calculated angle in radians
  pitchrad = atan(X/sqrt(Y*Y+Z*Z)); // calculated angle in radians
  rolldeg = 180*(atan(Y/sqrt(X*X+Z*Z)))/PI; // calculated angle in degrees
  pitchdeg = 180*(atan(X/sqrt(Y*Y+Z*Z)))/PI; // calculated angle in degrees
//  rolldeg = rolldeg*1.097;
//  if (rolldeg >= 90) rolldeg =90;
//  else if (rolldeg <= -90) rolldeg = -90;
  Serial.println(rolldeg);  

// Code fragment for Magnetometer heading (yaw)
  MagnetometerRaw raw = compass.ReadRawAxis();
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)
  float heading = atan2(scaled.YAxis, scaled.XAxis);
//  Serial.print(scaled.XAxis);Serial.println(scaled.YAxis);
  float declinationAngle = 0.0457;
  heading += declinationAngle;
  if(heading < 0)
    heading += 2*PI;
  if(heading > 2*PI)
    heading -= 2*PI;
  float headingDegrees = heading * 180/M_PI;
  
// Code fragment for Gyroscope (roll, pitch, yaw)  
  gyro.readGyro(&gx,&gy,&gz); 
  looptime = millis() - time;
  gx_rate = (gx-goffsetX) / 14.375;
  gy_rate = (gy-goffsetY) / 14.375;
  gz_rate = (gz-goffsetZ) / 14.375;
  delay(100);
 /* Serial.print(gx_rate);  // calculated angle in degrees
  Serial.print(",");
  Serial.print(gy_rate);
  Serial.print(",");
  Serial.print(gz_rate); 
  Serial.println(","); 
*/    
  anglegx = anglegx + (gx_rate * looptime);
  anglegy = anglegy + (gx_rate * looptime);
  anglegz = anglegz + (gz_rate * looptime);
// Accelerometer, Magnetometer and Gyroscope Output
//  Serial.println(rolldeg);
////  Serial.print(",");
//  Serial.println(pitchdeg);  // calculated angle in degrees
//  Serial.print("Yaw ");
//  Serial.println(yaw);
//  Serial.println(",");
  //Serial.print(anglegx); 
  // Serial.print(","); 
  //Serial.print(anglegy); 
  //Serial.print(","); 
  //Serial.println(anglegz);
  //Serial.println(looptime);
//  Output(raw, scaled, heading, headingDegrees);
}
