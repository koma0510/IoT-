void setup() {
  digitalWrite(14,LOW);
  pinMode(14,OUTPUT);
}

void loop() {
  int i;
  for (i=0;i<3;i++){
    digitalWrite(14,HIGH);delay(200);
    digitalWrite(14,LOW);delay(200);
  }
  digitalWrite(14,HIGH);
  delay(800);
  digitalWrite(14,LOW);
  delay(1000);
}
