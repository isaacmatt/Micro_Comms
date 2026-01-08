#include <esp_now.h>
#include <WiFi.h>



//Structure of the data being sent to us with ESPNOW
typedef struct message_struct {
  char a[32];
} message_struct;

message_struct receivedData;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));  //copies incoming data into the receivedData variable which is a struct
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(receivedData.a);
  Serial.println();
}

void setup() {
    // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;

  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}



//
void loop() {

}
