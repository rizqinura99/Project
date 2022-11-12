int sensor[5] = {A0, A2, A4, A6, A8};
#define R 10000
int v[5]= {0,0,0,0,0};
int R2[5] = {0,0,0,0,0};
float temperature[5] = {0,0,0,0,0};
float logR2[5] = {0,0,0,0,0};
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temp1, temp2, temp3, temp4, temp5;
float tempBack, tempStomach, tempChest, tempBody, lastTemp;
long lastMillis, lastRead;

//A0 = perut
//A1 = 
//A2 = punggung kiri
//A3 = punggung kanan
//A4 = dada kanan
void setup() {
  Serial.begin(9600);
  for(int i = 0; i<5;i++){
    pinMode(sensor[i], INPUT);
  }
}

void loop() {
//  Serial.println("why");
  TempRead();
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
//  tempBody = (tempBack + tempChest + tempStomach) / 3;
  lastMillis = millis();
  }
  if (millis() - lastRead > 1000){
//    Serial.print("Temp 1 : ");Serial.println(temp1);
//    Serial.print("Temp 2 : ");Serial.println(temp2);
//    Serial.print("Temp 3 : ");Serial.println(temp3);
//    Serial.print("Temp 4 : ");Serial.println(temp4);
//    Serial.print("Temp 5 : ");Serial.println(temp5);
    Serial.print("Temp Back : ");Serial.println(tempBack);
    Serial.print("Temp Chest : ");Serial.println(tempChest);
    Serial.print("Temp Stomach : ");Serial.println(tempStomach);
//    Serial.print("Temp Body : ");Serial.println(tempBody);
    lastRead = millis();
  }
}
