#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>

uint8_t MAC_array[6];
char MAC_char[18];

// Global variables
const char serverAddress[] = "debugrobotics.azurewebsites.net";
WiFiClient wifiClnt;
HttpClient client = HttpClient(wifiClnt, serverAddress, 80);

#include "CloudPrint.h"

//Wifi
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//ArduinoOTA
#include <ArduinoOTA.h>

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
  wifiManager.setDebugOutput(false);
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("STCI-BOT")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
}

void init_OTA()
{
  ArduinoOTA.onStart([]()
  {
    noInterrupts();
    timer0_detachInterrupt();
    ML_fwd(0);
    MR_fwd(0);
    interrupts();

      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      //Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]()
  {
      //Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
      //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    /*
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
      */
  });

  ArduinoOTA.begin();
  //Serial.println("OTA Ready");
}

void upload_IP() {
  String clientIP = ipToString(WiFi.localIP());
  client.get("/Home/SetIP?clientName=tk1bot&clientIP="+ clientIP);
  cloudPrintln(clientIP);
}

void setMotorSpeed(String speedStr) {
  int lastCmdEnd = speedStr.lastIndexOf("#");
  if (lastCmdEnd >= 9) {
    int rightMotorSpeed = speedStr.substring(lastCmdEnd - 4, lastCmdEnd).toInt();
    int leftMotorSpeed = speedStr.substring(lastCmdEnd - 9, lastCmdEnd - 5).toInt();

    if (leftMotorSpeed >= 0)
    {
      ML_fwd(leftMotorSpeed);
    }
    else
    {
      ML_rev(leftMotorSpeed * -1);
    }

    if (rightMotorSpeed >= 0)
    {
      MR_fwd(rightMotorSpeed);
    }
    else
    {
      MR_rev(rightMotorSpeed * -1);
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
  }
  //Serial.println(MAC_char);

  pinMode(INTERRUPT_PIN_L, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN_R, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_L), count_encoder_L, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_R), count_encoder_R, FALLING);

  init_wifi();
  init_OTA();
  upload_IP();
  wheel_state=stop;
  init_motors();
  //  init_timer();

  cloudPrintln("Ready to Receive");
}

String res = "";
void loop() {
   ArduinoOTA.handle();

   /*
   wheel_state=forward;
   Setpoint=50;
   delay(5000);
   ArduinoOTA.handle();
   wheel_state=reverse;
   Setpoint=50;
   delay(5000);
   */

   ML_fwd(150);
   MR_fwd(150);
   delay(5000);                  // waits for a second
   ArduinoOTA.handle();
   ML_rev(200);
   MR_rev(200);
   delay(5000);                  // waits for a second

   /*
   if (Serial.available() > 0)
   {
     char recvd;
     while (Serial.available() > 0)
     {
       recvd = Serial.read();
       res+= String(recvd);

       if (recvd == '#')
       {
         break;
       }
     }

     if (recvd == '#')
     {
        setMotorSpeed(res);
        res = "";
        Serial.println("Ack");
     }
   }
   delay(100);
   //cloudPrint(".");
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
 }
