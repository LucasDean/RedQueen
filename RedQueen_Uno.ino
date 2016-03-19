#include <Adafruit_NeoPixel.h>  // WS2812 LED Control
#define PIN 6  // LED Data Pin
/* Parameter 1 = number of pixels in strip
   Parameter 2 = pin number (most are valid)
   Parameter 3 = pixel type flags, add together as needed:
     NEO_RGB     Pixels are wired for RGB bitstream
     NEO_GRB     Pixels are wired for GRB bitstream
     NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
     NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip) */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_RGB + NEO_KHZ800);
/* Before connecting the WS2812 to a power source, connect a big capacitor from power to ground. 
   A cap between 100µF and 1000µF should be good. Placing a small-ish resistor between your 
   Arduino’s data output and the WS2812’s data input will help protect the data pin. A 
   resistor between 220 and 470 Ω should do nicely.
   The included “WS2812_Definitions.h” file defines a huge list of standard colors. */
#include <Wire.h>
#define SLAVE_ADDRESS 0X04
// Definitions  --------------------------------------------------------------------------------------------------------
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change :
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

//----------------------------------------------------------------------------------------------------------------------
void setup() { //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  strip.begin(); // LEDs
  strip.show(); // Initialize all pixels to 'off'
  for (int j=0; j<strip.numPixels(); j++){
    strip.setPixelColor(j, strip.Color(0,20,0));    // Set fourth LED to full red, no green, full blue
    strip.show(); 
  }
  Serial.begin(9600);  // Open Serial with PC
  Serial.println("Initializing I2C");
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
  Serial.println("I2C Initialized");
}//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop() { //--------------------------------------------------------------------------------------------------------
  for(zone=0;zone<active_zones;zone++){  // Iterates through active zones
    
    do {
    pinval[zone]=analogRead(zone);  // Store analog read value for each zone
    delta = pinval[zone]-previous_pinval[zone];
    val_delta = abs(delta);
    previous_pinval[zone] = pinval[zone];
    delay(50);
    Serial.println("wating");    
    } while (val_delta > 4 );
    
    if (pinval[zone] <1020 && pinval[zone] > 261) { // Single Entry Only
      if ( pinval[zone] < 1020 && pinval[zone] >= 898 ){
        zcode[zone] = 26; } // All closed 
    
      else if ( pinval[zone] < 898 && pinval[zone] >= 699 ) {
        int entryway_index = (zone*4 - 1) + 1;
        entryway[entryway_index] = 1;
        zcode[zone] = 25; // Entryway 1 of this zone is open
        //Serial.println("Door 1");
        //delay(2000);
      }
    
      else if ( pinval[zone] < 699 && pinval[zone] >= 562 ) {
        int entryway_index = (zone*4 -1) + 2;
        entryway[entryway_index] = 1;
        zcode[zone] = 24; // Entryway 2 of this zone is open
        //Serial.println("Door 2");
        //delay(2000);  
      }
    
      else if ( pinval[zone] < 457 && pinval[zone] >= 377 ) {
        int entryway_index = (zone*4 - 1) + 3;
        entryway[entryway_index] = 1;
        zcode[zone] = 22; // Entryway 3 of this zone is open 
        //Serial.println("Door 3");
        //delay(2000);  
      }
            
      else if ( pinval[zone] < 280 && pinval[zone] >= 261 ) {
        int entryway_index = (zone*4 - 1) + 4;
        entryway[entryway_index] = 1;
        zcode[zone] = 18; // Entryway 4 of this zone is open
        //Serial.println("Door 4");
        //delay(2000);
      }  
    }
    
    else{ // More than one entry
      if ( pinval[zone] < 562 && pinval[zone] >= 457 ) {
        entryway[(zone*4 - 1) + 2] = 1;
        entryway[(zone*4 - 1) + 1] = 1;
        zcode[zone] = 23;} // Entryway 2 and 1 ;
    
      else if ( pinval[zone] < 377 && pinval[zone] >= 335 ) {
        entryway[(zone*4 - 1) + 3] = 1;
        entryway[(zone*4 - 1) + 1] = 1;
        zcode[zone] = 21;} // Entryway 3 and 1 ;
    
      else if ( pinval[zone] < 335 && pinval[zone] >= 301 ) {
        entryway[(zone*4 - 1) + 3] = 1;
        entryway[(zone*4 - 1) + 2] = 1;
        zcode[zone] = 20;} // Entryway 3 and 2;
    
      else if ( pinval[zone] < 261 && pinval[zone] >= 240 ) {
        entryway[(zone*4 - 1) + 4] = 1;
        entryway[(zone*4 - 1) + 1] = 1;
        zcode[zone] = 17;} // Entryway 4 and 1;
    
      else if ( pinval[zone] < 240 && pinval[zone] >= 222 ) {
        entryway[(zone*4 - 1) + 4] = 1;
        entryway[(zone*4 - 1) + 2] = 1;
        zcode[zone] = 16;} // Entryway 4 and 2;
    
      else if ( pinval[zone] < 202 && pinval[zone] >= 185 ) {
        entryway[(zone*4 - 1) + 3] = 1;
        entryway[(zone*4 - 1) + 4] = 1;
        zcode[zone] = 14;} // Entryway 3 and 4;
    } 
  // Compile the CODE (e.g. 101201 for a two-zone code)
//  if (zcode[zone] == 26) {
//    for (int i = 0; i < entryways; i++) {
//      strip.setPixelColor(i, strip.Color(0,20,0)); 
//      strip.show();
//      entryway[i] = 0;
//    }
//  }
//  else{
    for (int i = 0; i < entryways; i++) {
      if (entryway[i] == 1) {
        strip.setPixelColor(i, strip.Color(20,0,0)); 
        strip.show();
      }
    }    
//  }
    previous_code = code; // Update previous code before next round
    unsigned long currentMillis = millis();
  }
}//---------------------------------------------------------------------------------------------------------------------
void receiveEvent(int howMany) { //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  while (0 < Wire.available()){ // loop through all bytes
    data = Wire.read();     // receive byte as a integer
    Serial.println("Received: " and data);         // print the integer
    // Perform the command given by the Pi master based on the data received.
}}//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void requestEvent() { //------------------------------------------------------------------------------------------------
  Serial.println("Pi Requesting Data");
  Wire.write(8);  // Will write the larger CODE value after being compiled
}//---------------------------------------------------------------------------------------------------------------------
