#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/sleep.h>


  
//Pin Definitions
const int switchPin = 3; // Define the pin for the switch
const int pumpPin = 2; // Define the pin for the pump
const int panelPin = 4; // Define the pin for the solar panel

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
boolean isSunny = true; //make sure system knows if sleep should be turned off
boolean pumpActive = false; //assume pump is off
boolean batFull = false; //assume battery is not full

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
    unsigned int x=0;
    float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
    
    for (int x = 0; x < 150; x++){ //Get 150 samples
      AcsValue = analogRead(A2);     //Read current sensor values  
      Samples = Samples + AcsValue;  //Add samples together
      delay (3); // let ADC settle before next sample 3ms
    }
    AvgAcs=Samples/150.0;//Taking Average of Samples
    
    AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.100;
    float current = AcsValueF;
    Serial.print(AcsValueF);//Print the read current on Serial monitor
    delay(50);
    return current;
}

float readBatteryVoltage() {
  // Read voltage using voltage divider (prolly two resistors)
  int rawValue = analogRead(A1);
  float voltage = (rawValue * (float)analogRead(A1)) / 1024.0;
  voltage = voltage / 2.0; // Adjust for voltage divider
  return voltage;
}

void updateDisplay(){
    static const unsigned char PROGMEM image_FaceNormal_29x14_bits[] = {0x00,0x00,0x00,0x00,0x3c,0x00,0x01,0xe0,0x7a,0x00,0x03,0xd0,0x7e,0x00,0x03,0xf0,0x7e,0x00,0x03,0xf0,0x7e,0x00,0x03,0xf0,0x3c,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x40,0x00,0x00,0x10,0x40,0x00,0x00,0x10,0x40,0x00,0x00,0x08,0x80,0x00,0x00,0x07,0x00,0x00};
    display.drawRect(1, 32, 46, 21, 0xFFFF);
    display.drawRect(46, 38, 3, 9, 0xFFFF);
    display.fillRect(3, 34, 42, 17, 0xFFFF);
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(1, 8);
    display.setTextWrap(false);
    display.print("Voltage:");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(1, 17);
    display.setTextWrap(false);
    display.print("Current:");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(1, 26);
    display.setTextWrap(false);
    display.print("Power:");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(1, 62);
    display.setTextWrap(false);
    display.print("Battery LvL");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(45, 17);
    display.setTextWrap(false);
    display.print("1");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(45, 26);
    display.setTextWrap(false);
    display.print("5");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(45, 8);
    display.setTextWrap(false);
    display.print("12");
    display.drawLine(59, 64, 59, 0, 0xFFFF);
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(74, 62);
    display.setTextWrap(false);
    display.print("Water LvL");
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    display.setCursor(64, 10);
    display.setTextWrap(false);
    display.print("System:");
    display.drawBitmap( 99, 0, image_FaceNormal_29x14_bits, 29, 14, 0xFFFF);

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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Start the OLED display
  display.setTextColor(WHITE);
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP); // Set up the switch pin with a pull-up resistor
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);

  delay(2000);         // wait for initializing
  display.clearDisplay(); // clear display
  display.setTextSize(1);          // text size
  display.setTextColor(WHITE);     // text color
  display.setCursor(0, 10);        // position to display
  display.println("SIPS"); // text to display
  display.display(); 


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
      }else if (batLevel <= 13.4)
      {
        digitalWrite(2,  LOW); //turn on pv to charge bat
      }
      break;
  
}
}
