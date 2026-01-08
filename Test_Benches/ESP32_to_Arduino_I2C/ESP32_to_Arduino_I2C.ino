#include <Wire.h>

#define I2C_SLAVE_ADDR 0x12

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA=21, SCL=22
  delay(1000);
}

void loop() {
  // Send data to Uno
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write((const uint8_t*)"Ping from ESP32", strlen("Ping from ESP32"));
  byte err = Wire.endTransmission();
  Serial.print("Write error: ");
  Serial.println(err);

  delay(100);

  // Request reply from Uno
  Wire.requestFrom(I2C_SLAVE_ADDR, 32);
  while (Wire.available()) {
    char c = Wire.read();
    Serial.write(c);
  }
  Serial.println();

  delay(2000);
}
