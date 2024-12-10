// DIPSW 設定読込みプログラム
void setup(){
 pinMode(12,INPUT);
 pinMode(13,INPUT);
 Serial.begin(9600);
}
void loop(){
 int stat=0;
 int bit1=digitalRead(12);
 int bit0=digitalRead(13);
 if(bit0==LOW){
  stat|=0x01;
 }
 if(bit1==LOW){
  stat|=0x02;
 }
 Serial.print("DIPSW status=");
 Serial.println(stat);
 delay(1000);
}
