#include <ESP8266WiFi.h>

const char* ssid = "...";
const char* password = "...";
const char* host = "iot.hongo.wide.ad.jp";
const int port = 10340; // ** 割り当てられたものを使用せよ**
unsigned char seq=0;

void setup() {
 Serial.begin(115200);
 delay(10);
 
 Serial.print("Connecting to ");
 Serial.println(ssid);

 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}

int value = 0;
char send_buf[100];

void loop() {
 delay(5000);
 
 Serial.print("connecting to ");
 Serial.print(host);
 Serial.print(":");
 Serial.println(port);

 // Use WiFiClient class to create TCP connections
 WiFiClient client;

 if (!client.connect(host, port)) {
  Serial.println("connection failed");
  return;
 }
 
 while(true){
   // This will send the request to the server
   sprintf(send_buf,"%d",++seq);
   client.print(send_buf);
   delay(1000);
   if(!client.connected()){
     client.stop();
     break;
   }
 }
}
 
