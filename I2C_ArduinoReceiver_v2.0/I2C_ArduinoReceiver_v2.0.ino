#include <Wire.h>

#define I2C_SLAVE_ADDR 0x12
#define BUFFER_SIZE 64

// Buffer for data received from I2C (volatile: used in ISR)
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
  // --- Handle I2C Data ---
  if (i2cDataReady) {
    // Copy from volatile buffer to a local buffer
    char localBuf[BUFFER_SIZE];
    noInterrupts(); // Critical section: copy safely
    strncpy(localBuf, (const char*)i2cBuffer, BUFFER_SIZE);
    i2cDataReady = false;
    interrupts();
    Serial.print("Received over I2C: ");
    Serial.println(localBuf);
    // Now you can process this message as needed
  }

  // --- Handle Serial Input (e.g., from computer) ---
  if (Serial.available() > 0) {
    String serialInput = Serial.readStringUntil('\n');
    serialInput.trim();
    Serial.print("Received over Serial: ");
    Serial.println(serialInput);
    // Here, add any command handling, etc.
  }

  // ... You can add more code for other systems here ...
  delay(10); // Not strictly required, just keeps loop smooth
}

// ISR: Receive from I2C Master (ESP32 or other)
void onReceiveISR(int howMany) {
  i2cLen = 0;
  while (Wire.available() && i2cLen < BUFFER_SIZE - 1) {
    i2cBuffer[i2cLen++] = Wire.read();
  }
  i2cBuffer[i2cLen] = '\0'; // Null-terminate for easy string use
  i2cDataReady = true;
}

// ISR: On I2C Master request, reply with a fixed message
void onRequestISR() {
  const char reply[] = "Hello from Arduino";
  Wire.write((const uint8_t*)reply, sizeof(reply) - 1);
}
