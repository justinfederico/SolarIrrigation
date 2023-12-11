#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <U8g2lib.h>

  
//Pin Definitions
const int switchPin = 9; // Define the pin for the switch
//const int pumpPin = 2; // Define the pin for the pump
const int panelPin = 4; // Define the pin for the solar panel

//Measurement Variables
float batLevel;
float temp;
int trig = 12;
int echo = 11;
float panelAmps;
float panelVolts;
float batVolts;
float panelWatts;
const float lowBat = 12.0;
const float fullBatt = 13.6;
unsigned int interruptCounter;
boolean isSunny; //make sure system knows if sleep should be turned off
int progress = 0; // progress of the progressbar
float R1 = 10000.0;
float R2 = 3000.0;
float R3 = 22300.0;
float R4 = 10000.0;

//screen constants rec by adafruit
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // initialization for the used OLED display
static const unsigned char image_FaceNormal_29x14_bits[] U8X8_PROGMEM = {0x00,0x00,0x00,0x00,0x3c,0x00,0x80,0x07,0x5e,0x00,0xc0,0x0b,0x7e,0x00,0xc0,0x0f,0x7e,0x00,0xc0,0x0f,0x7e,0x00,0xc0,0x0f,0x3c,0x00,0x80,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x10,0x01,0x00,0x00,0xe0,0x00,0x00};

//interrupt handler
volatile bool interruptOccurred = false;

void handleInterrupt() {
  interruptOccurred = true;
}

float readSolarPanelVoltage() {
  // Read voltage using voltage divider (prolly two resistors)
  int rawValue = analogRead(A3);
  float ADCvoltage = (rawValue * 5.0)/1023;
  float voltage = ADCvoltage / (R2/(R1+R2));
  Serial.print(F("Solar Panel Voltage: "));
  Serial.print(voltage);
  Serial.print("\n");

  return voltage;
}

float readCurrent() {
    float AcsValue = 0.0, Samples = 0.0, AvgAcs = 0.0, AcsValueF = 0.0;
 
  // for (int x = 0; x < 150; x++) { //Get 150 samples
  //   AcsValue = analogRead(A0);     //Read current sensor values
  //   Samples = Samples + AcsValue;  //Add samples together
  //   delay (3); // let ADC settle before following sample 3ms
  // }
  // AvgAcs = Samples / 150.0; //Taking Average of Samples
  // AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.100; //scaled for 20A current sensor
  // Serial.print("Current: ");
  // Serial.print(AcsValueF);//Print the read current on Serial monitor
  // Serial.print("\n");
  AcsValueF = 6.0;
  return AcsValueF;
  delay(50);
}

float readBatteryVoltage() {
  // Read voltage using voltage divider (prolly two resistors)
  int rawBatValue = analogRead(A2);
  float batVoltage = (rawBatValue * 5.0)/1023;
  float final = batVoltage / (R4/(R3+R4));
  Serial.print(F("Battery Voltage: "));
  Serial.print(final);
  Serial.print("\n");
}

void updateDisplay(){
    u8g2.clearBuffer();					// clear the internal memory
    //lopaka u8g2 screen code
    batVolts = readBatteryVoltage();
    panelVolts = readSolarPanelVoltage();
    progress = map(panelVolts, 0, 21, 0, 100); 
    panelAmps = readCurrent();
    panelWatts = panelVolts * panelAmps;
    u8g2.setBitmapMode(1);
    u8g2.drawFrame(1, 32, 46, 21);
    u8g2.drawFrame(46, 38, 3, 9);
    u8g2.drawBox(3, 34, map(progress, 0, 100, 0, 42), 17);
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(1, 8, "Voltage:");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(1, 17, "Current:");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(1, 26, "Power:");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(1, 62, "Battery LvL");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setCursor(40, 17);
    u8g2.print(panelAmps);
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setCursor(40, 26);
    u8g2.print(panelWatts);
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setCursor(40, 8);
    u8g2.print(panelVolts);
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(74, 62, "Water LvL");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(70, 8, "Volts");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(70, 17, "Amps");
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(70, 26, "Watts");
    u8g2.drawXBMP( 99, 0, 29, 14, image_FaceNormal_29x14_bits);
    u8g2.sendBuffer();					// transfer internal memory to the display

}
int waterLevel = 90;
enum State {
  SLEEP,
  STANDBY,
  ACTIVE
};

volatile bool wakeup = false;

void wakeupISR() {
  wakeup = true;
}

char ch=0; //global variable for Serial.read()

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); 
  u8g2.begin(); // start the u8g2 library
  Serial.begin(9600);
  pinMode(9, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(9), handleInterrupt, FALLING);

  delay(2000);         // wait for initializing
}

State currentState = STANDBY;
// unsigned long lastVoltageReadTime = 0;
// unsigned long lastCurrentReadTime = 0;
// const unsigned long voltageReadInterval = 1000;  // 1 second
// const unsigned long currentReadInterval = 2000;  // 2 seconds



/////////////////////////MAIN LOOP/////////////////////////
void loop() {
  delay(400);
  batLevel = readBatteryVoltage();
  temp = readSolarPanelVoltage();

  if (temp > 2.0) {
    isSunny = true;
  } else {
    isSunny = false;
  }

  Serial.println(currentState);
  digitalWrite(2, HIGH); // turn on pv to charge bat
  switch (currentState) {
    case SLEEP:
      // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      // sleep_enable();
      // attachInterrupt(digitalPinToInterrupt(9), wakeupISR, FALLING);

      // while (!wakeup && !isSunny) {
      //   sleep_cpu();
      //   digitalWrite(LED_BUILTIN, HIGH);
      //   delay(2000);
      //   digitalWrite(LED_BUILTIN, LOW);
      //   delay(1000);
      // }

      // sleep_disable();
      // wakeup = false;
      currentState = STANDBY;
      break;

    case STANDBY:
      if (isSunny==0 || batLevel <= 12.0) {
        digitalWrite(3, LOW); // turn off the pump
        currentState = SLEEP;
      } else if (isSunny==1 && batLevel >= 13.0 && waterLevel <= 100) {
        currentState = ACTIVE;
      }
      // Standby mode code here
      updateDisplay();
      break;

    case ACTIVE:
      if (isSunny && (batLevel <= 12.0 || waterLevel >= 80)) {
        currentState = STANDBY;
      }
      // Active mode code here
      digitalWrite(3, HIGH); // turn on the pump
      if (batLevel >= 13.4) {
        digitalWrite(2, LOW); // turn off pv to let bat discharge
      } else if (batLevel <= 13.4) {
        digitalWrite(2, HIGH); // turn on pv to charge bat
      }
      updateDisplay();
      break;
  }
}

