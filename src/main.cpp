/*
 Titel        : ESP32 mit TTF-St7735 Display über OTA
 Info         : - Achtung - ESP32 mit der Lib-Version 3.0.2
 Beschreibung : Nur die wichtigs Sachen für OTA
              : Name in Fritzbox "ESP32-ST7735"
 Arduino      : ESP32
 Modul ID     : ESP32 S
 Projekt-Nr.  : 15
 Datum        : 04.07.2023
 Schaltung in : wird noch erstellt
 Hardwareinfo : TFT-Display über Software SPI
              : 
 Status       : in Arbeit (OK)
 Version      : V1.0.0 (20230704)
 Einsatz      : TFT Display mit Anzeige aus HA MQTT mit Encoder für Menü's anwahlt
 Hinweis      : Erstellt mit PlatformIO (OTA Port muss von Hand eingegeben werden IP Leerzeichen und Hostname)
              : z.B 192.168.178.160 ESP-ST7735 (Achtung - nach Upload mit USB sind die Port für ATO weg)
              : 
 ToDo         :

ESP32 various dev board   : CS: TX, DC:  4, RST:  5, BL:  5, SCK: 19, MOSI: 18, MISO: 12


*/
/* History 20210621 begin
 V1.0.0  22.06.2021 abgeschlossen OK
   - WiFi Einstellungen
   - Webserver für Text zur IP
   - OTA Einstellungen
   - Display eingebunden
  V1.0.1 
   - Rahmen
   - ENCODER
*/


#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ST7735.h>  // include Adafruit ST7735 TFT library
#include <Encoder.h>
#include <Arduino_GFX_Library.h>
// eigene Dateien
// #include "rahmen.h"


// ST7735 TFT module connections
#define TFT_RST   16     // TFT RST 
#define TFT_CS    5      // TFT CS  
#define TFT_DC    15     // TFT DC  

// initialize ST7735 TFT library with hardware SPI 
#define SCK       18     // CLK
#define MOSI      23     // MOSI
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const int PIN_A = 32;   //ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!
const int PIN_B = 33;   //ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
const int BUTTON = 21;   //ky-040 sw  pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!

int16_t Z = 0;
int16_t position = 0;
boolean BUTTON_Status = false;

byte lastButtonState = 0;
const int debounceTime = 30; // millis
unsigned long lastButtonTimeChange = 0;
volatile int BUTTON_Flag;
byte toggleState = 0;
const int LEDPin = 2;

// Encoder encoder(PIN_A,PIN_B);
long altPosition = -999;
const char* ssid = "klncqwoez283431pidd1ic9r32f8931";       // Ihr Wlan,Netz SSID eintragen
const char* passw = "h785fh6108b01";                        // Ihr Wlan,Netz Passwort eintragen
const char* mqtt_server = "192.168.178.65";                 // MQTT Server

#define ESPHostname "ESP32_ST7735"

String Version = "V1.0.1 ";
String Proj = " Proj-15";
// verwendete Funktionen
void WiFiInit();
void Anfangs_bild();
void Rahmen1();
void Display_SW();


IRAM_ATTR void ISR_button() {
  BUTTON_Flag = 1;
  // Serial.print("ausgelöst");
}


/////////////////////////////////   setup   ////////////////////////////////////////////////////////   
void setup()  {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT);
  pinMode(PIN_A, INPUT);
  pinMode(PIN_B, INPUT);

  WiFiInit();
  Serial.println("   Es geht los...");
  tft.initR(INITR_BLACKTAB);                        // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);                     // füllt screen mit black color
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  tft.setRotation(1);
  tft.setTextSize(1);                               // text size = 1
  Serial.print("Jetzt interrupt:");
  attachInterrupt(digitalPinToInterrupt(BUTTON), ISR_button, CHANGE);
  Anfangs_bild();
  ArduinoOTA.setHostname(ESPHostname);
  ArduinoOTA.begin();
  delay(5900);
  void Display_SW();
  
}
/////////////////////////////////   setup - Ende   ////////////////////////////////////////////////////////////


void loop()  {
  ArduinoOTA.handle();
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
///  long newPosition = encoder.read();
///     if (newPosition != altPosition){
///     altPosition = newPosition;
  tft.setCursor(2,90);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(85,90);
  tft.print(BUTTON_Flag);
  Serial.println(BUTTON_Flag);
  // BUTTON_Status = digitalRead(BUTTON);
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
  if (BUTTON_Flag == 0) {
    tft.setCursor(50,100);
    tft.print("nicht Gedrueckt ");
    Serial.println("nicht Gedrueckt");
    } else 
      if (BUTTON_Flag == 1) {
        // tft.setCursor(50,100);
        // if (millis() - lastButtonTimeChange > debounceTime && BUTTON_Flag) {
        //   lastButtonTimeChange = millis();
        //   if(digitalRead(BUTTON) == 0 && lastButtonState == 1) {
        //     toggleState =! toggleState;
        //     digitalWrite(LEDPin, toggleState);
        //     lastButtonState = 0;
        //     tft.print("nicht Gedrueckt ");
        //     Serial.println("nicht Gedrueckt");
        //   } else 
        //     if(digitalRead(BUTTON) == 1 && lastButtonState == 0) {
        //       lastButtonState = 1;
        //       tft.setTextColor(ST7735_RED, ST7735_BLACK);
        //       tft.setCursor(50,100);
        //       tft.print("  gedrueckt     ");
        //       Serial.println("  gedrueckt     ");
        //       BUTTON_Flag = 0;
        //     }
        // }
      }     
    Rahmen1();  
}

// **********************************   LOOP - Ende   ***************************************************   

void Display_SW() {
  tft.setCursor(0, 0);
  tft.fillRect(0, 0, 128, 160, ST7735_BLACK);
}

void Anfangs_bild() {
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  tft.setTextSize(1);                 // text size = 1
  tft.setCursor(21, 10);              // move cursor to position (43, 10) pixel
  tft.print("ESP32 NodeMCU");
  // tft.setCursor(4, 27);        
  // tft.setTextColor(ST7735_WHITE, ST7735_BLACK);     // set text color to white and black background
  // tft.setTextSize(1);                 // text size = 1
  // tft.setCursor(19, 10);              // move cursor to position (43, 10) pixel
  // tft.print("ESP8266 NodeMCU");
  tft.setCursor(15, 30);       
  tft.print("ESP32-" + Version + "/" + Proj);    // schreibt den String  ...
  tft.setCursor(10, 50);
  tft.print("ESP_ST7735.cpp");             // von 1. Zeile und 1. Spalte ...
  tft.setCursor(20, 70);
  tft.print( "  ");      // mit Buchstabe Oben-Links
  tft.print(" - PlatformIO -");
  tft.drawRect(0, 0, 160, 128, ST7735_YELLOW);
  tft.setCursor(2,100);
  // tft.fillRect(0,0,160,128,ST7735_MAGENTA);
  // delay(5000);
}

void Rahmen1()  {  // erstellt das Menü 1
  //  tft.fillScreen(ST7735_BLACK);
   tft.drawRect(0, 0, 160, 128, ST7735_CYAN);                   // zeichnet eine aussenrahmen
   tft.setTextSize(1);
   tft.setTextColor(ST7735_BLUE, ST7735_BLACK);
   tft.setCursor(3,3);
   tft.print("Menue");
   tft.drawFastHLine(0, 11, 160, ST7735_BLUE);

}

void WiFiInit(){
 delay(10);
 Serial.print("\n\nVerbindungsaufnahme zu: ");
 Serial.print(ssid);
 WiFi.begin(ssid, passw);
 while (WiFi.status() != WL_CONNECTED){
   delay(500);
   Serial.print(".");
 }
 Serial.println("\nMit WiFi verbunden");
 Serial.print("IP Adresse: ");
 Serial.print(WiFi.localIP());
}
