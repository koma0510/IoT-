void setup() {
  Serial.begin(9600);
}

void loop() {
 int x = analogRead(A0);
 float I = x*(pow(10,-5) + 3.2 * pow(10,-3)) / 1024.0;
 float Lx = 3 * pow(10,5) * I;

 Serial.print("Lx = ");
 Serial.println(Lx);

 delay(1000);
}
