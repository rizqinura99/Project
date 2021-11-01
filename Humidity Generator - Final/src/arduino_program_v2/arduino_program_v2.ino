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
#define pinWP2_R8   45
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
int Lev1 = 11;
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
long  elapsedTime, EndTime;

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
  pinMode(pinWP2_R8, OUTPUT);

  digitalWrite(pinUV1_R1,LOW);
  digitalWrite(pinUV2_R2,LOW);
  digitalWrite(pinCMPC_R3,HIGH);
  digitalWrite(pinFAN_R4,HIGH);
  digitalWrite(pinRO_R5,HIGH);
  digitalWrite(pinWP_R6,HIGH);
  digitalWrite(pinWH_R7,HIGH);
  digitalWrite(pinWP2_R8,HIGH);

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
        Serial.println("Delay 1 Menit");
        for(ulang = 0; ulang <10; ulang++)  //satu kali loop 6 detik, maka 60 detik = 10 kali loop
        {
            Baca_sensor();
            serial();
            sendSerial();
        }
        //Menyalakan CMPC
        digitalWrite(pinCMPC_R3,LOW);
        digitalWrite(pinFAN_R4,LOW);
        data.param.CMPC_R3 = true;
        data.param.FAN_R4 = true;
        serial();
        sendSerial(); 
      Serial.println("Kompre Fan Aktif");
    //  Delay 10 Menit Setelah CMPC ON
    //  Serial.println("Delay 10 menit");
        for(ulang = 0; ulang <40 ; ulang++)  //satu kali loop 6 detik, maka 600 detik = 100 kali loop
        {
            Baca_sensor();
            serial();
            sendSerial();
        }
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
        for(ulang = 0; ulang <40; ulang++) //satu kali loop 6 detik, maka 600 detik = 100 kali loop
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


void Baca_Tampil_RH_Temp()
{
    float humidity = sht31.getHumidity(); //Baca nilai humidity
    float temperatureC = sht31.getTemperature_C(); //Baca nilai suhu dalam Celcius
    float temperatrueF = sht31.getTemperature_F(); //Baca nilai suhu dalam Fahrenheit
    data.param.temperatureCTemp = temperatureC;
    data.param.humidityTemp = humidity;
    Serial.println(humidity);
    Serial.println(temperatureC);
//    hasil[0]=h;
//    hasil[1]=t;
//    hasil[2]=f;
//    return hasil;
}

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
    long ColorTime = millis();
    if ((ColorTime-EndTime) >= 15000){
      MeasureColorSensor();
      long EndTime = millis();
    }
      Serial.print(red);Serial.print(green);Serial.println(blue);
      if ((red >= 5) && (red <= 7) && (green >=19) && (green <= 23) && (blue >=19)&&(blue <= 23))
      {
      Serial.println("Kotor");
      data.param.warna = 0; 
      }
      else if ((red >= 3) && (red <= 5) && (green >=9) && (green <= 12) && (blue >=9)&&(blue <= 12))
      {
      Serial.println("Sedang"); 
      data.param.warna = 1;
      }
      else if ((red >= 3) && (red <= 6) && (green >=5) && (green <= 8) && (blue >=5)&&(blue <= 8))
      {
      Serial.println("Bersih"); 
      data.param.warna = 2;
      }
      else {
      Serial.println("Tidak Terbaca");
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
//        Serial.print("Tegangan = "); Serial.print(voltage); Serial.println("V");
    }
    else {
        //Serial.println("Error Pembacaan Arus");
    }
    
    float current = pzem.current();
    if(current != NAN){
//        Serial.print("Arus = "); //Serial.print(current); //Serial.println("A");
    }
    else {
        //Serial.println("Error Pembacaan Arus");
    }
    
    float power = pzem.power();
    if(current != NAN){
        data.param.power = power;
        Serial.print("Power: "); Serial.print(power); Serial.println("W");
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
  Serial.println(rpm);
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
    Baca_Tampilan_RPM();
    Baca_Tampilan_ColorSensor();
    Baca_Sensor_Arus();
    
}

void sendSerial(){
  if (Serial.available()){
      Serial.readBytes(dataRecv.packet, sizeof(dataRecv.packet));
            if((dataRecv.param.NORMAL == true) && (dataRecv.param.HOT == false)){
              digitalWrite(pinWP_R6,LOW);//normal_relay ON
              digitalWrite(pinWH_R7,HIGH);
              digitalWrite(pinWP2_R8,HIGH);
            }
            else if((dataRecv.param.NORMAL == false) && (dataRecv.param.HOT == true)){
              digitalWrite(pinWP_R6,HIGH);
              digitalWrite(pinWH_R7,LOW);//hot_relay ON
              digitalWrite(pinWP2_R8,HIGH);
            }
            else if ((dataRecv.param.NORMAL == true) && (dataRecv.param.HOT == true)){
              digitalWrite(pinWP_R6,HIGH);
              digitalWrite(pinWH_R7,LOW); //hot and out the water
              digitalWrite(pinWP2_R8,LOW);
            }
            else{
              digitalWrite(pinWP_R6,HIGH);//normal_relay OFF
              digitalWrite(pinWH_R7,HIGH);//hot_relay OFF 
              digitalWrite(pinWP2_R8,HIGH);
            }
            
            //HOT
            
            
//            else {
//
//            }

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
//  btn_PowerOnOff();
 int perulangan = 0;
 while (perulangan == 0){
    if (Serial.available()){ 
      Serial.readBytes(dataRecv.packet, sizeof(dataRecv.packet));
      if (dataRecv.param.START == true){
        btn_PowerOnOff();
        break;
      }
      else if (dataRecv.param.START == false){
        break;
      }
    }
  }
}
