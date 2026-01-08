#include <Wire.h>

void setup() {
  Wire.begin();           // Join I2C bus as master
  Serial.begin(9600);     // Open serial monitor
  delay(1000);

  Serial.println("I2C Scanner Starting...");
}

void loop() {
  byte error, address;
  int devices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found I2C device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      devices++;
    }
    delay(5);
  }

  if (devices == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.println("Scan complete.");
  }

  delay(3000);  // Wait before re-scanning
}
