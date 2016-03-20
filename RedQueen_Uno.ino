#include <Adafruit_NeoPixel.h>  // WS2812 LED Control
#define PIN 6  // LED Data Pin
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_RGB + NEO_KHZ800);

#include <Wire.h>
#define SLAVE_ADDRESS 0X04

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000;           // interval at which to blink (milliseconds)
int data = 0;  // Store integer commands sent from Pi
int pinval[6];  // Array to store 6 analogRead values
int previous_pinval[] = {0, 0, 0, 0, 0, 0}; //Store previous pin val
int zcode[6];  // Array to store zone codes
int zone = 0;  // Zone iterative counter
int active_zones = 1; // Total zones active using low pin #s first
int code = 8;  // Dummy value for code until codeLookup complete
int previous_code = 0;
int entryway[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int entryways = active_zones * 4;
int delta = 5;
int val_delta = 0;
int reading_average = 0;
//String mode = "Active";
String mode = "Passive";
int brightness = 20;

void setup() {
  strip.begin(); // LEDs
  strip.show(); // Initialize all pixels to 'off'
  for (int j=0; j<strip.numPixels(); j++){
    strip.setPixelColor(j, strip.Color(0,brightness,0));    // Set fourth LED to full red, no green, full blue
    strip.show(); 
  }
//  Serial.begin(9600);  // Open Serial with PC
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
}

void loop() {
  for(zone=0;zone<active_zones;zone++){                                           // For zone 0
    do {
      int reading_1 = analogRead(zone);
      int reading_2 = analogRead(zone);
      int reading_3 = analogRead(zone);
      int reading_4 = analogRead(zone);
      int reading_5 = analogRead(zone);
      reading_average = (reading_1+reading_2+reading_3+reading_4+reading_5)/5;    // average five readings of pin 0
//      Serial.println(reading_average);
      pinval[zone]=reading_average;                                               // pinval[0] = average value
      delta = pinval[zone]-previous_pinval[zone];                                 // difference between old pinval[0] and new pinval[0]
      val_delta = abs(delta);                                                     // absolute value of this difference
      previous_pinval[zone] = pinval[zone];                                       // define new old pinval[0] value
    } while (val_delta > 4 );                                                     // keep looping until two consecutive averages are less than or equal to 4
    int index = zone * 4 - 1;
    if ( pinval[zone] < 1020 && pinval[zone] >= 898 ){                       // READING: 898 - 1020
      if (mode == "Passive") {                                                // CODE: 26  
        entryway[index+1] = 0;                                               // OPEN: NONE
        entryway[index+2] = 0;                                               
        entryway[index+3] = 0;
        entryway[index+4] = 0; }
      zcode[zone] = 26; }                                                           
    
    else if ( pinval[zone] < 898 && pinval[zone] >= 699 ) {                  // READING: 699 - 898
      entryway[index+1] = 1;                                                 // CODE: 25
      if (mode == "Passive") {                                                // OPEN: 1
        entryway[index+2] = 0;
        entryway[index+3] = 0;
        entryway[index+4] = 0; }
      zcode[zone] = 25; }

    else if ( pinval[zone] < 699 && pinval[zone] >= 562 ) {                  // READING: 562 - 699                        
      entryway[index+2] = 1;                                                 // CODE: 24
      if (mode == "Passive") {                                                // OPEN: 2
        entryway[index+1] = 0;
        entryway[index+3] = 0;
        entryway[index+4] = 0; }
      zcode[zone] = 24; }
    
    else if ( pinval[zone] < 457 && pinval[zone] >= 377 ) {                  // READING: 377 - 457
      entryway[index+3] = 1;                                                 // CODE: 22
      if (mode == "Passive") {                                                // OPEN: 3
        entryway[index+1] = 0;
        entryway[index+2] = 0;
        entryway[index+4] = 0; }                                             
      zcode[zone] = 22; }  
    
    else if ( pinval[zone] < 280 && pinval[zone] >= 261 ) {                  // READING: 261 - 280
      entryway[index+4] = 1;                                                 // CODE: 18
      if (mode == "Passive") {                                                // OPEN: 4
        entryway[index+1] = 0;                                              
        entryway[index+2] = 0;
        entryway[index+3] = 0; }  
      zcode[zone] = 18; } 
   
    else if ( pinval[zone] < 562 && pinval[zone] >= 457 ) {                  // READING: 457 - 562
      entryway[index+2] = 1;                                                 // CODE: 23
      entryway[index+1] = 1;                                                 // OPEN: 1 and 2
      if (mode == "Passive") {
        entryway[index+3] = 0;
        entryway[index+4] = 0; }
      zcode[zone] = 23; }
    
    else if ( pinval[zone] < 377 && pinval[zone] >= 335 ) {                  // READING: 335 - 377
      entryway[index+1] = 1;                                                 // CODE: 21
      entryway[index+3] = 1;                                                 // OPEN: 1 and 3
      if (mode == "Passive") {
        entryway[index+2] = 0;
        entryway[index+4] = 0; }
      zcode[zone] = 21; }
    
    else if ( pinval[zone] < 335 && pinval[zone] >= 301 ) {                  // READING: 301 - 335
      entryway[index+2] = 1;                                                 // CODE: 20
      entryway[index+3] = 1;                                                 // OPEN: 2 and 3
      if (mode == "Passive") {
        entryway[index+1] = 0;
        entryway[index+4] = 0; }
      zcode[zone] = 20; }
   
    else if ( pinval[zone] < 261 && pinval[zone] >= 240 ) {                  // READING: 240 - 261
      entryway[index+1] = 1;                                                 // CODE: 17
      entryway[index+4] = 1;                                                 // OPEN: 1 and 4
      if (mode == "Passive") {
        entryway[index+2] = 0;
        entryway[index+3] = 0; }
      zcode[zone] = 17; }
    
    else if ( pinval[zone] < 240 && pinval[zone] >= 222 ) {                  // READING: 222 - 240
      entryway[index+2] = 1;                                                 // CODE: 16
      entryway[index+4] = 1;                                                 // OPEN: 2 and 4
      if (mode == "Passive") {
        entryway[index+1] = 0;
        entryway[index+3] = 0; }
      zcode[zone] = 16; }
    
    else if ( pinval[zone] < 202 && pinval[zone] >= 185 ) {                  // READING: 185 - 202
      entryway[index+2] = 1;                                                 // CODE: 14
      entryway[index+4] = 1;                                                 // OPEN: 3 and 4
      if (mode == "Passive") {
        entryway[index+1] = 0;
        entryway[index+3] = 0; }
      zcode[zone] = 14; }

    else if ( pinval[zone] < 301 && pinval[zone] >= 280 ) {                   // READING: 280 - 301
      entryway[index+1] = 1;                                                  // CODE: 19
      entryway[index+2] = 1;                                                  // OPEN: 1, 2 and 3
      entryway[index+3] = 1;
      if (mode == "Passive") {
        entryway[index+4] = 0; }
      zcode[zone] = 19; }

    else if ( pinval[zone] < 222 && pinval[zone] >= 202 ) {                   // READING: 202 - 222
      entryway[index+1] = 1;                                                  // CODE: 15
      entryway[index+2] = 1;                                                  // OPEN: 1, 2 and 4
      entryway[index+4] = 1;
      if (mode == "Passive") {
        entryway[index+3] = 0; }
      zcode[zone] = 15; }

    else if ( pinval[zone] < 174 && pinval[zone] >= 167 ) {                   // READING: 164 - 176 ***
      entryway[index+2] = 1;                                                  // CODE: 12
      entryway[index+3] = 1;                                                  // OPEN: 2, 3 and 4
      entryway[index+4] = 1;
      if (mode == "Passive"){
        entryway[index+1] = 0; }
      zcode[zone] = 12;}

    else if ( pinval[zone] < 185 && pinval[zone] >= 176 ) {                   // READING: 176 - 185
      entryway[index+1] = 1;                                                  // CODE: 13
      entryway[index+3] = 1;                                                  // OPEN: 1, 3 and 4
      entryway[index+4] = 1;
      if (mode == "Passive"){
        entryway[index+2] = 0; }
      zcode[zone] = 13;}

    else {                                                                    // READING: 150 - 164
      entryway[index+1] = 1;                                                  // CODE: 11
      entryway[index+2] = 1;                                                  // OPEN: ALL
      entryway[index+3] = 1;
      entryway[index+4] = 1;
      zcode[zone] = 11;}
    
    for (int i = 0; i < entryways; i++) {                               // After checking all the zones, update all Leds
      if (entryway[i] == 1) {
        strip.setPixelColor(i, strip.Color(brightness,0,0)); 
        strip.show();
      }
      if (entryway[i] == 0) {
        strip.setPixelColor(i, strip.Color(0,brightness,0)); 
        strip.show();
      }
    }    
  }
}
void receiveEvent(int howMany) { 
  while (0 < Wire.available()) {
    data = Wire.read();
    Serial.println("Received: " and data);
  }
}
void requestEvent() {
  Wire.write(zcode[zone]);
}
