// 2023-04-05 bpi-leaf-s3
// oled, AP, WIFI
// 260236 free heap 
//
// memory stable, site needs adjustments to display what I want.
//
// webinterface working fine now. Adding AppleMidi, BLEMIDI, DINMIDI, USB MIDI later.
//
// AppleMIDI working now. :)
// Now added if debugging around choose(listofscalenames.. in setup line 650. So it does not require pushing the button before the loop starts
//
// // normal MIDI and BLE MIDI - both working now!
//
// added function to allow debugging, if button is pressed during startup.
//

// Libraries used for WiFi and Webserver:
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <EEPROM.h>

// For Neopixel on board
#include <Adafruit_NeoPixel.h>
#define LED_PIN     48
#define LED_COUNT   1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// for OTA:
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Define webserver name
WebServer server(80);

// show serial debugging messages 
bool debugging = false;

//Variables for connection and accesspoint
int i = 0;
int statusCode;
const char* ssid = "Default SSID";
const char* passphrase = "Default passord";
String st;
String content;
String esid;
String epass = "";

// Variables for rotary encoder
#define ENCODER_CLK 18
#define ENCODER_DT 16
#define ENCODER_SW 17
int indexOfCurrentOption = 0;  // variable to keep track of current selected option

// Buzzer is connected to  
#define SPEAKER_PIN 35
bool buzz = false;

// Vars in loop
// bool play = true;
int baseTone = 60;
int frequency;
struct NoteInfo {  // Variable, holding name and frequency.
  String name;
  int frequency;
};
// int lastStateEncoderSw = HIGH;  // the previous state from the input pin
byte currentStateEncoderSw = digitalRead(ENCODER_SW);     // the current reading from the input pin
unsigned long pressedtimeEncoderSw  = 0;
unsigned long releasedtimeEncoderSw = 0;
bool isPressed             = false;
bool isLongDetectedA       = false;
const int SHORT_PRESS_time = 100;  // 1000 milliseconds
const int LONG_PRESS_time  = 5000;   // 1000 milliseconds
unsigned long pressDurationEncoderSw;
int mode     =   0;
int bpm      = 120;
float period = 250;
int channel  =   1;

// rotary encoder
// will work even without being called from loop
volatile int encoderState;
volatile int lastEncoderState;
volatile int lastEncoderDirection = -1; // Initialize to invalid direction
volatile int rotations            =  0;
volatile int rotary               =  0;

//preferences
#include <Preferences.h>
// setup for preferences:
Preferences preferences;
void setup_preferences() {
  preferences.begin("system-info", false);
  preferences.putString("serial", "V 0.56");
  preferences.putInt("boots", 0);
  preferences.end();
}

// Scales, we have 60 arrays with the names on position 0, so we build an array of the names: 
const String sc01[9] = { "Major (Ionian)", "0", "2", "4", "5", "7", "9", "11", "12" };
 const String sc02[9] = { "Nat. Minor (Aeolian)", "0", "2", "3", "5", "7", "8", "10", "12" };
 const String sc03[9] = { "Harmonic Minor", "0", "2", "3", "5", "7", "8", "11", "12" };
 const String sc04[9] = { "Melodic Minor", "0", "2", "3", "5", "7", "9", "11", "12" };
 const String sc05[9] = { "Dorian", "0", "2", "3", "5", "7", "9", "10", "12" };
 const String sc06[9] = { "Mixolydian", "0", "2", "4", "5", "7", "9", "10", "12" };
 const String sc07[9] = { "Phrygian", "0", "1", "3", "5", "7", "8", "10", "12" };
 const String sc08[8] = { "Blues", "0", "3", "5", "6", "7", "10", "12" };
 const String sc09[7] = { "Pentatonic Major", "0", "2", "4", "7", "9", "12" };
 const String sc10[7] = { "Pentatonic Minor", "0", "3", "5", "7", "10", "12" };
 const String sc11[9] = { "Locrian", "0", "1", "3", "5", "6", "8", "10", "12" };
 const String sc12[9] = { "Phrygian dominant", "0", "1", "4", "5", "7", "8", "10", "12" };
 const String sc13[9] = { "Double Harmonic", "0", "1", "4", "5", "7", "8", "11", "12" };
 const String sc14[9] = { "Hungarian Minor", "0", "2", "3", "6", "7", "8", "11", "12" };
 const String sc15[9] = { "Spanish Gypsy", "0", "1", "4", "5", "7", "8", "10", "12" };
 const String sc16[8] = { "Whole Tone", "0", "2", "4", "6", "8", "10", "12" };
 const String sc17[14] = { "Chromatic", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" };
 const String sc18[10] = { "Bebop Dominant", "0", "2", "4", "5", "7", "9", "10", "11", "12" };
 const String sc19[9] = { "Altered", "0", "1", "3", "4", "6", "8", "10", "12" };
 const String sc20[9] = { "Neapolitan Minor", "0", "1", "3", "5", "7", "8", "11", "12" };
 const String sc21[8] = { "Pentatonic Blues", "0", "3", "5", "6", "7", "10", "12" };
 const String sc22[9] = { "Lydian", "0", "2", "4", "6", "7", "9", "11", "12" };
 const String sc23[9] = { "Super Locrian", "0", "1", "3", "4", "6", "8", "10", "12" };
 const String sc24[9] = { "Enigmatic", "0", "1", "4", "6", "8", "10", "11", "12" };
 const String sc25[7] = { "Japanese Hirajoshi", "0", "2", "3", "7", "8", "12" };
 const String sc26[7] = { "Japanese Kumoi", "0", "2", "3", "7", "9", "12" };
 const String sc27[7] = { "Egyptian", "0", "2", "5", "7", "10", "12" };
 const String sc28[7] = { "Balinese", "0", "1", "3", "7", "8", "12" };
 const String sc29[9] = { "Byzantine", "0", "1", "4", "5", "7", "8", "11", "12" };
 const String sc30[9] = { "Persian", "0", "1", "4", "5", "6", "8", "11", "12" };
 const String sc31[9] = { "Phrygian #3", "0", "1", "4", "5", "7", "8", "10", "12" };
 const String sc32[8] = { "Augmented", "0", "3", "4", "7", "8", "11", "12" };
 const String sc33[11] = { "9-tone Messiaen's 3", "0", "1", "2", "4", "5", "6", "8", "9", "10", "12" };
 const String sc34[10] = { "8-tone Spanish", "0", "1", "3", "4", "5", "6", "8", "10", "12" };
 const String sc35[8] = { "Prometheus", "0", "2", "4", "6", "9", "10", "12" };
 const String sc36[10] = { "6-tone sym", "0", "1", "3", "4", "6", "7", "9", "10", "12" };
 const String sc37[9] = { "Hungarian Major", "0", "2", "3", "6", "7", "8", "11", "12" };
 const String sc38[9] = { "Ukrainian Dorian", "0", "2", "3", "6", "7", "9", "10", "12" };
 const String sc39[7] = { "In Sen", "0", "1", "5", "7", "8", "12" };
 const String sc40[10] = { "8-tone Span. Gypsy", "0", "1", "3", "4", "5", "6", "8", "9", "12" };
 const String sc41[9] = { "Dorian b2 - Phryg 6", "0", "1", "3", "5", "7", "8", "10", "12" };
 const String sc42[10] = { "8-tone Span. Phryg.", "0", "1", "3", "4", "5", "7", "8", "10", "12" };
 const String sc43[8] = { "Prometh. Neap.", "0", "1", "4", "6", "9", "10", "12" };
 const String sc44[9] = { "Bebop Dorian", "0", "2", "3", "5", "7", "9", "10", "12" };
 const String sc45[9] = { "Altered Dorian", "0", "2", "3", "5", "7", "9", "10", "12" };
 const String sc46[9] = { "Lydian dominant", "0", "2", "4", "6", "7", "8", "10", "12" };
 const String sc47[10] = { "8-tone Spanish", "0", "1", "3", "4", "5", "7", "8", "11", "12" };
 const String sc48[10] = { "Bebop major", "0", "2", "4", "5", "7", "8", "9", "11", "12" };
 const String sc49[9] = { "Oriental", "0", "1", "4", "5", "6", "8", "9", "12" };
 const String sc50[7] = { "Major pentatonic", "0", "2", "4", "7", "9", "12" };
 const String sc51[9] = { "Double harm. -Byzant", "0", "1", "4", "5", "7", "8", "11", "12" };
 const String sc52[8] = { "Whole-tone", "0", "2", "4", "6", "8", "10", "12" };
 const String sc53[8] = { "Augmented", "0", "3", "4", "7", "8", "11", "12" };
 const String sc54[9] = { "Harmonic major", "0", "2", "4", "5", "7", "8", "11", "12" };
 const String sc55[8] = { "Major blues", "0", "2", "3", "4", "7", "9", "12" };
 const String sc56[7] = { "Hirajoshi", "0", "2", "3", "7", "8", "12" };
 const String sc57[9] = { "Phrygian dominant", "0", "1", "4", "5", "7", "8", "10", "12" };
 const String sc58[9] = { "Neapolitan major", "0", "1", "3", "5", "7", "8", "11", "12" };
 const String sc59[8] = { "Blues", "0", "3", "5", "6", "7", "10", "12" };
 const String sc60[10] = { "Half-whole dimin.", "0", "1", "3", "4", "6", "7", "9", "10", "12" };
 const String* scArrays[60] = { sc01, sc02, sc03, sc04, sc05, sc06, sc07, sc08, sc09, sc10,
                               sc11, sc12, sc13, sc14, sc15, sc16, sc17, sc18, sc19, sc20,
                               sc21, sc22, sc23, sc24, sc25, sc26, sc27, sc28, sc29, sc30,
                               sc31, sc32, sc33, sc34, sc35, sc36, sc37, sc38, sc39, sc40,
                               sc41, sc42, sc43, sc44, sc45, sc46, sc47, sc48, sc49, sc50,
                               sc51, sc52, sc53, sc54, sc55, sc56, sc57, sc58, sc59, sc60 };
 // Define the number of arrays
 const int numArrays = 60;
 //
 String ListOfScaleNames[60];  ///////// need to rename this var to "NameOfcale"
 String scaleLong[60];
String scaleOled[60];
//
// 
// we have a variable numArrays that describes, how many arrays there are
// scArrays[] is an Array, that lists the names of the 60 arrays like sc01. sc02 to sc60.
// currentArray is a local variable, that is set to hold the String variable of the currently chose Array (scArray[i]) 
void prepare_scales() {
  for (int i = 0; i < numArrays; i++) {        // numArrays defined above, the total number of arrays
    const String* currentArray = scArrays[i];  // currentArray = the currently chosen array from scArrays ( - an array, that lists all scale arrays' names)
    db("scArrays", sizeof(scArrays[i]));
    db("currentArray", sizeof(currentArray));

    // Get the name of the current array
    String scname = currentArray[0];
    ListOfScaleNames[i] = scname;              // scname holds the name of the scale (like ionian) and will here be set to an Array of the real life scale names ListOfScaleNames
    db("ListOfScaleNames", sizeof(ListOfScaleNames[i]));

    // Create a vector to hold the notes
    std::vector<int> notes;
    // std::vector<int> 
    // std::vector<int> notes

    // Loop over the notes in the current array and convert them to integers
    for (int j = 1; j < 20; j++) {                  // in this loop we go through the Array to pick out the notes
      String currentNoteString = currentArray[j];   // currentNoteString holds the notes like 0,2,4,5,7,9,11,12 as a String 
      int currentNote = currentNoteString.toInt();  // and here it is made into an integer and stored in the variable currentNote
      notes.push_back(currentNote);                 // the currentNote is then set to the end of the "notes" vector
      if (currentNoteString == "12") {              // when the value 12 is detected we reach the end of the array and leave the loop
        // Found the end of the notes
        // notes.push_back(j);   ///////////////////////////////////// ok, this would write the number of elements until 12 to the end of notes
        break;
      }
    }
    //
    // Now print the name and notes to the serial monitor
    Serial.print(i + 1);
    Serial.print(". Scale Name:  ");
    Serial.println(scname);
    Serial.print("Notes: ");
    for (int j = 0; j < notes.size(); j++) {
      Serial.print(notes[j]);
      Serial.print(", ");
    }
    Serial.println();
    //
    //
    // to make it pretty we have to
    // print the additional output
    Serial.println("  0   1   2   3   4   5   6   7   8   9  10  11  12");
    scaleLong[i] += ".";
    Serial.print(" ");
    for (int j = 0; j <= 12; j++) {
      if (std::find(std::begin(notes), std::end(notes), j) != std::end(notes)) {
        Serial.print(" O  ");
        scaleOled[i] += "0";
        scaleLong[i] += ".O..";
      } else {
        Serial.print("    ");
        scaleOled[i] += " ";
        scaleLong[i] += "....";
      }
    }
    // Serial.print
    db("scaleLong", sizeof(scaleLong[i]));
    
    Serial.println();
    Serial.println();
    // delay(400);
  }
}
const int ListOfScaleNamesSize = 60;
// String selectedText;

// Below is what we need for the OLED displays
// 
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
// define the SDA and SCL pins for the two I2C ports
#define SDA_1 39
#define SCL_1 40
// #define SDA_2 41
// #define SCL_2 42
TwoWire I2C_1 = TwoWire(0);
// TwoWire I2C_2 = TwoWire(1);
// name the displays
SSD1306AsciiWire OLED1(I2C_1);
// SSD1306AsciiWire OLED2(I2C_2);
//
#define I2C_ADDRESS 0x3C
//
void setup_oled() {
  // I2C for the display(s)
  I2C_1.begin(SDA_1, SCL_1, 400000L);
  // I2C_2.begin(SDA_2, SCL_2, 400000L);
  OLED1.begin(&Adafruit128x64, I2C_ADDRESS);   ///////////////////
  OLED1.setFont(Adafruit5x7);  // what font to use?
  uint32_t m = micros();       // what time is it?
  db("m", sizeof(m));
  OLED1.clear();
  // OLED1.println("      WELCOME      ");  // some test output
  OLED1.set2X();  // twice the size
  OLED1.println("SCALES &");
  OLED1.print("MODES ");
  OLED1.set1X();
  OLED1.println(" ");
  OLED1.setCursor(80, 3);
  OLED1.print("V.11 ");  // -micros: ");
  OLED1.print(micros() - m);
  delay(1000);
  // #if RST_PIN >= 0  // "#if" means this is called during compilation
  // if desired, a second oled can be initialised:
  // OLED2.begin(&Adafruit128x32, I2C_ADDRESS, RST_PIN);
  //#else   // RST_PIN >= 0
  // OLED2.begin(&Adafruit128x32, I2C_ADDRESS);
  // #endif  // RST_PIN >= 0
  // OLED2.setFont(Adafruit5x7);
  OLED1.setCursor(0,4);
  OLED1.println(" ");
  OLED1.println("Push to proceed.");
  OLED1.println(" ");
  OLED1.print(micros() - m);
  int now = millis();
   while (digitalRead(ENCODER_SW) == HIGH) {
     // rainbowCycle(5);
      OLED1.setCursor(0, 7);
      OLED1.print("or wait " + String(5 - (millis() - now) / 1000) + " sec. ");
      // OLED1.print(5 - (millis() - now) / 1000);
      OLED1.print(millis() - m);
      if (millis() > now + 5000) {
        Serial.println("timeout reached");
      break;
      }
  }
  OLED1.setCursor(0,4);
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  
  
  
  //  delay(2000);
}

// Neopixel -  will illuminate when waiting:
void rainbowCycle_old(int wait) {
  uint8_t j, i;
  strip.setBrightness(4);
  for(j=0; j<255; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
void rainbowCycle(int wait) {
  uint8_t j, i;
  strip.setBrightness(4);
  unsigned long previousMillis = 0;
  const unsigned long interval = wait;
  
  for(j=0; j<255; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
    } else {
      delay(interval - (currentMillis - previousMillis));
    }
  }
}
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 0, 255 - WheelPos * 3);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  } else {
   WheelPos -= 170;
   return strip.Color(0, 255 - WheelPos * 3, WheelPos * 3);
  }
}
void setup_neopixel(){
  strip.begin();
  strip.show();
}

void numberOfBoots1() {
  int boots, resets;

  // Open the preferences
  preferences.begin("MIDISCALES", false);

  // Read the number of boots and resets
  boots = preferences.getUInt("boots", 0);
  resets = preferences.getUInt("resets", 0);

  // Increment the number of boots
  preferences.putUInt("boots", boots + 1);

  // Close the preferences
  preferences.end();

  // Print the values to the serial interface
  Serial.print("Number of boots: ");
  Serial.println(boots);
  Serial.print("Number of resets: ");
  Serial.println(resets);
  OLED1.setCursor(0,4);
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.setCursor(0,4);
  OLED1.println("Number of boots:");  // cosmetics..
  OLED1.println(" ");
  OLED1.set2X();
  OLED1.print(boots);
  OLED1.set1X();
  delay(1000);
}

// SETUP for OTA
void setupOTA(const char* nameprefix) {
  // Configure the hostname
  uint16_t maxlen = strlen(nameprefix) + 7;
  char *fullhostname = new char[maxlen];
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(fullhostname, maxlen, "%s-%02x%02x%02x", nameprefix, mac[3], mac[4], mac[5]);
  ArduinoOTA.setHostname(fullhostname);
  delete[] fullhostname;

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232); // Use 8266 port if you are working in Sloeber IDE, it is fixed there and not adjustable

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
	//NOTE: make .detach() here for all functions called by Ticker.h library - not to interrupt transfer process in any way.
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("\nAuth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("\nBegin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("\nConnect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("\nReceive Failed");
    else if (error == OTA_END_ERROR) Serial.println("\nEnd Failed");
  });

  ArduinoOTA.begin();

  Serial.println("OTA Initialized");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

 #if defined(ESP32_RTOS) && defined(ESP32)
  xTaskCreate(
    ota_handle,          /* Task function. */
    "OTA_HANDLE",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */
 #endif
}

// rotary encoder does not even need to be called from loop, will use interrupt
void setup_rotary(){     // here the setup
  // initialize the encoder pins
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), updateEncoder, CHANGE);
}
void updateEncoder() {   // and here the instructions to detect encoder movements that will then change values in variable "rotary"
  encoderState = digitalRead(ENCODER_DT);
  
  if (encoderState != lastEncoderState) {
    // Encoder has changed state, check direction of rotation
    int encoderDirection = (encoderState == digitalRead(ENCODER_CLK)) ? 1 : -1; // 1 for clockwise, -1 for anti-clockwise
    
    if (lastEncoderDirection == encoderDirection) {
      // Same direction as previous change, increment rotation count
      rotations++;
    } else {
      // Direction has changed, reset rotation count
      rotations = 1;
      lastEncoderDirection = encoderDirection;
    }

    // Update rotary count if two rotations have occurred in the same direction
    if (rotations == 2) {
      if (encoderDirection == 1) {
        rotary--;
      } else {
        rotary++;
      }
      
      rotations = 0;
      lastEncoderDirection = -1; // Reset direction
    }
    
    lastEncoderState = encoderState;
  }
}

// Prepare website
int webchoose = 0;       // will be given the index number of the scale chosen from the client of the website
void setup_site() {      // set up the site

  while (WiFi.status() != WL_CONNECTED) {
    // rainbowCycle(5);
    delay(1000);
    yield();
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  server.on("/", []() {
    static int j;
    static int selectedOptionIndex = 0;
    String page = "<!DOCTYPE html><html><head><title>SCALES AND MODES</title>";
    //////////////////
    page += "<style>";
		page += "body { font-family: Arial, sans-serif; line-height: 1.5; margin: 0; padding: 0; }";
		page += "header { background-color: #333; color: #fff; padding: 20px; text-align: center; }";
		page += "nav { background-color: #eee; padding: 10px; }";
		page += "nav a { display: block; padding: 5px; text-decoration: none; color: #333; }";
		page += "nav a:hover { background-color: #333; color: #fff; }";
		page += "main { margin: 20px; }";
		page += "section { padding: 20px; margin-bottom: 20px; background-color: #f9f9f9; border: 1px solid #ccc; }";
		page += "h1 { margin-top: 0; }";
		page += "/* Smooth scroll effect */";
		page += "html { scroll-behavior: smooth;}";
	  page += "</style>";    
    ///////////////////
    page += "</head><body>";
    page += "<header> <h1>SCALES & MODES</h1><br><p>Current IP: " + WiFi.localIP().toString() + "</p></header>";
    // page += "<h1>SCALES & MODES</h1>";
    // page += "";
    //

    page += "<nav>";
    for (int i = 0; i < 60; i++) {
      // page += "";
      // page += "<p><b>Scale Name: </b>" + ListOfScaleNames[i] + " <button onclick='updateWebChoose(" + String(i) + "); setup_site(); reloadAfterDelay(900)'>Choose " + String(i) + "</button></p>";
      // page += " <button onclick='updateWebChoose(" + String(i) + "); setup_site(); <a href=\"#section" + String(i) + "\"></a>reloadAfterDelay(900)'>" + String(i) + "</button>";
      page += " <button onclick='updateWebChoose(" + String(i) + "); window.location.href=\"#section" + String(i) + "\"; reloadAfterDelay(600);'>" + String(i) + "</button>";
      
    }
    page += "</nav>";
    //
    // Define the JavaScript function that will update the global variable
    page += "<script>";
    page += "function updateWebChoose(value) {";
    page += "var xhr = new XMLHttpRequest();";
    page += "xhr.open('GET', '/choose_scale?index=' + value);";
    page += "xhr.send();";
    page += "}";
    page += "</script>";
    //


    // Define the JavaScript function that will update the global variable bpm
    /*
    page += "<script>";
    page += "function updateWebBpm(value) {";
    page += "var xhr = new XMLHttpRequest();";
    page += "xhr.open('GET', '/set_bpm?wBpm=' + value);";
    page += "xhr.send();";
    page += "}";
    page += "</script>";
    */
    //
    
    // Define the JavaScript function that will reload the page after a delay
    page += "<script>";
    page += "function reloadAfterDelay(delay) {";
    page += "setTimeout(function(){ location.reload(); }, delay);";
    page += "console.log('website reload should have been triggered now');";
    page += "}";
    page += "</script>";


    page += "<script>";
    page += "function scrollToTop() { document.documentElement.scrollTop = 0; document.body.scrollTop = 0; }";
    page += "</script>";

    // page += "<p>BPM: " + String(val1) + "</p>";
    //
    // now show all scale names and scales
    page += "<main>";

    /*
    page += "<p>BPM: " + String(bpm) + "</p>";
    page += "<p><a href='/dec_bpm'><button>-</button></a> ";
    page += "<a href='/inc_bpm'><button>+</button></a> ";
    page += "<form method='get' action='/set_bpm'>";
    page += "<input type='text' name='value'>";
    page += "<input type='submit' value='Set'>";
    page += "</form></p>";
    */


    for (int i = 0; i < 60; i++) {
      ArduinoOTA.handle();
      //  "<form><input type='text' name='text_input'><button type='submit'>Submit</button></form>
      page += "<section id=\"section" + String(i) + "\">";
      page += "<p><b>Scale Name: </b>" + ListOfScaleNames[i] + " <button onclick='updateWebChoose(" + String(i) + "); window.location.href=\"#section" + String(i) + "\"; reloadAfterDelay(600);'>Choose " + String(i) + "</button>    <button style='padding-left: 30px' onclick='scrollToTop()'>Top</button></p>";
      //                                               page += " <button onclick='updateWebChoose(" + String(i) + "); window.location.href=\"#section" + String(i) + "\"; reloadAfterDelay(900);'>" + String(i) + "</button>";

      // up would compile, too, bot low is used. Need to check what was the reason
   // page += "<p><b>Scale Name: </b>" + ListOfScaleNames[i] + " <button onclick='updateWebChoose(" + String(i) + ");              reloadAfterDelay(600)'>Choose " + String(i) + "</button></p>";
      //
      //
      if (i == webchoose) {
        page += "<div style='background-color: yellow; display: inline-block; padding: 5px;border: 1px solid black;'>";
        page += "<p><pre>..0...1...2...3...4...5...6...7...8...9..10..11..12..</pre></p>";
        OLED1.setCursor(0, 0);
        OLED1.println(ListOfScaleNames[i]);
        OLED1.println("0123456789012    " + String(i));
        OLED1.println(scaleOled[i]);
        //page += "<p><pre>..|...|...|...|...|...|...|...|...|...|...|...|...|..</pre></p>";
        page += "<b>";
        page += "<p><pre>" + scaleLong[i] + "</pre></p><p></p>";
        page += "</b>";
        page += "</div>";
      } else {
        page += "<p><pre>..0...1...2...3...4...5...6...7...8...9..10..11..12..</pre></p>";
        //page += "<p><pre>..|...|...|...|...|...|...|...|...|...|...|...|...|..</pre></p>";
        page += "<b>";
        page += "<p><pre>" + scaleLong[i] + "</pre></p><p></p>";
        page += "</b>";
      }
      page += "<p><pre>  </pre></p>";
      page += "</section><br>";
    }
    page += "<br>";
    page += "<br>";
    page += "<br>";
    page += "<br>";
    page += "<br>";
    page += "<br> To delete the current WiFi Credentials push the button below: <br>";
    page += "<br>";
    page += "<button onclick='confirmWiFi()'>Delete WiFi Credentials</button>";
    page += " Please note: after confirmation the unit will relaunch and";
    page += "<br>credentials must be submitted again on 192.168.4.1 in the SCALES&MODES net.";
    page += "<script>function confirmWiFi() { if (confirm('Are you sure you want to reset WiFi credentials?')) { window.location.href = '/deleteWifiCreds'; } }</script>";
    page += "<br>";
    page += "<br>";
    page += "<br> For a firmware update open Arduino IDE, go to Tools - Port and look for IP: " + WiFi.localIP().toString() + " or use serial connection.<br>";
    page += "<p></p>";
    page += "<br>";
    /*
    page += "<button onclick='updateFirmware()'>FIRMWARE UPDATE</button>";
    page += "<script>function updateFirmware() { if (confirm('Are you sure you want to update the firmware?')) { window.location.href = '/OTA'; } }</script>";
    //
    */
    page += "<br>";
    // page += "<br> Click the button below to put the ESP32 into deep sleep: <br>";
    page += "<br>";
    // page += "<button onclick='sleep()'>DEEP SLEEP</button>";
    // page += "<script>function sleep() { if (confirm('Are you sure you want to put the ESP32 into deep sleep?')) {";
    // page += "fetch('/sleep').then(response => response.text()).then(data => console.log(data));";
    // page += "}}</script>";
    // ----------------------------
    double reading = analogRead(14);  // this makes the output of the next line a lot more accurate.
    Serial.println(reading);
    Serial.println(String((float)analogRead(14)));
    float batteryV = ((float)analogRead(14) / 4095) * 7.26;  // * 7.26
    page += "Battery Voltage is ";
    page += String(batteryV);
    page += " and free memory: ";
    page += ESP.getFreeHeap();
    page += "<p>RG 2023-02-25";
    page += "</main></body></html>";
    server.send(200, "text/html", page);
  });

  server.on("/deleteWifiCreds", []() {
    deleteWifiCreds();
    server.send(200, "text/html", "Credentials deleted - please restart.");
  });

  server.on("/choose_scale", HTTP_GET, []() {
    int scaleIndex = server.arg("index").toInt();
    webchoose = scaleIndex;
    // play = false;
    setup_site();
    OLED1.clear();
    Serial.println("new site generation triggered, scale index = " + String(webchoose));
    Serial.println("now reloading site on client");
    server.sendHeader("Location", "/", true);
    // chooseScale(scaleIndex);
    server.send(200, "text/plain", "Scale chosen: " + String(scaleIndex));
  });

  
  /*
  server.on("/set_bpm", HTTP_GET, []() {
    int webBpm = server.arg("wBpm").toInt();
    bpm = webBpm;
    // play = false;
    setup_site();
    OLED1.clear();
    Serial.println("new site generation triggered, scale index = " + String(bpm));
    Serial.println("now reloading site on client");
    server.sendHeader("Location", "/", true);
    // chooseScale(scaleIndex);
    server.send(200,"<meta http-equiv=\"refresh\" content=\0; URL=/\">");
    //server.send(200, "text/plain", "Bpm set to: " + String(webBpm));
    // free webBpm;
  });
  */
  /*
  server.on("/dec_bpm", HTTP_GET, []() {
    bpm --;
    // play = false;
    setup_site();
    OLED1.clear();
    Serial.println("new site generation triggered, dec bpm");
    Serial.println("now reloading site on client");
    server.sendHeader("Location", "/", true);
    // chooseScale(scaleIndex);
    server.send(200, "text/plain", "Bpm set to: " + String(bpm));
  });

  server.on("/inc_bpm", HTTP_GET, []() {
    bpm ++;
    // play = false;
    setup_site();
    OLED1.clear();
    Serial.println("new site generation triggered, inc bpm");
    Serial.println("now reloading site on client");
    server.sendHeader("Location", "/", true);
    // chooseScale(scaleIndex);
    server.send(200, "text/plain", "Bpm set to: " + String(bpm));
  });  

  */


  server.begin();
  Serial.println("Web server started");
}

// setup Wifi
void setup_wifi_connection() {
  Serial.println();
  Serial.println("Disconnecting current wifi connection");
  WiFi.disconnect();
  EEPROM.begin(512);  //Initialasing EEPROM
  delay(10);
  pinMode(15, INPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  //---------------------------------------- Read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  for (int i = 0; i < 32; ++i) {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  for (int i = 32; i < 96; ++i) {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
  WiFi.begin(esid.c_str(), epass.c_str());
}

// AppleMIDI
#define SerialMon Serial
#include <AppleMIDI_Debug.h>
#include <AppleMIDI.h>
// Used to monitor connection
int8_t AppleMIDIisConnected = 0;
// APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();
// First create the AppleMIDI instance
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, "SCALES&MODES", 5004);
//                           │       │      │       └──── Local port number
//                           │       │      └──────────── Name
//                           │       └─────────────────── MIDI instance name
//                           └─────────────────────────── Network socket class

// DIN-MIDI
#include <MIDI.h>
struct SerialMIDISettings : public midi::DefaultSettings {
  static const long BaudRate = 31250;
  static const int8_t RxPin = 37;
  static const int8_t TxPin = 38;
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, DINMIDI, SerialMIDISettings);
void setup_DINMIDI() {
  Serial.println("setup_MIDI debug 00");
  Serial1.begin(31250, SERIAL_8N1, 37, 38);
  // midiSerial.begin(31250); // MIDI baud rate
  // DIN_MIDI.begin(MIDI_CHANNEL_OMNI);
  DINMIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all channels  /////////////////////////  will cause guru medidation
  // Send MIDI message
  Serial.println("setup_MIDI debug 01");
  DINMIDI.sendNoteOn(60, 127, 1);  // Note On, C4, velocity 127, channel 1
  delay(500);
  DINMIDI.sendNoteOn(60, 0, 1);  // Note Off, C4, velocity 127, channel 1
  Serial.println("setup_MIDI debug 02");
}

// BLEMIDI
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
bool BLEMIDIisConnected = false;
BLEMIDI_CREATE_INSTANCE("SCALES&MODES", BLE_MIDI)
void setup_BLEMIDI() {
  BLE_MIDI.begin(MIDI_CHANNEL_OMNI);
  BLEBLE_MIDI.setHandleConnected([]() {
    BLEMIDIisConnected = true;
    //tft.setTextColor(TFT_BLUE, TFT_BLACK);           // show Connection on screen
    // tft.setCursor(160, 25);
    // tft.setTextSize(2);    //
    // tft.print("BLE ok");
    //tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //tft.drawFastHLine(160, 35, 70, TFT_GREEN);
    //tft.drawFastHLine(160, 36, 70, TFT_BLUE);
    // digitalWrite(LED_BUILTIN, HIGH);
  });
  BLEBLE_MIDI.setHandleDisconnected([]() {
    BLEMIDIisConnected = false;
    // tft.setCursor(160, 25);
    // tft.setTextSize(2);    //
    // tft.print("      ");
    // digitalWrite(LED_BUILTIN, LOW);
  });
  BLE_MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    // digitalWrite(4, LOW);
  });
  BLE_MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    // digitalWrite(4, HIGH);
  });
}

unsigned long **midiEvents;  /////////////////////////////////






// SETUP
void setup() {


  // Allocate memory for 1000 MIDI events
  midiEvents = (unsigned long **)malloc(1000 * sizeof(unsigned long *));
  for (int i = 0; i < 1000; i++) {
    midiEvents[i] = (unsigned long *)malloc(4 * sizeof(unsigned long));
  }
  /////////////////////////////////////////////



  if (currentStateEncoderSw == LOW){debugging = true;}
  Serial.begin(115200);  // Initialising if(DEBUG)Serial Monitor
  setup_rotary();
  setup_preferences();   // To write some infos in flash memory  
  setup_oled();          // The Oled displays 
  setup_neopixel();      // neopixel
  OLED1.setCursor(120, 0);
  OLED1.print(".");
  numberOfBoots1();    // function that reads and writes variables to/from flash

  // Buzzer pin:
  pinMode(SPEAKER_PIN, OUTPUT);
  
  delay(1000);
  setup_DINMIDI();
    
  OLED1.clear();       
  OLED1.println("Starting up WiFi");
  
  setup_wifi_connection();
  // to wait while booting and the IDE to detect the connection of this serial device
  for (int i = 3; i > 0; i--) {
    OLED1.setCursor(120, 0);
    OLED1.print(i);
    Serial.println(i);
    delay(1000);
  }
  OLED1.clear();
  // Are we connected?
  check_wifi_status();
  
  // Check free memory
  uint64_t chipId = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  
  // get unit ID, show it and use it as part of the name for OTA updates
  Serial.println("Prepare OTA " + String(chipId));
  char CharOfChipId[20];
  char UnitName[40];
  itoa(chipId, CharOfChipId, 10);
  strcpy(UnitName, "Scales&Modes");
  strcat(UnitName, CharOfChipId);
  OLED1.println(UnitName);
  Serial.println(UnitName);
  setupOTA(UnitName);
  // Info, get scales ready and make website ready
  OLED1.println("Prepare Scales&Modes");
  //
  if (debugging){
    OLED1.println("-- DEBUGGING IS ON --");
    preferences.begin("Time_Mem_Volts", false);
     // Read the seconds, memory and volts from last time
     int lastseconds = (preferences.getUInt("lastsec", 0));
     int lastmem = (preferences.getUInt("lastmem", 0));
     int lastvolts = (preferences.getUInt("lastvolts", 0));
     OLED1.print(lastseconds);
     // Serial.println("Last Seconds:  " + String(lastseconds));
     OLED1.print("S ");
     OLED1.print(lastmem);
     // Serial.println("Last Memory:   " + String(lastmem));
     OLED1.print("M ");
     OLED1.print(lastvolts);
     OLED1.print("V");
    preferences.end();
  }
  prepare_scales();
  setup_site();
  //
  if(debugging) { 
    Serial.println(choose(ListOfScaleNames, 60, 0)); 
  }
  
  // OLED2.clear();
  OLED1.setCursor(0,4);
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");

  MIDI.begin();
  //AppleMIDI - setup:
  AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name) {
    AppleMIDIisConnected++;
    DBG(F("Connected to session"), ssrc, name);
  });
  AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t& ssrc) {
    AppleMIDIisConnected--;
    DBG(F("Disconnected"), ssrc);
  });

  setup_BLEMIDI();
}

// some more variables used in loop
int oldbpm;
int oldScaleNumber;
int oldBaseTone;
int oldChannel;
int check[17];
const unsigned long SHORT_PRESS_TIME  =  300;
const unsigned long MEDIUM_PRESS_TIME = 1000;
const unsigned long LONG_PRESS_TIME   = 2000;
unsigned long previousMillis          =    0;

unsigned long buttonStartTime         =    0;
unsigned long buttonDuration          =    0;
unsigned long lastSent                =    0;
bool isButtonPressed      = false;
bool hasButtonBeenPressed = false;
int indexCounter   = 0;
int midiNoteNumber = 60;
int ScaleIntegersArray[] = {0,2,4,5,7,9,11,12,0,0,0,0,0}; // so we have some notes to play after startup
int arr_len = 8;
int pattern[13*4] = {0, 2, 4, 5, 7, 9,11,12,12,11,9,7,5,4,2,0};
 //                    12,14,16,17,19,21,23,24,
 //                    24,23,21,19,17,16,14,12,
 //                    12,11, 9, 7, 5, 4, 3, 0,
 //                     0, 0, 0, 0, 0, 0, 0, 0,
 //                     0, 0, 0, 0, 0, 0, 0, 0,
//                     0, 0, 0, 0};

//
bool playing = true;

// Buzzer is connected to  
#define SPEAKER_PIN 35
// pinMode(SPEAKER_PIN, OUTPUT);

void loop() {
  
  check_wifi_status();
  
  voltage();
  
  server.handleClient();
  
  ArduinoOTA.handle();
  MIDI.read();
  
  // check the encoder switch to change the modus 
  int currentButtonState = digitalRead(ENCODER_SW);
  if (currentButtonState == LOW && !isButtonPressed) {
    buttonStartTime = millis();
    isButtonPressed = true;
  } else if (currentButtonState == HIGH && isButtonPressed) {
    isButtonPressed = false;
    buttonDuration = millis() - buttonStartTime;
    
    if (buttonDuration < SHORT_PRESS_TIME) {
      // Short press
      Serial.println("This was a short press");
      if (!hasButtonBeenPressed) {
        mode = (mode + 1) % 6;
        // OLED2.clear();
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        Serial.print("Modus: ");
        Serial.println(mode);
        hasButtonBeenPressed = true;
      }
    } else if (buttonDuration < MEDIUM_PRESS_TIME) {
      // Medium press
      Serial.println("This was a medium press");
      if (!hasButtonBeenPressed) {
        mode = (mode + 1) % 6;
        // OLED2.clear();
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        Serial.print("Modus: ");
        Serial.println(mode);
        hasButtonBeenPressed = true;
      }
    } else if (buttonDuration < LONG_PRESS_TIME) {
      // Long press
      Serial.println("This was a long press");
      if (!hasButtonBeenPressed) {
        mode = (mode + 1) % 6;
        Serial.print("Modus: ");
        Serial.println(mode);
        hasButtonBeenPressed = true;
      }
    } else {
      // Very long press
      Serial.println("This was a very long press");
      OLED1.clear();
      // OLED2.clear();
      OLED1.print("Release Switch");
      delay(2000);
      resetOrSleep();
    }
  } else if (currentButtonState == HIGH) {
    hasButtonBeenPressed = false;
  } 
  
  // now switch through modes
  if (mode == 0) {            // bpm
    OLED1.setCursor(0, 4);    
     OLED1.println("Set BPM   ");
     oldbpm = bpm;
     bpm = bpm + rotary;
     rotary = 0;
     if (bpm < 1) {bpm = 1;}     
     if (bpm != oldbpm) {
       Serial.println("BPM: " + String(bpm));
       // OLED2.clear();
       OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
       playing = true;
     }
     check[12] = millis();
     OLED1.set2X();
     OLED1_x_y_val(0,6,bpm);
     OLED1.set1X();
     period = 60000 / bpm;
     check[13] = millis();
  } else if (mode == 1) {     // scale
      OLED1.setCursor(0, 4);
      indexOfCurrentOption = webchoose;
      OLED1.println("Select Scale"); // + String(indexOfCurrentOption));
      OLED1_x_y_val(90, 4, indexOfCurrentOption);


      indexOfCurrentOption = webchoose;
      // oldScaleNumber = indexOfCurrentOption; moved down  6 lines
      indexOfCurrentOption = indexOfCurrentOption + rotary;
      rotary = 0;
      if (indexOfCurrentOption > 59) {indexOfCurrentOption =  0;};
      if (indexOfCurrentOption <  0) {indexOfCurrentOption = 59;};
      if (oldScaleNumber != indexOfCurrentOption){
        oldScaleNumber = indexOfCurrentOption; // was above, moved here, now the scale can be changed via web interface
        webchoose = indexOfCurrentOption;
        playing = true;
        Serial.println("Inside loop - mode 1 - changing scale. Scale: " + String(indexOfCurrentOption));
        Serial.println(ListOfScaleNames[indexOfCurrentOption]);
        Serial.println(indexOfCurrentOption);
        // OLED2.clear();
        OLED1.setCursor(0,2);
        OLED1.println("                      ");
        OLED1.setCursor(0,2);
        OLED1.println(ListOfScaleNames[indexOfCurrentOption]);
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
      
        
        const String* currentlySelectedScale = scArrays[indexOfCurrentOption];
        
        for (int i=0; i < 14; i++) {               // clear the array
          ScaleIntegersArray[i] = 0;
          }
        indexCounter = 0;        
        Serial.print("ScaleIntegersArray: ");
        arr_len = 0;                    /// lenght of the array is found by counting the elements up to one with the value 12 in the loop below
        for (int i = 0; i < 30;  i++) {
          // we can use this loop to calculate the length of the current array
          arr_len++;
          ScaleIntegersArray[i] = currentlySelectedScale[i+1].toInt();  // currentlyselectedscsale holds the name in pos. 0, so we 
          Serial.print(" " + String(ScaleIntegersArray[i]) + ",");
          if (ScaleIntegersArray[i] == 12) {break;}                     // we yield the loop as soon as we reach 12   
        }

        
        if (debugging) {
          Serial.println(" ");
          Serial.print("ScaleIntegersArray: ");
          for (int i = 0; i <= arr_len; i++){
            Serial.print(i);
            Serial.print(": ");
            Serial.print(ScaleIntegersArray[i]);
            Serial.print(", ");
          }
          Serial.println(" ");
        }
        Serial.println(" ");        

        int size = arr_len;
        Serial.println("size: "+ String(size));
        int* tempPattern = buildArrayWithReverse(ScaleIntegersArray, size);

        for (int i = 0; i < arr_len*2; i++) {
            pattern[i] = tempPattern[i];
        }

        if (debugging) {   // show what is inside tempPattern 
          Serial.println(" ");
          Serial.print("tempPattern: ");
          for (int i = 0; i < arr_len*2; i++) {
            Serial.print(i);
            Serial.print(": ");
            Serial.print(tempPattern[i]);
            Serial.print(", ");
          }
          Serial.println(" ");
        }
        Serial.println(" ");        
        Serial.println(" ");
        Serial.println(" ");       
        Serial.println(" ");
        indexCounter = 0;
        // zeroCount = 0;
        
      }
      OLED1.setCursor(0,2);
      OLED1.println(ListOfScaleNames[indexOfCurrentOption]);
      OLED1.setCursor(0,5);
      OLED1.println(ListOfScaleNames[indexOfCurrentOption]);
      OLED1.println("0123456789012    " );
      OLED1.println(scaleOled[indexOfCurrentOption]);
  } else if (mode == 2) {     // basetone
      OLED1.setCursor(0, 4);
      OLED1.print("Set Basetone ");
      oldBaseTone = baseTone;
      baseTone = baseTone + rotary;
      rotary = 0;
      if (baseTone > 72) { baseTone = 24; }
      if (baseTone < 24) { baseTone = 72; }
      NoteInfo info = getNoteName(baseTone);
      if (oldBaseTone != baseTone){
        // OLED2.clear();
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        Serial.println("Basetone: " + info.name + " " + String(baseTone));
        playing = true;
      } 
      OLED1.setCursor(0, 6);
      OLED1.set2X();
      OLED1.print(info.name);
      OLED1.setCursor(40, 6);
      OLED1.print("..");
      OLED1_x_y_val(60, 6, baseTone); // + String(baseTone));
      OLED1.set1X();
  } else if (mode == 3) {     // channel
      OLED1.setCursor(0, 4);
      OLED1.print("Set Channel    ");
      oldChannel = channel;
      channel = channel + rotary;
      rotary = 0;
      if (channel > 16) { channel = 1; }
      if (channel < 1) { channel = 16; }
      if (oldChannel != channel){
        // OLED2.clear();
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        Serial.println("MIDI Channel: " + String(channel));
        playing = true;
      }
      OLED1.set2X();
      OLED1_x_y_val(0,6,channel);
      OLED1.set1X();
  } else if (mode == 4) {
      OLED1.setCursor(0, 4);
      OLED1.print("Sequencer      ");  
  } else {                    // Status
    OLED1.setCursor(0,4);
    OLED1.println("STATUS: ");
    OLED1.println(" ");
    OLED1.println(" ");
    OLED1.println("Push Button");
    String buzzOrNot[2] = {"Buzzer on", "Buzzer off"};
    String andNow = choose2(buzzOrNot, 2, 1); 
    Serial.println(andNow);
    if (andNow == "Buzzer on") {
      buzz = true; // handleSleep();
      mode = 0;
    } else {
      buzz = false; // delay(1000);
      mode = 0;
    // ESP.restart();
    }


  } 

  // now play the notes in the correct chronical distance
  if (millis() - lastSent > period) {              // if it's time to play the next tone
     midiNoteNumber = baseTone + pattern[indexCounter];
     playing = true;
     sendNoteToMidi(midiNoteNumber, 127, channel);
     lastSent = millis();

     NoteInfo info = getNoteName(midiNoteNumber);

     

     OLED1.set2X();  // display twice the size
     OLED1.setCursor(0, 0);
     OLED1.print(info.name + "    ");

     OLED1_x_y_val(55, 0, indexCounter);
     OLED1_x_y_val(95, 0, pattern[indexCounter]); 
     
     // OLED1.setCursor(55,0);
     // OLED1.print(" " + String(indexCounter) + " " + String(pattern[indexCounter]) + "     ");
     OLED1.set1X();

     indexCounter++;
     playing = true;

     buzzer(SPEAKER_PIN, info.frequency, period);


     if (indexCounter == 2 * arr_len) {indexCounter = 0;}
  }
  // stop playing the note 
  if (millis() - lastSent + 30 > period && playing) {         // stop playing the note   
     sendNoteToMidi(midiNoteNumber, 0, channel);
     // noBuzzer(SPEAKER_PIN);
     playing = false;
   }
}

// void output(int midiNoteNumber, int velocity, int channel) {
void sendNoteToMidi(int midiNoteNumber, int velocity, int channel) {

  Serial.println("Note " + String(midiNoteNumber) + " with velocity " + String(velocity) + " on channel " + String(channel));
  
  DINMIDI.sendNoteOn(midiNoteNumber, velocity, channel);   // hardware MIDI

  if (AppleMIDIisConnected > 0) {                             // isconnected means AppleMIDI is connected
    MIDI.sendNoteOn(midiNoteNumber, velocity, channel);  // here MIDI is finally sent
    OLED1.setCursor(110,6);
    OLED1.print("RTP");
  } else {
    OLED1.setCursor(110,6);
    OLED1.print("      ");
  }

  if (BLEMIDIisConnected) {                                 // isconnected means BLEMIDI is connected
    // BLUETOOTHMIDI.sendNoteOn(midiNoteNumber, velocity, channel);  // here MIDI is finally sent
    BLE_MIDI.sendNoteOn(midiNoteNumber, velocity, channel);
    OLED1.setCursor(110,7);
    OLED1.print("BLE");
  } else {
    OLED1.setCursor(110,7);
    OLED1.print("      ");
  }  
}

int* buildArrayWithReverse(const int* existing_array, int size) {
    int* new_arr = new int[size * 2];

    // Copy existing array to new array
    for (int i = 0; i < size; i++) {
        new_arr[i] = existing_array[i];
    }

    // Reverse second half of new array
    for (int i = size; i < size * 2; i++) {
        new_arr[i] = existing_array[(size * 2) - i - 1];
    }

    return new_arr;
}

void OLED1_x_y_val(int x, int y, int val) {
    OLED1.setCursor(x, y);
    if (val < 10) OLED1.print("  ");
    else if (val < 100) OLED1.print(" ");
    // else if (val < 1000) OLED1.print(" ");
    //else if (val < 10000) OLED1.print(" ");
    OLED1.print(val);
    OLED1.print(F(" "));    // not necessary when you format properly
}
/*
void OLED2_x_y_val(int x, int y, int val) {
    OLED2.setCursor(x, y);
    if (val < 10) OLED2.print("  ");
    else if (val < 100) OLED2.print(" ");
    // else if (val < 1000) OLED2.print("  ");
    // else if (val < 10000) OLED2.print(" ");
    OLED2.print(val);
    // OLED2.print(F(" "));    // not necessary when you format properly
}
*/

void buzzer(int pin, int frequency, int period) {
  if (buzz) {
    int now = millis();
    while (now > millis() - period/2) {
      // generate the tone
      digitalWrite(pin, HIGH);
      delayMicroseconds(frequency / 2);
      digitalWrite(pin, LOW);
      delayMicroseconds(frequency / 2);
    }
    digitalWrite(pin, LOW);
  }
}  

// here the function that calculates the Name and frequency of a midiNoteNumber
// stored in a struct NoteInfo 
NoteInfo getNoteName(int midiNoteNumber) {
  // Calculate the octave
  int octave = (midiNoteNumber / 12) - 1;
  // Calculate the note number
  int note = midiNoteNumber % 12;
  // Get the note name
  String noteName;
  int faktor;
  switch (note) {
    case 0:
      noteName = "C";
      faktor = 523.25;
      break;
    case 1:
      noteName = "C#";
      faktor = 554.37;
      break;
    case 2:
      noteName = "D";
      faktor = 587.33;
      break;
    case 3:
      noteName = "D#";
      faktor = 622.25;
      break;
    case 4:
      noteName = "E";
      faktor = 659.25;
      break;
    case 5:
      noteName = "F";
      faktor = 698.46;
      break;
    case 6:
      noteName = "F#";
      faktor = 739.99;
      break;
    case 7:
      noteName = "G";
      faktor = 783.99;
      break;
    case 8:
      noteName = "G#";
      faktor = 830.61;
      break;
    case 9:
      noteName = "A";
      faktor = 880;
      break;
    case 10:
      noteName = "A#";
      faktor = 932.33;
      break;
    case 11:
      noteName = "B";
      faktor = 987.77;
      break;
  }
  // Calculate the frequency using the provided formula
  frequency = faktor * pow(2, octave - 5);
  // Alternatively: Calculate 2 raised to a power using bit shifting
  // frequency = faktor * (1 << (octave - 5));
  // Return the note and frequency
  return { noteName + String(octave), frequency };
}

// Function that either trigger a reset or send ESP to sleep.
// Also useful for OTA updates, because it uses a short loop in "choose2"
void resetOrSleep(){
  String reset_sleep[2] = {"Reset or OTA", "Sleep"};
  String andNow = choose2(reset_sleep, 2, 0); 
  Serial.println(andNow);
  if (andNow == "Sleep") {
    handleSleep();
  } else {
    delay(1000);
    ESP.restart();
  }
}

// the following function will accept an array of Strings,
// display the strings on Oled2 while turning the encoder
// and choose the first one options[0] if the encoder Switch
// is pressed without turning
// using the first function allows to use only two arguments and
// defines "0" as default value for the second function
String choose(String options[], int numOptions) {
  return choose(options, numOptions, 0);
}
String choose(String options[], int numOptions, int defaultOption) {
  indexOfCurrentOption = defaultOption;
  // clear();
  OLED1.setCursor(0,4);
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.setCursor(0, 0);
  OLED1.println(options[indexOfCurrentOption]);
  OLED1.println(" ");  
  OLED1.println("Turn, Push = select");

  int lastEncoderState = digitalRead(ENCODER_CLK);  // variable to keep track of last encoder state
  int lastEncoderDirection = -1;                    // variable to keep track of last direction of rotation (-1 for no direction)
  int rotations = 0;                                // variable to keep track of the number of rotations

  while (true) {
    ArduinoOTA.handle();

    // check if the encoder has been rotated
    int encoderState = digitalRead(ENCODER_CLK);
    if (encoderState != lastEncoderState) {
      ArduinoOTA.handle();
      int encoderDirection = digitalRead(ENCODER_DT) != encoderState ? 1 : 0;  //1 for clockwise, 0 for anti-clockwise
      if (lastEncoderDirection == encoderDirection) {
        rotations++;
      } else {
        rotations = 0;
      }
      lastEncoderDirection = encoderDirection;
      if (rotations == 2) {
        if (encoderDirection == 1) {
          indexOfCurrentOption++;
        } else {
          indexOfCurrentOption--;
        }
        rotations = 0;
        lastEncoderDirection = -1;  // reset lastEncoderDirection to -1
        // keep the current option within the valid range
        indexOfCurrentOption = constrain(indexOfCurrentOption, 0, numOptions - 1);
        // display the current option
        Serial.println("Current option: " + options[indexOfCurrentOption]);
        // OLED2.clear();
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.setCursor(0, 0);
        OLED1.println(options[indexOfCurrentOption]);
        OLED1.println("0123456789012    " + String(indexOfCurrentOption));
        OLED1.println(scaleOled[indexOfCurrentOption]);
      }
      lastEncoderState = encoderState;
    }
    // check if the encoder switch has been pressed
    if (digitalRead(ENCODER_SW) == LOW) {
      // return the selected option
      delay(500);
      return options[indexOfCurrentOption];
    }
  }
}

String choose2(String options[], int numOptions, int defaultOption) {
  indexOfCurrentOption = defaultOption;
  // OLED2.clear();
  OLED1.setCursor(0,4);
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");
  OLED1.println("                      ");  
  OLED1.setCursor(0, 4);
  OLED1.print(options[indexOfCurrentOption]);

  int lastEncoderState = digitalRead(ENCODER_CLK);  // variable to keep track of last encoder state
  int lastEncoderDirection = -1;                    // variable to keep track of last direction of rotation (-1 for no direction)
  int rotations = 0;                                // variable to keep track of the number of rotations

  while (true) {
    ArduinoOTA.handle();   //

    int encoderState = digitalRead(ENCODER_CLK);
    if (encoderState != lastEncoderState) {
      ArduinoOTA.handle();
      int encoderDirection = digitalRead(ENCODER_DT) != encoderState ? 1 : 0;  //1 for clockwise, 0 for anti-clockwise
      if (lastEncoderDirection == encoderDirection) {
        rotations++;
      } else {
        rotations = 0;
      }
      lastEncoderDirection = encoderDirection;
      if (rotations == 2) {
        if (encoderDirection == 1) {
          indexOfCurrentOption++;
        } else {
          indexOfCurrentOption--;
        }
        rotations = 0;
        lastEncoderDirection = -1;  // reset lastEncoderDirection to -1
        // keep the current option within the valid range
        indexOfCurrentOption = constrain(indexOfCurrentOption, 0, numOptions - 1);
        // display the current option
        Serial.println("Current option: " + options[indexOfCurrentOption]);
        // OLED2.clear();
        OLED1.setCursor(0,4);
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.println("                      ");
        OLED1.setCursor(0, 4);
        OLED1.println(options[indexOfCurrentOption]);       
      }
      lastEncoderState = encoderState;
    }
    // check if the encoder switch has been pressed
    if (digitalRead(ENCODER_SW) == LOW) {
      // return the selected option
      delay(500);
      return options[indexOfCurrentOption];
    }
  }
}

// check wifi - are we connetced?
void check_wifi_status() {
  if ((WiFi.status() == WL_CONNECTED)) {
    ArduinoOTA.handle();
    static unsigned long starttime = millis();
    if (millis() - starttime >= 15000) {
      Serial.print("Connected to ");
      Serial.print(esid);
      Serial.print(" Successfully, AP-IP is ");
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      Serial.print(ipStr + " local IP is ");
      ip = WiFi.localIP();
      ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      Serial.println(ipStr);
      Serial.println(webchoose);
      freeram();
      starttime = millis();
    }

  } else {
  }

  if (testWifi() && (digitalRead(15) != 1)) {
      return;
  } else {
    Serial.println("Connection Status Negative / D15 HIGH");
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();  // Setup HotSpot
  }

  Serial.println();
  Serial.println("Waiting.");
  OLED1.clear();
  OLED1.println("Please connect to");
  OLED1.println("WiFi Network called");
  OLED1.println("'SCALES&MODES' and");
  OLED1.println("enter credentials");
  Serial.println("Please connect to");
  Serial.println("WiFi Network called");
  Serial.println("'SCALES&MODES' and");
  Serial.println("enter credentials");

  while ((WiFi.status() != WL_CONNECTED)) {
    rainbowCycle(5);
    Serial.print(".");
    // delay(100);
    server.handleClient();
  }
}

// OTA
void handleOTA() {
  String page = "<html><body>";
  page += "<h1>OTA Firmware Update</h1>";
  page += "<form method='POST' action='/update' enctype='multipart/form-data'>";
  page += "<input type='file' name='update'>";
  page += "<br><br>";
  page += "<input type='submit' value='Update'>";
  page += "</form>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

// check and display Voltage
void voltage() {
  static unsigned long starttime = millis();
  float batteryV;
  if (millis() - starttime >= 5000) {
    double reading = analogRead(14);  // this makes the output of the next line a lot more accurate.
    Serial.print("Voltage analogRead14 in # of 4096:        ");    
    Serial.println(reading);
    Serial.print("Voltage float(analogRead14) in # of 4096: ");
    Serial.println(String((float)analogRead(14)));
    batteryV = ((float)analogRead(14) / 4095) * 7.26;  // * 7.26
    // Serial.println(" A total of " + String(millis() / 1000) + " and Voltage is: " + String(batteryV));
    Serial.println("Voltage is: " + String(batteryV));
    // OLED1.clear();
    OLED1.setCursor(0, 3);
    OLED1.print(WiFi.localIP());
    OLED1.print(" ");
    OLED1.print(String(batteryV));
    db("batteryV", sizeof(batteryV));
    OLED1.print("V");
    if (debugging){OLED1.print(" Dbg");};
    starttime = millis();
    freeram();
    ArduinoOTA.handle();
    if (batteryV > 1.5 && batteryV < 3) {  //////////////////////////
      Serial.println("\nGoing to sleep now.");
      OLED1.clear();
      OLED1.println("VOLTAGE LOW");
      // later:
      OLED1.println("Going to sleep");
      handleSleep();
    }

    double DinDetectRead = analogRead(10);
    if (DinDetectRead < 2000) {
      OLED1.setCursor(110,5);
      OLED1.print("DIN");
    } else {
      OLED1.setCursor(110,5);
    OLED1.print("   ");
    }




  }
  delay(1);
}

//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change
bool testWifi(void) {
  int c = 0;
  //Serial.println("Waiting for Wifi to connect");
  while (c < 20) {
    // rainbowCycle(5);
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void launchWeb() {
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      //Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    //st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("SCALES&MODES", "");
  Serial.println("Initializing_softap_for_wifi credentials_modification");
  launchWeb();
  Serial.println("over");
}

void createWebServer() {
  {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Welcome to Wifi Credentials Update page";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i) {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i) {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.restart();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
    });
  }
}

void handleSleep() {
  OLED1.clear();
  OLED1.print("GOING TO SLEEP");
  delay(2000);
  OLED1.clear();
  OLED1.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
  String page = "<html><body>";
  page += "<h1>GOING TO SLEEP NOW</h1>";
  page += "</body></html>";
  server.send(200, "text/html", page);
  // Put the ESP32 into deep sleep
  // esp_sleep_enable_timer_wakeup(30000000); // 30 seconds
  // OLED2.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
  esp_deep_sleep_start();
  ESP.deepSleep(0);
}

// Free Heaps
void freeram() {
  if (debugging) {
    Serial.print("Free memory: ");
    Serial.println(ESP.getFreeHeap());
    OLED1.setCursor(0,2);
    OLED1.print("                      ");
    OLED1.setCursor(0,2);
    OLED1.print("Mem:");
    OLED1.print(ESP.getFreeHeap());
    OLED1.print(" ");
    OLED1.print(millis()/1000);
    Serial.println(ESP.getFreeHeap());
    Serial.println(millis()/1000);
    int batteryV = ((float)analogRead(14) / 4) * 7.26;  // * 7.26
    preferences.begin("Time_Mem_Volts", false);
     preferences.putUInt("lastsec", millis()/1000);
     preferences.putUInt("lastmem", ESP.getFreeHeap());
     preferences.putUInt("lastvolts",batteryV);
    preferences.end();    
  }
}

//debugging will display variable name and value if debugging =1
void db(String var, int val) {
  if (debugging) {
    Serial.print("Debug: ");
    Serial.println(var + " " + String(val));
  }
}

// Delete Wifi Credentials
void deleteWifiCreds() {
  Serial.println("RESET WIFI CREDENTIALS");
  Serial.println("clearing eeprom");
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM cleared");
  ESP.restart();
}
