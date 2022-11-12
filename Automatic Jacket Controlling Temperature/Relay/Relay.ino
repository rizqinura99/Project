//Deklarasi PIN
#define RelayBack 7
#define RelayChest 9
#define RelayStomach 11
#define RelayAll 13
const int b1 = 2;
bool relayState = LOW;
int count;
long lastMillis;
int command;

void setup() {
  Serial.begin(9600);
  pinMode(b1, INPUT_PULLUP);
  pinMode(RelayBack, OUTPUT);
  pinMode(RelayChest, OUTPUT);
  pinMode(RelayStomach, OUTPUT);
  pinMode(RelayAll, OUTPUT);
}

void loop() {
//  if (digitalRead(b1) == LOW) count++;
//  if (count == 2){
//    relayState =! relayState;
//    count = 0;
//  }
  if (Serial.available()) command = Serial.read();
//  Serial.println(command);
  if (command == 'a' ) {
    relayState = HIGH;
    Serial.println("Clicked a");
  }
  else if (command == 'b') {
    relayState = LOW;
    Serial.println("Clicked b"); 
  }
//  if (millis() - lastMillis > 5000){
//    relayState =! relayState;
//    lastMillis = millis();
//  }
//  Serial.println(relayState);
  digitalWrite(RelayChest, relayState);
  digitalWrite(RelayStomach, relayState);
  digitalWrite(RelayAll, relayState);
  digitalWrite(RelayBack, relayState);
}
