#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/sleep.h>

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



bool isSunny = false;
float batLevel = 11.5;
int waterLevel = 50;
enum State {
  SLEEP,
  STANDBY,
  ACTIVE
};

volatile bool wakeup = false;

void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), wakeupISR, FALLING);
}

void wakeupISR() {
  wakeup = true;
}
State currentState = STANDBY;
void loop() {
  switch (currentState) {
    case SLEEP:
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      attachInterrupt(digitalPinToInterrupt(2), wakeupISR, FALLING);
      while (!wakeup) {
        sleep_cpu();
      }
      sleep_disable();
      wakeup = false;
      currentState = STANDBY;
      // Exit sleep mode here
      break;
    case STANDBY:
      if (!isSunny || batLevel <= 12.0) {
        currentState = SLEEP;
        // Enter sleep mode here
      }
      // Standby mode code here
      break;
      case ACTIVE:
      if (!isSunny || batLevel <= 12.0 || waterLevel >= 75) {
        currentState = STANDBY;
        // Exit active mode here
      }
      // Active mode code here
      break;
  }
}