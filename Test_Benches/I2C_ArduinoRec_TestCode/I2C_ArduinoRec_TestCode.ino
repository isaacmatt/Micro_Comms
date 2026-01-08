#include <Wire.h>
#define I2C_SLAVE_ADDR 0x12
#define BUFFER_SIZE 64

volatile char i2cBuffer[BUFFER_SIZE];
volatile uint8_t i2cLen = 0;
volatile bool i2cDataReady = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(onReceiveISR);
  Wire.onRequest(onRequestISR);
}

void loop() {
  if (i2cDataReady) {
    char localBuf[BUFFER_SIZE];
    noInterrupts();
    strncpy(localBuf, (const char*)i2cBuffer, BUFFER_SIZE);
    i2cDataReady = false;
    interrupts();
    Serial.print("Received over I2C: ");
    Serial.println(localBuf);
  }
}

void onReceiveISR(int howMany) {
  i2cLen = 0;
  while (Wire.available() && i2cLen < BUFFER_SIZE - 1) {
    i2cBuffer[i2cLen++] = Wire.read();
  }
  i2cBuffer[i2cLen] = '\0';
  i2cDataReady = true;
}

void onRequestISR() {
  const char reply[] = "Hello from Arduino";
  Wire.write((const uint8_t*)reply, sizeof(reply) - 1);
}
