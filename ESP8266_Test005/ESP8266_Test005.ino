

void setup() {
  pinMode(16,INPUT);
  Serial.begin(9600);
  Serial.println("Starting 1-minute timer...");
}

void loop() {
  unsigned long startTime = millis();
  boolean exist = false;
  
  while(millis() - startTime < 60000){
    int bit = digitalRead(16);
    if(bit == HIGH){
      exist = true;
      break;
    }
    delay(1);
  }
  
 if(exist){
  Serial.println("there is a person");
  delay(60000- (millis() - startTime));
 } else {
  Serial.println("there isn't a person in 1min");
 }

 delay(1000); // 1秒間待機（任意）
}
