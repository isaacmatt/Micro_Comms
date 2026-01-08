#include <Wire.h>

#define I2C_SLAVE_ADDR 0x12

// — globals to pass data out of the ISR —
volatile bool   gotData = false;
String         inBuf;    

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SLAVE_ADDR);    // join I2C bus as slave
  Wire.onReceive(onReceiveISR);
  Wire.onRequest(onRequestISR);
}

void loop() {
  if (gotData) {
    Serial.print("Received: ");
    Serial.println(inBuf);
    inBuf = "";
    gotData = false;
  }
  // ... you can do other work here ...
  delay(10);
}

// runs in the TWI ISR!
void onReceiveISR(int howMany) {
  // read everything into a String
  while (Wire.available()) {
    inBuf += char(Wire.read());
  }
  gotData = true;  
}

// runs in the TWI ISR!
void onRequestISR() {
  // send your reply
  const char reply[] = "Hello from Uno";
  Wire.write((const uint8_t*)reply, sizeof(reply)-1);
}
