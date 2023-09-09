
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_DC     8
#define OLED_CS     10
#define OLED_RESET  9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);


#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

volatile bool interruptOccurred = false;
void handleInterrupt() {
  interruptOccurred = true;
}
// Select I2C BUS
void TCA9548A(uint8_t bus){
 
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  Serial.print(bus);
}
void setup() {
  // Set up serial communication
//   Serial.begin(9600);
//   Wire.begin();
//   TCA9548A(2);
//   if (!joint1Encoder.begin(0x76)) {
//     Serial.println("Could not find a valid device on bus 2, check your wiring!");
//     while (1);
//   }
//   Serial.println();
  
//   TCA9548A(3);
//   if (!joint2Encoder.begin(0x76)) {
//     Serial.println("Could not find a valid device on bus 3, check your wiring!!");
//     while (1);
//   }
//   Serial.println();
// }
}

void loop() {
  // put your main code here, to run repeatedly:

}

