#include "cactus_io_SHT31.h"
#include <SoftwareSerial.h>
#include "PZEM004Tv30.h"

//PIN Untuk Relay
#define pinUV1_R1   4
#define pinUV2_R2   5
#define pinCMPC_R3  6
#define pinFAN_R4   7
#define pinRO_R5    8
#define pinWP_R6    9
#define pinWH_R7    10
//#define pinESP_R8   

//PIN Untuk Sensor
#define trigPin1    31
#define echoPin1    32
#define trigPin2    34
#define echoPin2    35
#define s0Pin     15
#define s1Pin     14
#define s2Pin     18
#define s3Pin     16
#define outPin    17
#define RpmPin    2
#define DISPLAY_W 16

PZEM004Tv30 pzem(12,11); //RX = 11, TX = 12

cactus_io_SHT31 sht31;

int LevFull_1 = 5;
int LevFull_2 = 10;
int Lev1 = 13;
int Lev3 = 7;
int ulang = 0;
//int statusON = 0;
//bool statusNormal = false;
//bool statusPanas = false;
long duration, distance;
uint32_t Tank1_value = 0;
uint32_t Tank2_value = 0;
int red = 0;
int green = 0;
int blue = 0;
float revolutions=0;
int rpm=0; // max value 32,767 16 bit
long  startTime=0;
long  elapsedTime;

union {
  struct {
    int warna; //2Bytes
    float power; //4Bytes
    int Tank1_value; //2Bytes
    int Tank2_value; //2Bytes
    float humidityTemp; //4Bytes
    float temperatureCTemp; //4Bytes
    bool CMPC_R3; //1Byte
    bool FAN_R4; //1Byte
    bool RO_R5; //1Byte
    bool WP_R6; //1Byte
    bool WH_R7; //1Byte
    int RPM; //2Bytes
  } param;
  byte packet[25];
} data;

union {
  struct {
    bool NORMAL;
    bool HOT;
    bool START;
  } param;
  byte packet[3];
} dataRecv;

void setup()
{
  Serial.begin(57600);
  //KURANG CARA INISIASI KE LAYAR RASPI

  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  digitalWrite(13,LOW);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(pinUV1_R1, OUTPUT);
  pinMode(pinUV2_R2, OUTPUT);
  pinMode(pinCMPC_R3, OUTPUT);
  pinMode(pinFAN_R4, OUTPUT);
  pinMode(pinRO_R5, OUTPUT);
  pinMode(pinWP_R6, OUTPUT);
  pinMode(pinWH_R7, OUTPUT);

  digitalWrite(pinUV1_R1,HIGH);
  digitalWrite(pinUV2_R2,HIGH);
  digitalWrite(pinCMPC_R3,HIGH);
  digitalWrite(pinFAN_R4,HIGH);
  digitalWrite(pinRO_R5,HIGH);
  digitalWrite(pinWP_R6,HIGH);
  digitalWrite(pinWH_R7,HIGH);

  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);
  pinMode(outPin, INPUT);
  digitalWrite(s0Pin, HIGH);
  digitalWrite(s1Pin, HIGH);

  if (!sht31.begin()) {
  //Serial.println("Count not find sensor. Check wiring and I2C address");
  while(1) delay(1);
  }
  
}

void btn_PowerOnOff() //Fungsi alat berjalan
{
        Serial.println("Sistem Mulai");
        Baca_sensor();
        serial();
        sendSerial();    
    //  Delay 1 Menit
    //    //Serial.println("Delay 1 Menit");
        for(ulang = 0; ulang <9; ulang++)  //satu kali loop 6 detik, maka 60 detik = 10 kali loop
        {
            Baca_sensor();
            serial();
            sendSerial();
        }
        //Menyalakan CMPC
        digitalWrite(pinCMPC_R3,LOW);
        data.param.CMPC_R3 = true;
        serial();
        sendSerial(); 
    //  Serial.println("Kompre Aktif");
    //  Delay 10 Menit Setelah CMPC ON
    //  Serial.println("Delay 10 menit");
        for(ulang = 0; ulang < 99; ulang++)  //satu kali loop 6 detik, maka 600 detik = 100 kali loop
        {
            Baca_sensor();
            serial();
            sendSerial();
        }
        digitalWrite(pinFAN_R4,LOW);
        data.param.FAN_R4 = true;
        digitalWrite(pinUV1_R1,HIGH);
        digitalWrite(pinUV2_R2,HIGH);
        serial();
        sendSerial();
        //Serial.print("FAN Aktif");  
        Baca_sensor();
        serial();
        sendSerial();
    
        int status_prosesA = 0;
        while (status_prosesA == 0){
            if((Tank1_value <= LevFull_1)&&(Tank2_value >= LevFull_2))
            {
                digitalWrite(pinRO_R5,LOW);
                data.param.RO_R5 = true;
                serial();
                sendSerial();
                //Serial.print("Pompa Aktif");
                Baca_sensor();
                serial();
                sendSerial();
            }
            else if((Tank1_value >= Lev1)&&(Tank2_value >= LevFull_2))
            {
                digitalWrite(pinRO_R5,HIGH);
                data.param.RO_R5 = false;
                serial();
                sendSerial();
                //Serial.print("Pompa Tidak Aktif");
                Baca_sensor();
                serial();
                sendSerial();
            }
            else if((Tank1_value <= Lev3)&&(Tank2_value <= LevFull_2))
            {
                digitalWrite(pinRO_R5,HIGH);
                data.param.RO_R5 = false;
                //Serial.print("Pompa Tidak Aktif");
                Baca_sensor();
                serial();
                sendSerial();
                break; 
            }
            else {
                Baca_sensor();
                serial();
                sendSerial();
            }
        }
        int status_prosesB = 0;
        while(status_prosesB==0){
            if((Tank1_value <= Lev3))
            {
                digitalWrite(pinCMPC_R3,HIGH);
                data.param.CMPC_R3 = false;
                //Serial.print("Kompressor Tidak Aktif");
                Baca_sensor();
                serial();
                sendSerial();
                break;
            }
            else{
                Baca_sensor();
                serial();
                sendSerial();
            }
        }
        //Delay 10 Menit
        //Serial.println("Delay 10 menit sblm selesai");
        for(ulang = 0; ulang <99; ulang++) //satu kali loop 6 detik, maka 600 detik = 100 kali loop
        {
            Baca_sensor();
            serial();
            sendSerial();
        }
        digitalWrite(pinFAN_R4,HIGH);
        data.param.FAN_R4 = false;
        serial();
        sendSerial();
        //Serial.print("FAN Tidak Aktif");
}

void btn_AirNormal()
{
     
    //codingan serial dari python, mengubah nilai statusNormal dari Trigger pada interface
    if (Serial.available()){
      Serial.readBytes(dataRecv.packet, sizeof(dataRecv.packet));
      if (dataRecv.param.NORMAL)
      {
        //Serial.print("Air Normal = Nyala");
        digitalWrite(pinWP_R6,LOW);
        data.param.WP_R6 = true;
        serial();
      }
    else
    {
        //Serial.print("Air Normal = Mati");
        digitalWrite(pinWP_R6,HIGH);
        data.param.WP_R6 = false;
        serial();
    }}
}

void btn_AirPanas()
{
    //codingan serial dari python, mengubah nilai statusPanas dari Trigger pada Interface
    if (Serial.available()){
      Serial.readBytes(dataRecv.packet, sizeof(dataRecv.packet));
      if (dataRecv.param.HOT)
      {
        //Serial.print("Air Panas = Nyala");
        digitalWrite(pinWH_R7,LOW);
        data.param.WH_R7 = true;
        serial();
      }
    else
      {
        //Serial.print("Air Panas = Mati");
        digitalWrite(pinWH_R7,HIGH);
        data.param.WH_R7 = false;
        serial();
      }}
}
void Baca_Tampil_RH_Temp()
{
    float AverageTemperature = 0;
    float AverageHumidity = 0;
    int MeasurementsToAverage = 4;
    float hasil[2];
    float temperatureCTemp = 0;
    float humidityTemp = 0;

    for(int i = 0; i < MeasurementsToAverage; ++i)
    {
        MeasureTemperature(hasil);
        //delay(150);
        AverageTemperature += hasil[1];
        AverageHumidity += hasil [0];
    }
    temperatureCTemp = AverageTemperature /= MeasurementsToAverage;
    humidityTemp = AverageHumidity /= MeasurementsToAverage;
    data.param.temperatureCTemp = temperatureCTemp;
    data.param.humidityTemp = humidityTemp;
}

void MeasureTemperature(float hasil[2])
{
    float h = sht31.getHumidity(); //Baca nilai humidity
    float t = sht31.getTemperature_C(); //Baca nilai suhu dalam Celcius
    float f = sht31.getTemperature_F(); //Baca nilai suhu dalam Fahrenheit

    hasil[0]=h;
    hasil[1]=t;
    hasil[2]=f;
    return hasil;
}

//void Baca_Tampil_Ultrasonik_Old()
//{
////    int MeasurementsToAverage = 5;
//    uint32_t ultra[1];
//    MeasureUltrasonik(ultra)
//    Tank1_value = ultra[1];
//    Tank2_value = ultra[0];
////    for(int i = 0; i < MeasurementsToAverage; ++i)
////    {
////        MeasureUltrasonik(ultra);
////        delay(20);
////        Tank1_value += ultra[1];
////        Tank2_value += ultra[0];
////    }
//    Serial.println(Tank1_value);
//    Serial.println(Tank2_value);
////    Tank1_value /= MeasurementsToAverage;
////    Tank2_value /= MeasurementsToAverage;
//    data.param.Tank1_value = Tank1_value;
//    data.param.Tank2_value = Tank2_value;
//}

void Baca_Tampil_Ultrasonik()
{
    SonarSensor(trigPin1, echoPin1);
    Tank1_value = distance;
    delay(20);
    SonarSensor(trigPin2, echoPin2);
    delay(20);
    Tank2_value = distance;
    Serial.println(Tank1_value);
    Serial.println(Tank2_value);
    data.param.Tank1_value = Tank1_value;
    data.param.Tank2_value = Tank2_value;
//    return ultra;  
}

void SonarSensor(int trigPin, int echoPin)
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1; 
}

void Baca_Tampilan_ColorSensor()
{
    MeasureColorSensor();
    if ((red >= 5) && (red <= 7) && (green >=19) && (green <= 23) && (blue >=19)&&(blue <= 23))
  {
    //Serial.println("Kotor");
    data.param.warna = 0; 
  }
  else if ((red >= 3) && (red <= 5) && (green >=9) && (green <= 12) && (blue >=9)&&(blue <= 12))
  {
    //Serial.println("Sedang"); 
    data.param.warna = 1;
    }
  else if ((red >= 3) && (red <= 6) && (green >=5) && (green <= 8) && (blue >=5)&&(blue <= 8))
  {
    //Serial.println("Bersih"); 
    data.param.warna = 2;
    }
   else {
    //Serial.println("Tidak Terbaca");
    data.param.warna = 3;
   }
}

void MeasureColorSensor()
{
    digitalWrite(s2Pin, LOW);
    digitalWrite(s3Pin, LOW);
    red = pulseIn(outPin, digitalRead(outPin) == HIGH ? LOW : HIGH);

    digitalWrite(s3Pin, HIGH);
    blue = pulseIn(outPin, digitalRead(outPin) == HIGH ? LOW : HIGH);

    digitalWrite(s2Pin, HIGH);
    green = pulseIn(outPin, digitalRead(outPin) == HIGH ? LOW : HIGH); 
}

void Baca_Sensor_Arus()
{
    float voltage = pzem.voltage();
    if(voltage != NAN){
        //Serial.print("Tegangan = "); //Serial.print(voltage); //Serial.println("V");
    }
    else {
        //Serial.println("Error Pembacaan Arus");
    }
    
    float current = pzem.current();
    if(current != NAN){
        //Serial.print("Arus = "); //Serial.print(current); //Serial.println("A");
    }
    else {
        //Serial.println("Error Pembacaan Arus");
    }
    
    float power = pzem.power();
    if(current != NAN){
        data.param.power = power;
        //Serial.print("Power: "); //Serial.print(power); //Serial.println("W");
    }
    else {
        //Serial.println("Error reading power");
        //pass
    }

}

void Baca_Tampilan_RPM()
{
  revolutions=0; rpm=0;
  startTime=millis();         
  attachInterrupt(digitalPinToInterrupt(2),interruptFunction,RISING);
  delay(1000);
  detachInterrupt(2);

  elapsedTime=millis()-startTime;

  if(revolutions>0)
  {
    rpm=(max(1, revolutions) * 60000) / elapsedTime;        //calculates rpm
  }
  data.param.RPM = rpm;
}

void interruptFunction() //interrupt service routine
{  
  revolutions++;
}

void fillMessage2DisplayWidth(String & message)
{
  if(message.length()<DISPLAY_W+1)
  {
    while(message.length()<DISPLAY_W)
    {
      message+=" "; 
    }
     return;
  }
    message = message.substring(0, DISPLAY_W-1); 
}

void Baca_sensor(){
    Baca_Tampil_RH_Temp();
    Baca_Tampil_Ultrasonik();
    Baca_Tampilan_ColorSensor();
    Baca_Sensor_Arus();
    Baca_Tampilan_RPM();
}

void sendSerial(){
  if (Serial.available()){
      Serial.readBytes(dataRecv.packet, sizeof(dataRecv.packet));
            if(dataRecv.param.NORMAL){
              digitalWrite(pinWP_R6,LOW);//normal_relay ON
            }
            else{
              digitalWrite(pinWP_R6,HIGH);//normal_relay OFF
            }
            
            //HOT
            if(dataRecv.param.HOT){
              digitalWrite(pinWH_R7,LOW);//hot_relay ON
            }
            else {
              digitalWrite(pinWH_R7,HIGH);//hot_relay OFF 
            }

            //START
            if(dataRecv.param.START == false){
              digitalWrite(pinUV1_R1, LOW); //uv1 off
              digitalWrite(pinUV2_R2, LOW); //uv2 off
              digitalWrite(pinCMPC_R3, HIGH); //cmpc off
              data.param.CMPC_R3 = false;
              digitalWrite(pinFAN_R4, HIGH); //fan off
              data.param.FAN_R4 = false;
              digitalWrite(pinRO_R5, HIGH); //pompa off
              data.param.RO_R5 = false;
              digitalWrite(pinWP_R6, HIGH); //normal off
              data.param.WP_R6 = false;
              digitalWrite(pinWH_R7, HIGH); //hot off
              data.param.WH_R7 = false;
              loop();
            }
            
        }
}
void serial(){
    Serial.write('@');
    Serial.write(data.packet, sizeof(data.packet));
//    delay(100);
}

void loop() 
{
  btn_PowerOnOff();
// int perulangan = 0;
// while (perulangan == 0){
//    if (Serial.available()){ 
//      Serial.readBytes(dataRecv.packet, sizeof(dataRecv.packet));
//      if (dataRecv.param.START == true){
//        btn_PowerOnOff();
//        break;
//      }
//      else if (dataRecv.param.START == false){
//        break;
//      }
//    }
//  }
}
