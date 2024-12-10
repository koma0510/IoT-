#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

char ssid[] = "...";
char pass[] = "...";

const char*  ntp_server = "ntp.nict.jp";
const char* next_server = "www.gutp.jp";

#define OLED_RESET 2


void setup() {
   Serial.begin(115200);
  
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected to Wifi");

   // 時刻同期
//   if (!syncNTPTime(ntp_server)) {
//     Serial.println("Failed to sync time with NTP server");
//   }
//   delay(10000);
}

void loop() {
  syncNTPTime(ntp_server);

  delay(15000);
  
  unsigned long currentTime = now();

  setTime(currentTime);
  Serial.print("Date (JST): ");
  Serial.print(year());
  Serial.print('/');
  Serial.print(month());
  Serial.print('/');
  Serial.print(day());
  Serial.print(" Time: ");
  Serial.print(hour());
  Serial.print(':');
  Serial.print(minute());
  Serial.print(':');
  Serial.println(second());

  delay(1000); // 1秒待機
}

//ブザー
void setBZ(boolean on){
  if(on){
    digitalWrite(14,HIGH);
  } else{
    digitalWrite(14,LOW);
  }
}

//プッシュSW
boolean getPushStatus(){
  int stat = digitalRead(2);
  if(stat==LOW){
    return true;
  }else{
    return false;
  }
}

int prev_stat = LOW;
boolean detectPushStatus(){
  int stat = digitalRead(2);
  if(stat==LOW && prev_stat==HIGH){
    prev_stat = stat;
    return true;
  }else{
    prev_stat = stat;
    return false;
  }
}

//DIP SW
int getDIPSWStatus(){
  int stat=0;
   int bit1=digitalRead(12);
   int bit0=digitalRead(13);
   if(bit0==LOW){
   stat|=0x01;
   }
   if(bit1==LOW){
   stat|=0x02;
   }
   return stat;
}

//人感センサ
boolean getMDStatus(){
  int exist = digitalRead(16);
  if(exist == HIGH){
    return true;
  }else{
    return false;
  }
}

//照度
int getIlluminance(){
   int x = analogRead(A0);
   float I = x*(pow(10,-5) + 3.2 * pow(10,-3)) / 1024.0;
   int Lx = 3 * pow(10,5) * I;
   return Lx;
}

//時刻
unsigned long getNTPTime(const char* ntp_server){
  WiFiUDP udp;
  udp.begin(8888);
  unsigned long unix_time=0;

  byte packet[48];
  memset(packet, 0, 48);
  packet[0] = 0b11100011;
  packet[1] = 0;
  packet[2] = 6;
  packet[3] = 0xEC;
  packet[12] = 49;
  packet[13] = 0x4E;
  packet[14] = 49;
  packet[15] = 52;
  
  udp.beginPacket(ntp_server, 123);
  udp.write(packet, 48);
  udp.endPacket();
  
  for(int i=0;i<10;i++){
    delay(500);
    if(udp.parsePacket()){
     udp.read(packet, 48);
     unsigned long highWord = word(packet[40], packet[41]);
     unsigned long lowWord = word(packet[42], packet[43]);
     unsigned long secsSince1900 = highWord << 16 | lowWord;
     const unsigned long seventyYears = 2208988800UL;
     unix_time = secsSince1900 - seventyYears + 32400UL; // 32400 = 9 hours (JST)
     break;
    }
  }
  udp.stop();
  return unix_time;
}

//時刻同期
boolean syncNTPTime(const char*ntp_server){
  unsigned long unix_time = getNTPTime(ntp_server);
  if(unix_time > 0){
    setTime(unix_time);
    return true;
  }
  return false;
}


  
