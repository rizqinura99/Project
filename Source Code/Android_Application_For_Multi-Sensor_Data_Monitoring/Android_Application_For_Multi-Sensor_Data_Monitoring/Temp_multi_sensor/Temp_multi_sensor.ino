float pin1=A1;
float value1=0;
float pin2=A2;
float value2=0;
void setup() { 
  Serial.begin(9600);
}
void loop() {
value1=analogRead(pin1);
Serial.println((value1*500)/1023);
Serial.println("|");
value2=analogRead(pin2);
Serial.println((value2*500)/1023);
delay(1000);

}
