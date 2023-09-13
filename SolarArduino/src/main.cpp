#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
//Measurement Variables
const float AVCC = 4.863;
float panelAmps;
float panelVolts;
float batVolts;
float housingTemp;
float currentSolarWatts;
float previousSolarWatts;
unsigned int interruptCounter;
boolean load = false; //assume initial state of uncharged
boolean daytime = true; //make sure system knows if sleep should be turned off


//screen constants rec by adafruit
#define TRUE 1
#define FALSE 0
#define ON TRUE
#define OFF FALSE
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

//interrupt handler
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
float readSolarPanelVoltage() {
  // Read voltage using voltage divider (prolly two resistors)
  // int rawValue = analogRead(voltagePin);
  // float voltage = (rawValue * (float)analogRead(A0)) / 1024.0;
  // voltage = voltage / 2.0; // Adjust for voltage divider
  // return voltage;
}

float readCurrent() {
  // Read current using hall effect IC
  // int rawValue = analogRead(currentSensorPin);
  // float current = (rawValue * (float)analogRead(A0)) / 1024.0;
  // current = current / (0.1); // Adjust for sensor sensitivity
  // return current;
}
// MPPT Algorithm function
void mpptAlgorithm() {
  // Variables specific to the MPPT algorithm
  float maxPower = 0.0; // Maximum power achieved so far
  float optimalVoltage = 0.0; // Optimal voltage for maximum power
  float currentVoltage = 0.0; // Current measured solar panel voltage
  float currentCurrent = 0.0; // Current measured current from the sensor

  // Read solar panel voltage
  currentVoltage = readSolarPanelVoltage();

  // Read current using Hall effect sensor
  currentCurrent = readCurrent();

  // Implement MPPT algorithm here
  float power = currentVoltage * currentCurrent;
  if (power > maxPower) {
    maxPower = power;
    optimalVoltage = currentVoltage;
    // i assume this is where the load adjustment stuff with mosfets and switches comes in
  }

  // Display data on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Solar Panel MPPT");
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Voltage: ");
  display.print(currentVoltage);
  display.setCursor(0, 40);
  display.print("Current: ");
  display.print(currentCurrent);
  display.display();
}



void setup() {
  // Initialize OLED display
  int dummy; //get rid of this, need actual address
  if (!display.begin(dummy, OLED_RESET)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  Serial.begin(115200);
  //Set up serial communication
  // Serial.begin(9600);
  // Wire.begin();
  // TCA9548A(2);
  // if (!dummy.begin(0x76)) {
  //   Serial.println("Could not find a valid device on bus 2, check your wiring!");
  //   while (1);
  // }
  // Serial.println();
  
  // TCA9548A(3);
  // if (!dummy.begin(0x76)) {
  //   Serial.println("Could not find a valid device on bus 3, check your wiring!!");
  //   while (1);
  // }
  // Serial.println();
}



void loop() {
  //all this is test stuff atm
  float Vin = ((float)analogRead(A0)/1023)*5;
  Serial.println(Vin,3);
  delay(1000);

}

