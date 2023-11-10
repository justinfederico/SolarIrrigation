#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/sleep.h>
#include <U8g2lib.h>

  
//Pin Definitions
const int switchPin = 3; // Define the pin for the switch
const int pumpPin = 2; // Define the pin for the pump
const int panelPin = 4; // Define the pin for the solar panel

//Measurement Variables
const float AVCC = 4.863;
int trig = 12;
int echo = 11;
float panelAmps;
float panelVolts;
float batVolts;
float housingTemp;
float currentSolarWatts;
float previousSolarWatts;
const float lowBat = 12.0;
const float fullBatt = 13.6;
unsigned int interruptCounter;
boolean load = false; //assume initial state of uncharged
boolean isSunny = true; //make sure system knows if sleep should be turned off
boolean pumpActive = false; //assume pump is off
boolean batFull = false; //assume battery is not full
int progress = 0; // progress of the progressbar

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
  float voltage = (rawValue * (float)analogRead(A3)) / 1024.0;
  voltage = voltage / 2.0; // Adjust for voltage divider
  return voltage;
}

float readCurrent() {
    float AcsValue = 0.0, Samples = 0.0, AvgAcs = 0.0, AcsValueF = 0.0;
 
  for (int x = 0; x < 150; x++) { //Get 150 samples
    AcsValue = analogRead(A0);     //Read current sensor values
    Samples = Samples + AcsValue;  //Add samples together
    delay (3); // let ADC settle before following sample 3ms
  }
  AvgAcs = Samples / 150.0; //Taking Average of Samples
  AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.100; //scaled for 20A current sensor
 
  Serial.print(AcsValueF);//Print the read current on Serial monitor
  return AcsValueF;
  delay(50);
}
int readWaterLevel(){
  long t = 0, h = 0, hp = 0;
  
  // Transmitting pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Waiting for pulse
  t = pulseIn(echo, HIGH);
  
  // Calculating distance 
  h = t / 58;
 
  h = h - 6;  // offset correction
  h = 50 - h;  // water height, 0 - 50 cm
  
  hp = 2 * h;  // distance in %, 0-100 %
  
  // Sending to computer
  Serial.print(hp);
  // Serial.print(" cm\n");
  Serial.print("\n");
  return hp;
  delay(1000);
}
float readBatteryVoltage() {
  // Read voltage using voltage divider (prolly two resistors)
  int rawValue = analogRead(A1);
  float voltage = (rawValue * (float)analogRead(A1)) / 1024.0;
  voltage = voltage / 2.0; // Adjust for voltage divider
  return voltage;
}

void updateDisplay(){
    u8g2.clearBuffer();					// clear the internal memory
    // code from https://lopaka.app/
    float PanelVolts = readSolarPanelVoltage();
    float PanelAmps = readCurrent();
    float PanelWatts = PanelVolts * PanelAmps;
    u8g2.setBitmapMode(1);
    u8g2.drawFrame(1, 32, 46, 21);
    u8g2.drawFrame(46, 38, 3, 9);
    u8g2.drawBox(3, 34, progress, 17);
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
    u8g2.print(PanelAmps);
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setCursor(40, 26);
    u8g2.print(PanelWatts);
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setCursor(40, 8);
    u8g2.print(PanelVolts);
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
    
    // increase the progress value to go over 0-100
    progress = progress + 1;
    if (progress > 42) {
      progress = 0;
    }

}
float batLevel = 11.5;
int waterLevel = 50;
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
  pinMode(2, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP); // Set up the switch pin with a pull-up resistor
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);

  delay(2000);         // wait for initializing
}

State currentState = STANDBY;
unsigned long lastVoltageReadTime = 0;
unsigned long lastCurrentReadTime = 0;
const unsigned long voltageReadInterval = 1000;  // 1 second
const unsigned long currentReadInterval = 2000;  // 2 seconds



/////////////////////////MAIN LOOP/////////////////////////
void loop() {
  int switchState = digitalRead(switchPin);

  // Check the state of the switch
  if (switchState == LOW) { // Switch is pressed (LOW)
    currentState = SLEEP;
  } else {
    currentState = STANDBY;
  }

  unsigned long currentTime = millis();

  // Read solar panel voltage every 1 second
  if (currentTime - lastVoltageReadTime >= voltageReadInterval) {
    panelVolts = readSolarPanelVoltage();
    lastVoltageReadTime = currentTime;
  }

  // Read current every 2 seconds
  if (currentTime - lastCurrentReadTime >= currentReadInterval) {
    panelAmps = readCurrent();
    lastCurrentReadTime = currentTime;
  }
  readWaterLevel();
  switch (currentState) {
    case SLEEP:
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      attachInterrupt(digitalPinToInterrupt(2), wakeupISR, FALLING);
      while (!wakeup && !isSunny) { // Exit sleep mode when isSunny becomes true
        sleep_cpu();
      }
      sleep_disable();
      wakeup = false;
      currentState = STANDBY;
      // Exit sleep mode here
    break;
    case STANDBY:
      if (!isSunny || batLevel <= 12.0) {
        digitalWrite(3,  HIGH); //turn off the pump
        currentState = SLEEP;
        // Enter sleep mode here
      }
      // Standby mode code here

      if (batLevel >= 13.4) {
          digitalWrite(2,  HIGH); //turn off pv to let bat discharge
        }else if (batLevel <= 13.4)
        {
          digitalWrite(2,  LOW); //turn on pv to charge bat
          updateDisplay();
        }
      break;
    case ACTIVE:
      if (isSunny || (batLevel <= 12.0 || waterLevel >= 75)) {
        currentState = STANDBY;
        // Exit active mode here
      }
      // Active mode code here
      digitalWrite(3,  LOW); //turn on the pump
      if (batLevel >= 13.4) {
        digitalWrite(2,  HIGH); //turn off pv to let bat discharge
        updateDisplay();
      }else if (batLevel <= 13.4)
      {
        digitalWrite(2,  LOW); //turn on pv to charge bat
        updateDisplay();
      }
      break;
  
}
}
