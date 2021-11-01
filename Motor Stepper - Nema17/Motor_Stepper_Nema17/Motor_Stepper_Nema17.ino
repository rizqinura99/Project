// defines pins numbers

const int stepPin = PB1; 
const int dirPin = PB0; 
const int enPin = PA5;
bool keadaan = LOW;
unsigned long perulangan = 0, lastT = 0;
int opsi, a;
void setup() {
  Serial.begin(57600);
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);
  digitalWrite(dirPin,HIGH);
  digitalWrite(stepPin,HIGH);
  
}
void loop() {
  // FUNGSI MEMILIH KONDISI //
  switch(opsi){
    case 0:
    Serial.println("Pilih Perputaran Yang Diinginkan");
    Serial.println("1. 30 derajat");
    Serial.println("2. 45 derajat");
    Serial.println("3. 90 derajat");
    Serial.println("4. 135 derajat");
    Serial.println("5. 180 derajat");
    Serial.println("6. 210 derajat");
    Serial.println("7. 270 derajat");
    Serial.println("8. 335 derajat");
    break;
    
    case 1:
    Serial.println("30 Derajat");
    a = 133;
    Putar();
    opsi = 0;
    break;

    case 2:
    Serial.println("45 Derajat");
    a = 200;
    Putar();
    opsi = 0;
    break;

    case 3:
    Serial.println("90 Derajat");
    a = 400;
    Putar();
    opsi = 0;
    break;

    case 4:
    Serial.println("135 Derajat");
    a = 600;
    Putar();
    opsi = 0;
    break;

    case 5:
    Serial.println("180 Derajat");
    a = 800;
    Putar();
    opsi = 0;
    break;

    case 6:
    Serial.println("210 Derajat");
    a = 933;
    Putar();
    opsi = 0;
    break;

    case 7:
    Serial.println("270 Derajat");
    a = 1200;
    Putar();
    opsi = 0;
    break;

    case 8:
    Serial.println("335 Derajat");
    a = 1577;
    Putar();
    opsi = 0;
    break;
  }
  if (Serial.available())
  {
    char command = Serial.read(); 
    if (command == '0'){
    opsi = 0;
    Serial.println("Pilih 0");
  }
  else if (command == '1'){
    opsi = 1;
    Serial.println("Pilih 1");
  }
  else if (command == '2'){
    opsi = 2;
    Serial.println("Pilih 2");
  }
  else if (command == '3'){
    opsi = 3;
    Serial.println("Pilih 3");
  }
  else if (command == '4'){
    opsi = 4;
    Serial.println("Pilih 4");
  }
  else if (command == '5'){
    opsi = 5;
    Serial.println("Pilih 5");
  }
  else if (command == '6'){
    opsi = 6;
    Serial.println("Pilih 6");
  }
  else if (command == '7'){
    opsi = 7;
    Serial.println("Pilih 7");
  }
  }
//    perulangan = micros();
//    keadaan != keadaan;
//    digitalWrite(stepPin,keadaan);
//    delayMicroseconds(350); 
//  digitalWrite(stepPin, HIGH);
//  delayMicroseconds(200);
//  digitalWrite(stepPin, LOW);
//  delayMicroseconds(200);
//  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
//  // Makes 200 pulses for making one full cycle rotation
  
  delay(1000); // One second delay

  /*digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 800; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
  delay(1000);*/
  
}

void Putar(){
  for(int x = 0; x < a; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);  
  }
  digitalWrite(dirPin,LOW);
  delay(1000);
  for(int x = 0; x < a; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);  
  }
  digitalWrite(dirPin,HIGH);
  
}
