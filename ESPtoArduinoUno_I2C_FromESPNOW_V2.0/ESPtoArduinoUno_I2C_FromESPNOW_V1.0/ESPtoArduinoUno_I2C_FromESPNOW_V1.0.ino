#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define I2C_SLAVE_ADDR 0x12
#define QUEUE_LENGTH  5

typedef struct message_struct {
  char a[32];
} message_struct;

// Declare the global variable here:
message_struct receivedData;

QueueHandle_t msgQueue;

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {
  // The MAC address is now in recvInfo->src_addr
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(receivedData.a);

  Serial.print("From MAC: ");
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          recvInfo->src_addr[0], recvInfo->src_addr[1], recvInfo->src_addr[2],
          recvInfo->src_addr[3], recvInfo->src_addr[4], recvInfo->src_addr[5]);
  Serial.println(macStr);
  Serial.println();

  // ***ENQUEUES DATA RECEIVED ***
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(msgQueue, &receivedData, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}



void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Wire.begin(21, 22);

  msgQueue = xQueueCreate(QUEUE_LENGTH, sizeof(message_struct));
  if (!msgQueue) {
    Serial.println("Failed to create queue!");
  }
}

void loop() {
  message_struct dataToSend;
  if (xQueueReceive(msgQueue, &dataToSend, pdMS_TO_TICKS(100))) {
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write((const uint8_t*)dataToSend.a, strlen(dataToSend.a));
    byte err = Wire.endTransmission();
    Serial.print("I2C write error: ");
    Serial.println(err);

    // Optionally read reply:
    Wire.requestFrom(I2C_SLAVE_ADDR, 32);
    while (Wire.available()) {
      char c = Wire.read();
      Serial.write(c);
    }
    Serial.println();
  }
}
