#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

char ssid[] = "...";
char pass[] = "...";

const char* ntp_server = "ntp.nict.jp";
unsigned long last_sync_time = 0;
unsigned long last_treat_time = 0;

//network
const char* tcp_server = "iot.hongo.wide.ad.jp";
const int port = 10340; 

WiFiClient client;

int existed = 0;

#define OLED_RESET 2

Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  //wifi接続の可否判別
  int wifi_connect = 0;
  
   Serial.begin(115200);
  
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(0,0);
   display.println("Connecting to ");
   display.println(ssid);
   display.display();

   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   while (WiFi.status() != WL_CONNECTED && wifi_connect < 50) {
     wifi_connect++;
     delay(500);
   }
   display.println();
   display.display();

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("wifi connect");
    
    display.println("success!");
    display.println("IP address: ");
    display.println(WiFi.localIP());
    display.display();
  } else {
    display.println("failure");
    display.display();
    Serial.println("Fail to connect");
    while(1);
  }

  //時刻同期
  display.clearDisplay();
  display.setCursor(0,0);
  
  display.println("Sync to");
  display.println(ntp_server);
  
  if(syncNTPTime(ntp_server)){
     delay(500);
     
     unsigned long t=now();
     last_treat_time = t;
     char str_time[30];
     sprintf(str_time,"%04d-%02d-%02dT%02d:%02d:%02d", year(t),month(t),day(t), hour(t),minute(t),second(t));
     
     display.println();
     display.println("success!");
     display.println(str_time);
     display.display();
  } else {
    display.println();
    display.println("failure");
    display.display();
    Serial.println("Fail to get time");
    while(1);
  }

  //pinの設定
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  pinMode(14,OUTPUT);
  pinMode(16,INPUT);
}

void loop() {
  unsigned long t = now();
  getMDStatus();

  if(Serial.available()){
    String command = Serial.readStringUntil('\n');
    command.trim();
    if(command == "on"){
      Serial.println("Bz on");
      setBZ(true);
    } else if(command == "off"){
      Serial.println("Bz off");
      setBZ(false);    
    } 
  }
  
  if(t/10 != last_treat_time/10){
  //10秒おきに処理
    display.clearDisplay();
    display.setCursor(0,0);
    //ID
    int ID = getDIPSWStatus();
    
    //時刻
    char str_time[30];
    sprintf(str_time,"%04d-%02d-%02dT%02d:%02d:%02d", year(t),month(t),day(t), hour(t),minute(t),second(t));
  
    //照度
    int Lx = getIlluminance();

    //人感センサ
    int exist_in_tensec = existed;
    existed = 0;

    display.print(ID);
    display.print(",");
    display.print(str_time);
    display.print(",");
    display.print(Lx);
    display.print(",");
    display.println(exist_in_tensec);
    display.println();
    display.display();

    //TCPの確立
    if(client.connect(tcp_server, port)){  
      String message = String(ID) + "," + str_time + "," + String(Lx) + "," + String(exist_in_tensec);
      client.println(message);
      // サーバーからの応答を処理
      while (client.connected()) {
        if (client.available()) {
          String response = client.readStringUntil('\n');
          if (response.indexOf("OK") != -1) {
            display.println("...OK\r\n");
          } else if (response.indexOf("ERROR") != -1) {
            Serial.println("NG format");
            display.println("...NG\r\n");
          } else {
            Serial.println("Error in TCP");
            display.println("...ERR\r\n");
          }
          break;
        }
      }
      client.stop();
    } else {
      Serial.println("Error in TCP");
      display.println("...ERR\r\n");
    } 
    display.display();
    last_treat_time = t;
  }

  if(t/300 != last_sync_time/300){
    //5分ごとに同期
    if(!syncNTPTime(ntp_server)){
      Serial.println("Fail to sync time");
    }
  }
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
    last_sync_time = unix_time;
    return true;
  }
  return false;
}

//ID
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
void getMDStatus(){
  if(existed == 0){
    int exist = digitalRead(16);
    if(exist == HIGH){
      existed = 1;
    }else{
      existed = 0;
    }
  }
}

//照度
int getIlluminance(){
   int x = analogRead(A0);
   float I = x*(pow(10,-5) + 3.2 * pow(10,-3)) / 1024.0;
   int Lx = 3 * pow(10,5) * I;
   return Lx;
}

//ブザー
void setBZ(boolean on){
  if(on){
    digitalWrite(14,HIGH);
  } else{
    digitalWrite(14,LOW);
  }
}
