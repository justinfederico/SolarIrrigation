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
float readSolarPanelVoltage() {
  // Read voltage using voltage divider (prolly two resistors)
  int rawValue = analogRead(A0);
  float voltage = (rawValue * (float)analogRead(A0)) / 1024.0;
  voltage = voltage / 2.0; // Adjust for voltage divider
  return voltage;
}

float readCurrent() {
 int current;
 return current;
}
bool isSunny = false;
float batLevel = 11.5;
int waterLevel = 50;
enum State {
  SLEEP,
  STANDBY,
  ACTIVE
};

void wakeupISR() {
  wakeup = true;
}

volatile bool wakeup = false;

void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);
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
      if (isSunny || (batLevel <= 12.0 || waterLevel >= 75)) {
        currentState = STANDBY;
        // Exit active mode here
      }
      // Active mode code here
      break;
  }
}