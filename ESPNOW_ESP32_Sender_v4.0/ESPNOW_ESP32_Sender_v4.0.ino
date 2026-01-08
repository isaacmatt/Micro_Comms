#include <esp_now.h>
#include <WiFi.h>

//Receiver MAC address 
//uint8_t receiverAddress[] = {0x3C,0x8A, 0x1F, 0xA3, 0x9A, 0x98};  //receiver is ESP board A 
//uint8_t receiverAddress[] = {0x3C,0x8A, 0x1F, 0xA4, 0x3E, 0x90};  //receiver is ESP board B 
//uint8_t receiverAddress[] = {0x3C,0x8A, 0x1F, 0xA2, 0x90, 0xEC};  //receiver is ESP board C 
uint8_t receiverAddress[] = {0x9C,0x9C, 0x1F, 0xD6, 0x87, 0xA0};  //receiver is ESP board D 
const int NUM_BUTTONS = 6;
const int buttonPins[NUM_BUTTONS] = {12, 13, 14, 15, 16, 17}; // Update PINS HERE these are temporary
bool lastButtonState[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}; // Active LOW


//Structure to send data with ESPNOW
typedef struct message_struct {
  char a[32];  
} message_struct;

message_struct dataToSend;

esp_now_peer_info_t peerInfo;

//callback for data after it is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void inputMessageTranslation(int input){
  switch(input){
  case 1: // Position 1 OR Cut-in Position
    strcpy(dataToSend.a, "00000001");
    break;
  case 2: // Position 2 
    strcpy(dataToSend.a, "00000010");
    break;
  case 3: // Position 3
    strcpy(dataToSend.a, "00000011");
    break;
  case 4: // Position 4
    strcpy(dataToSend.a, "00000100");
    break;
  case 5: // Position 5
    strcpy(dataToSend.a, "00000101");
    break;
  case 6: // MOUNTING Position OR Emergency Stop
    strcpy(dataToSend.a, "00000110");
    break;
  default:
    break;
 }
}

void handleButtonInput() {
    static bool lastButtonState[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
    for (int i = 0; i < NUM_BUTTONS; ++i) {
        bool currState = digitalRead(buttonPins[i]);
        if (currState == LOW && lastButtonState[i] == HIGH) {
            inputMessageTranslation(i+1); // Buttons 0-5 → cases 1-6
            espNOWSend();
            Serial.print("Button "); Serial.print(i+1); Serial.println(" pressed.");
            delay(50); // crude debounce, replace with millis logic for production
        }
        lastButtonState[i] = currState;
    }
}


void handleSerialInput() {
    if (Serial.available() > 0) {
        String inputString = Serial.readStringUntil('\n');
        inputString.trim();
        int inputInt = inputString.toInt();
        if (inputString.length() > 0 && inputInt > 0 && inputInt <= 6) {
            inputMessageTranslation(inputInt);
            espNOWSend();
        } else {
            Serial.println("Invalid input. Enter a number between 1 and 6.");
        }
    }
}


void espNOWSend(){

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &dataToSend, sizeof(dataToSend));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

void loop() {
    handleButtonInput();
    handleSerialInput();
}

