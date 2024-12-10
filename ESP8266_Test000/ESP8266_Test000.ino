void setup() {
  pinMode(14,OUTPUT);
}

void loop() {
  digitalWrite(14,HIGH);
  delay(500);
  digitalWrite(14,LOW);
  delay(500);
}
