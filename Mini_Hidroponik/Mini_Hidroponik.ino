#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16,2);
#include "cactus_io_SHT31.h"

cactus_io_SHT31 sht31;
long lastMillis;;

//Deklarasi PIN
int sensor[4] = {A1, A2, A3, A4};
#define LED_R1 3
#define Pump_R2 4
#define Fan_R3 5

//Deklarasi Variabel
#define R 10000
int v[4]= {0,0,0,0};
int R2[4] = {0,0,0,0};
float temperature[4] = {0,0,0,0};
float logR2[4] = {0,0,0,0};
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float humidity;
float temperatureSHT;
float temperatureTermistor;
float temperatureAverage;
int prosesRelay = 0;
  
void setup(){
 
  Serial.begin(9600);
  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("GREEN HOUSE MINI");
  lcd.setCursor(4,1);
  //Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  //delay(1000);//Wait before accessing Sensor
  pinMode(LED_R1, OUTPUT);
  pinMode(Pump_R2, OUTPUT);
  pinMode(Fan_R3, OUTPUT);
  digitalWrite(LED_R1, HIGH);
  digitalWrite(Pump_R2, HIGH);
  digitalWrite(Fan_R3, HIGH);
  delay(2000);
  lcd.clear();
  for(int i = 0; i<4;i++){
    pinMode(sensor[i], INPUT);
  }
  
  if (!sht31.begin()) {
    Serial.println("Count not find sensor. Check wiring and I2C address");
    while(1) delay(1);
  }
 
}
 
void loop(){
  Serial.println(prosesRelay);
  Baca_Suhu();
  Baca_Humidity();

  
  switch (prosesRelay){
  case 0:
    if (temperatureAverage <= 25){
      digitalWrite(LED_R1, HIGH);
      digitalWrite(Fan_R3, HIGH);
    }
    else if (temperatureAverage >= 30){
      digitalWrite(LED_R1, LOW);
      digitalWrite(Fan_R3, LOW);
    }
    else {
      digitalWrite(LED_R1, LOW);
      digitalWrite(Fan_R3, HIGH);
    }
    prosesRelay = 1;
    break;
  case 1:
    if (humidity <= 70){
      digitalWrite(Pump_R2, LOW);
    }
    else {
      digitalWrite(Pump_R2, HIGH);
    }
    prosesRelay = 0;
    break;
  }

 
}// end loop(

void Baca_Humidity(){
    humidity = sht31.getHumidity();
    Serial.print(sht31.getHumidity()); Serial.println(" %");
    lcd.setCursor(0,0);
    lcd.print("Humidity =");
    lcd.setCursor(10,0);
    lcd.print(humidity);
    lcd.setCursor(15,0);
    lcd.print("%");
    //delay(300);
}

void Baca_Suhu(){
    if(millis()-lastMillis > 100){
      for(int i=0; i<4; i++){
        v[i] = analogRead(sensor[i]);
        R2[i] = R * (1023.0 / (float)v[i] - 1.0);
        logR2[i] = log(R2[i]);
        temperature[i] = (1.0 / (c1 + c2*logR2[i] + c3*logR2[i]*logR2[i]*logR2[i]));
        temperature[i] = temperature[i] - 273.15;
      }
    temperatureTermistor = ((temperature[0]+temperature[1]+temperature[2]+temperature[3])/4);
    temperatureSHT = sht31.getTemperature_C();
    Serial.print(sht31.getTemperature_C()); Serial.println(" *C");
    temperatureAverage = (temperatureTermistor + temperatureSHT)/2;
    Serial.println(temperatureAverage);
    lcd.setCursor(0,1);
    lcd.print("Suhu = ");
    lcd.setCursor(7,1);
    lcd.print(temperatureAverage);
    lcd.setCursor(12,1);
    lcd.print("C");
    lastMillis = millis();
    }
  }
