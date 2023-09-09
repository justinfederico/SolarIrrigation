#include <Wire.h>
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

