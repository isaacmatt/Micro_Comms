#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA, SCL
  delay(1000);
  Serial.println("Scanning...");
}

void loop() {
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Found I2C device at 0x%02X\n", addr);
    }
    delay(5);
  }
  Serial.println("Scan complete.\n");
  delay(5000);
}
