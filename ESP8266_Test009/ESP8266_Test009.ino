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
unsigned long last_sync_time=0;

#define OLED_RESET 2

Adafruit_SSD1306 display(OLED_RESET);

void setup()
{
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
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     display.print(".");
     display.display();
   }
   display.println();
   display.display();
  
   display.println("WiFi connected");
   display.println("IP address: ");
   display.println(WiFi.localIP());
   display.display();
   syncNTPtime();
}

void loop()
{
   unsigned long t=now();
   
   char str_time[30];
   sprintf(str_time,"%04d-%02d-%02dT%02d:%02d:%02d", year(t),month(t),day(t), hour(t),minute(t),second(t));
  
  
   display.clearDisplay();
   display.setCursor(0,0);
   display.println(str_time);
   display.display();
  
   if(t/300!=last_sync_time/300){
     syncNTPtime();
     last_sync_time=t;
   }
   delay(500);
}

boolean syncNTPtime(){
   unsigned long unix_time=getNTPtime();
   if(unix_time>0){
   setTime(unix_time);
   return true;
   }
   return false;
}

unsigned long getNTPtime(){
   WiFiUDP udp;
   udp.begin(8888);
   unsigned long unix_time=0UL;
   
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
