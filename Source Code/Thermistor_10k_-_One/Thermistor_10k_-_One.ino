long lastMillis;
int sensor = A2;

#define R 10000
int v = 0, R2 = 0;
float temperature = 0;
float logR2 = 0;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);
}

void loop() {
  if (millis() - lastMillis > 100){
    v = analogRead(sensor);
    R2 = R * (1023.0 / (float)v -1.0);
    logR2 = log(R2);
    temperature = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    temperature -= 273.15;
    Serial.println(temperature);
    lastMillis = millis();
  }
  
}
