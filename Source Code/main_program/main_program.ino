#include <WiFiEsp.h>
#include "ThingSpeak.h"

WiFiEspClient client;

//Deklarasi Comm
char ssid[] = "Cobadulu";
char pass[] = "rizqi1234";
unsigned long ID = 1922421; //Sending Data ID
const char * API = "JLPRMP3BAV959RF7";
const char * APIR = "QDETEFBASPJGBVX2";
unsigned long ID2 = 1922443; //Receiving Data ID
const char * API2 = "BGLU35O93OJQDWHR";
const char * APIR2 = "7V6HHA84EFXO3A6L";
int field1 = 1,field2 = 2,field3 = 3;
int field4 = 4,field5 = 5,field6 = 6;
int field7 = 7,field8 = 8,statusCode;
//Sent Data
float backTemp_sent;
float chestTemp_sent;
float stomachTemp_sent;
//Receive Data
float tempVal_recv;
int backOpt_recv,chestOpt_recv,stomachOpt_recv;
int pwmVal_recv;
int sysState_recv;

//Deklarasi PIN
#define RelayBack 7
#define RelayChest 9
#define RelayStomach 11
#define RelayAll 13
#define VoltPin A3
int sensor[5] = {A2, A4, A6, A8, A10};
int inBack, inStomach, inChest;
int pwmBack = 6, pwmStomach = 8, pwmChest = 10;
const int redLed = 20, greenLed = 22;
float Vanalog, Vread;

//Deklarasi Variabel
#define R 10000
int v[5]= {0,0,0,0,0};
int R2[5] = {0,0,0,0,0};
float temperature[5] = {0,0,0,0,0};
float logR2[5] = {0,0,0,0,0};
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp1, temp2, temp3, temp4, temp5;
float tempBack, tempStomach, tempChest, tempBody, tempEnv, lastTemp;
long lastMillis;
int in_pwm, out_pwm, pwm_val;

//Control Variable
double tempSet = 43;
double in_val, out_val;
double Kp = 0, Ki = 0, Kd = 0;
bool backState, chestState, stomachState, allState;
int conditionState;

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  
  WiFi.init(&Serial1);
  ThingSpeak.begin(client);
  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println("Can't Connect");
    while(true);
  }
  Serial.println("WiFi Ready To Use");
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(RelayBack, OUTPUT);
  pinMode(RelayChest, OUTPUT);
  pinMode(RelayStomach, OUTPUT);
  pinMode(RelayAll, OUTPUT);
  pinMode(pwmBack, OUTPUT);
  pinMode(pwmChest, OUTPUT);
  pinMode(pwmStomach, OUTPUT);
  digitalWrite(RelayBack,HIGH);
  digitalWrite(RelayChest,HIGH);
  digitalWrite(RelayStomach,HIGH);
  digitalWrite(RelayAll,HIGH);
  
  for(int i = 0; i<5;i++){
    pinMode(sensor[i], INPUT);
  }

}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Connecting...");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid,pass);
      Serial.print(".");
    }
    Serial.println("Connected to WiFi");
  }
  conditionState = sysState_recv;
  if (conditionState = 0) AutoControlWithPWM();
  else if (conditionState = 1) ManualControlWithPWM();
  
  TempRead();
  VoltRead();
  ControlWithOnOff();
  ComHandler();
}

void ControlWithOnOff() {
  if(backOpt_recv == 1) backState = LOW;
  else backState = HIGH;
  if(chestOpt_recv == 1) chestState = LOW;
  else chestState = HIGH;
  if(stomachOpt_recv == 1) stomachState = LOW;
  else stomachState = HIGH;
  digitalWrite(RelayBack, backState);
  digitalWrite(RelayChest, chestState);
  digitalWrite(RelayStomach, stomachState);
//  digitalWrite(RelayAll, allState);
}

void AutoControlWithPWM() {
  tempEnv = tempVal_recv;
  if (tempEnv < tempBody) {
    if (tempBody < tempSet){
      pwm_val--;
      if (pwm_val > 252) pwm_val ==252;
      analogWrite(pwmBack, pwm_val);
      analogWrite(pwmChest, pwm_val);
      analogWrite(pwmStomach, pwm_val);
      allState = LOW;
    }
    else if (tempBody >= tempSet){
      pwm_val++;
      analogWrite(pwmBack, pwm_val);
      analogWrite(pwmChest, pwm_val);
      analogWrite(pwmStomach, pwm_val);
      allState = HIGH;
    } 
  }
  digitalWrite(RelayBack, allState);
  digitalWrite(RelayChest, allState);
  digitalWrite(RelayStomach, allState); 
}

void ManualControlWithPWM() {
  if (pwmVal_recv > 100) pwmVal_recv = 100;
  else if (pwmVal_recv < 0) pwmVal_recv = 0;
  in_pwm = abs(pwmVal_recv - 100);
  inBack = map(in_pwm, 0, 100, 0, 252);
  inStomach = inBack;
  inChest = inBack;
  analogWrite(pwmBack, inBack);
  analogWrite(pwmStomach, inStomach);
  analogWrite(pwmChest, inChest);
}

void TempRead() {
  if (millis() - lastMillis > 100){
    for(int i=0; i<5; i++){
        v[i] = analogRead(sensor[i]);
        R2[i] = R * (1023.0 / (float)v[i] - 1.0);
        logR2[i] = log(R2[i]);
        temperature[i] = (1.0 / (c1 + c2*logR2[i] + c3*logR2[i]*logR2[i]*logR2[i]));
        temperature[i] = temperature[i] - 273.15;
      }
  temp1 = temperature[0];
  temp2 = temperature[1];
  temp3 = temperature[2];
  temp4 = temperature[3];
  temp5 = temperature[4];
  //Filter Data
  if (temp3 - temp4 > 4) temp3 = temp4;
  else if (temp4 - temp3 > 4) temp4 = temp3;
  if (temp2 - temp5 > 4) temp2 = temp5;
  else if (temp5 - temp2 > 4) temp5 = temp2;
  if (abs(lastTemp - temp1) < 4 || lastTemp == 0) lastTemp = temp1;
  //Rata-Rata Data
  tempBack = (temp3 + temp4) / 2;
  tempChest = (temp2 + temp5) / 2;
  tempStomach = lastTemp;
  Serial.print("Back :" + String(tempBack)+" ");
  Serial.print("Chest :" + String(tempChest)+" ");
  Serial.println("Stomach :" + String(tempStomach));
  tempBody = (tempBack + tempChest + tempStomach) / 3;
  lastMillis = millis();
  }
}

void VoltRead() {
  Vanalog = analogRead(VoltPin);
  Vread = (Vanalog * 0.00489 * 5);
  Serial.print("Volt :"); Serial.println(Vread);
}

void ComHandler() {
  ThingSpeak.setField(field1,tempBack);
  ThingSpeak.setField(field2,tempChest);
  ThingSpeak.setField(field3,tempStomach);
  ThingSpeak.writeFields(ID,API);
  
  ThingSpeak.readMultipleFields(ID2,APIR2);
  tempVal_recv = ThingSpeak.getFieldAsLong(field1);
  backOpt_recv = ThingSpeak.getFieldAsLong(field2);
  chestOpt_recv = ThingSpeak.getFieldAsLong(field3);
  stomachOpt_recv = ThingSpeak.getFieldAsLong(field4);
  sysState_recv = ThingSpeak.getFieldAsLong(field5);
  pwmVal_recv = ThingSpeak.getFieldAsLong(field6);
  Serial.print(tempVal_recv);Serial.print("-");
  Serial.print(backOpt_recv);Serial.print("-");
  Serial.print(chestOpt_recv);Serial.print("-");
  Serial.print(stomachOpt_recv);Serial.print("-");
  Serial.print(sysState_recv);Serial.print("-");
  Serial.println(pwmVal_recv); //Ini harus diubah nilainya. Dibalik
}
void AppHandler() {
  //Ini belum ada cara pengirimannya ke aplikasinya kaya gimana
}

//double computePID(double inp) {
//  currentTime = millis();
//  elapsedTime = (double)(currentTime - previousTime);
//
//  error = tempSet - inp;
//  intError += error * elapsedTime;
//  rateError = (error - lastError)/elapsedTime;
//
//  double out = kp*error + ki*intError + kd*rateError;
//
//  lastError = error;
//  previousTime = currentTime;
//
//  return out;
//}



/*//Serial Data
union {
  struct {
    float backTemp;
    float chestTemp;
    float stomachTemp;
  } param;
  byte packet[12];
} sentData;

union {
  struct {
    float tempVal;
    bool backOpt;
    bool chestOpt;
    bool stomachOpt;
    bool allOpt;
    int backVal;
    int chestVal;
    int stomachVal;
    int sysState;
  } param;
  byte packet[16];
} recvData; */

/*if (Serial1.available()) { //Menerima data dari App
    char1 = Serial1.read();
    if (char1 == '$') {
      comIndex1 = 0;
      valid1 = true;
      tempEnv = recvData.param.tempVal;
      backState = recvData.param.backOpt;
      chestState = recvData.param.chestOpt;
      stomachState = recvData.param.stomachOpt;
      allState = recvData.param.allOpt;
      inBack = recvData.param.backVal;
      inChest = recvData.param.chestVal;
      inStomach = recvData.param.stomachVal;
      conditionState = recvData.param.sysState;
    }
    else if (valid1) {
      recvData.packet[comIndex1] = char1;
      comIndex1++;
      if (comIndex1 >= sizeof(recvData.packet)) valid1 = false;
    }
  }*/
