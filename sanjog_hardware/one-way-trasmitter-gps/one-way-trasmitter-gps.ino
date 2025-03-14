// ESP NOW One way Transmitter, send GPS data

#include <SoftwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>

// GPS Module Pins
int RXPin = 4;
int TXPin = 16;
int GPSBaud = 9600;  // Default GPS baud rate
SoftwareSerial gpsSerial(RXPin, TXPin);

// Structure to send GPS data
typedef struct struct_message {
  char latitude[12];
  char longitude[12];
} struct_message;

struct_message gpsData;

// ESP-NOW Peer MAC Address (Receiver ESP32 MAC Address)
uint8_t receiverMAC[] = {0x5C, 0x01, 0x3B, 0x4B, 0xBA, 0xB4};

esp_now_peer_info_t peerInfo;

// Function to extract latitude and longitude from $GPRMC
void extractGPSData(String nmeaSentence) {
  if (nmeaSentence.startsWith("$GPRMC")) {
    int commas[12], index = 0;
    for (int i = 0; i < nmeaSentence.length(); i++) {
      if (nmeaSentence[i] == ',') {
        commas[index++] = i;
      }
      if (index >= 12) break;
    }

    if (index >= 9) {
      String lat = nmeaSentence.substring(commas[2] + 1, commas[3]);
      lat += nmeaSentence.substring(commas[3] + 1, commas[4]);
      String lon = nmeaSentence.substring(commas[4] + 1, commas[5]);
      lon += nmeaSentence.substring(commas[5] + 1, commas[6]);

      lat.toCharArray(gpsData.latitude, sizeof(gpsData.latitude));
      lon.toCharArray(gpsData.longitude, sizeof(gpsData.longitude));
    }
  }
}

// ESP-NOW Send Callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("ESP-NOW Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud);

  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  String gpsSentence = "";

  while (gpsSerial.available()) {
    char gpsChar = gpsSerial.read();
    Serial.write(gpsChar);
    if (gpsChar == '$') gpsSentence = "";
    gpsSentence += gpsChar;
    if (gpsChar == '\n') break;
  }

  extractGPSData(gpsSentence);

  // Send GPS data over ESP-NOW
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&gpsData, sizeof(gpsData));
  Serial.println(result == ESP_OK ? "Sent Successfully" : "Send Failed");

  delay(2000);
}
