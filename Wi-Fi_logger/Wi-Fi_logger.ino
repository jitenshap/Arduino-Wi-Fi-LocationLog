#include "AtWiFi.h"
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <SPI.h>
#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"
#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB // New colour
File myFile;
void setup() 
{
  Serial.begin(115200);
  
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN); tft.setFreeFont(FSB12);
  tft.setCursor(0, 18);
  tft.println("Wi-Fi initializing");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 18);
  tft.println("Done");

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 18);
  tft.println("SD initializing");
  Serial.print("Initializing SD card...");
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) 
  {
    Serial.println("initialization failed!");
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 18);
    tft.println("SD initialization failed");
    while (1);
  }

  Serial.println("initialization done.");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 18);
  tft.println("Done");
}
 
void loop() 
{
  long ptime = millis();  //だいたい30秒間隔になるように調整
  Serial.println("Start scan");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 18);
  tft.println("Scanning");
  int n = WiFi.scanNetworks();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 18);
  tft.println("Done");  
  Serial.println("scan done");
  if (n == 0) 
  {
    Serial.println("no networks found");
  }
  else 
  {
    Serial.print(n);
    Serial.println(" networks found");

    File locFile;
    locFile = SD.open("bssids.csv", FILE_APPEND);
    locFile.print((String)millis() + ",");  //タイムスタンプ
    
    for (int i = 0; i < n; ++i) 
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(") ");
      String bssid = "";
      if(WiFi.BSSID(i)[0] < 0x10) bssid += "0";
      bssid += String(WiFi.BSSID(i)[0], HEX);
      bssid += ":";
      if(WiFi.BSSID(i)[1] < 0x10) bssid += "0";
      bssid += String(WiFi.BSSID(i)[1], HEX);
      bssid += ":";
      if(WiFi.BSSID(i)[2] < 0x10) bssid += "0";
      bssid += String(WiFi.BSSID(i)[2], HEX);
      bssid += ":";
      if(WiFi.BSSID(i)[3] < 0x10) bssid += "0";
      bssid += String(WiFi.BSSID(i)[3], HEX);
      bssid += ":";
      if(WiFi.BSSID(i)[4] < 0x10) bssid += "0";
      bssid += String(WiFi.BSSID(i)[4], HEX);
      bssid += ":";
      if(WiFi.BSSID(i)[5] < 0x10) bssid += "0";
      bssid += String(WiFi.BSSID(i)[5], HEX);
      
      Serial.print(bssid);
      locFile.print(bssid);
      if(i != (n - 1))
      {
        locFile.print(",");
      }
      else
      {
        locFile.println();
      }
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
    }
    locFile.close();
    
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 18);
    tft.println("Wrote log");    
  }
  ptime = millis() - ptime;

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 18);

  tft.print("Wait");
  long delayDur = millis() + (long)30000 - ptime;
  if(ptime > 0)
  {
    while(((long)delayDur - (long)millis()) > 0)
    {
      tft.print(".");
      delay(1000);
    }
  }
}
