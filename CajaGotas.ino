#include <OLEDDisplayUi.h>
#include <OLEDDisplayFonts.h>
#include <SSD1306Wire.h>
#include <SSD1306Spi.h>
//include <SH1106Spi.h>
#include <OLEDDisplay.h>
#include <SSD1306.h>
//#include <SSD1306Brzo.h>
#include <SSD1306I2C.h>
//#include <SH1106Wire.h>
//#include <SH1106Brzo.h>
//#include <SH1106.h>


/*
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
*/
SSD1306Wire display(0x3c, SDA, SCL); 

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
   display.init();                      // initialize the lcd 
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");
  // Print a message to the LCD.
   display.drawString(0,0,"Hello world");
   
   Serial.println(SDA);
   Serial.println(SCL);
   

}

void loop() {
  // put your main code here, to run repeatedly:

}
