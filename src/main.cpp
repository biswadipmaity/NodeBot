#include "Arduino.h"
#include <ESP8266WiFi.h>
uint8_t MAC_array[6];
char MAC_char[18];

// Global variables
WiFiClient wifiClient;
const char* server = "debugrobotics.azurewebsites.net";

//Wifi
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//ArduinoOTA
#include <ArduinoOTA.h>

#include <Wire.h>
#include "SSD1306.h"
#include "images.h"
SSD1306  display(0x3c, D3, D4);

//Wheels Code
#define INTERRUPT_PIN_L D1
#define INTERRUPT_PIN_R D2
#include <wheels.h>

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void init_wifi()
{
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("STCI-BOT")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  Serial.println(WiFi.localIP());

  display.drawString(0, 44, ipToString(WiFi.localIP()));
  // write the buffer to the display
  display.display();
}

void init_display()
{
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void init_OTA()
{
  ArduinoOTA.onStart([]()
  {
    noInterrupts();
    timer0_detachInterrupt();
    interrupts();

      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]()
  {
      Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void upload_IP() {
  if (wifiClient.connect(server,80))
  {
     Serial.println("Uploading IP");
     // Make a HTTP request:
     String clientIP = ipToString(WiFi.localIP());
     wifiClient.print("GET /Home/SetIP?clientName=bot&clientIP="+ clientIP + " HTTP/1.1\n");
     wifiClient.print("Host: ");
     wifiClient.println(server);
     wifiClient.println("Accept: */*");
     wifiClient.println("Connection: close");
     wifiClient.println();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Started  : ");
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
  }
  Serial.println(MAC_char);

  pinMode(INTERRUPT_PIN_L, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_R, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_L), count_encoder_L, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_R), count_encoder_R, FALLING);

  init_display();
  display.drawString(0, 0, "Hello World OTA Rocks");
  display.drawString(0, 22, MAC_char);
  // write the buffer to the display
  display.display();

  init_wifi();
  init_OTA();
  upload_IP();
  init_motors();
  init_timer();
}

void loop() {
   ArduinoOTA.handle();
   /*
   //ML_fwd(120);
   ML_fwd(Output_L);
   MR_fwd(Output_R);
   delay(2000);
   ML_rev(Output_L);
   MR_rev(Output_R);
   delay(2000);
   */
   
   /*
   for(int i=0;i<128;i++)
   {
     ML_fwd(i);
     MR_fwd(i);
     delay(10);
   }

   for(int i=128;i>0;i--)
   {
     ML_rev(i);
     MR_rev(i);
     delay(10);
   }
   */

   /*
  ML_fwd();
  MR_fwd();
  delay(1000);                  // waits for a second
  ML_rev();
  MR_rev();
  delay(1000);                  // waits for a second
  */
}
