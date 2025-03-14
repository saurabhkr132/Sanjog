// ESP NOW One way - receiver with OLED SCREEN, gets GPS Data from transmitter

#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Structure to receive GPS data
typedef struct struct_message {
  char latitude[12];
  char longitude[12];
} struct_message;

struct_message receivedData;

// ESP-NOW Receive Callback
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.println("GPS Data Received:");
  Serial.print("Latitude: ");
  Serial.println(receivedData.latitude);
  Serial.print("Longitude: ");
  Serial.println(receivedData.longitude);
  Serial.println("------------------");

  // Display data on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("GPS Coordinates:");
  display.println();
  display.print("Lat: ");
  display.println(receivedData.latitude);
  display.print("Lon: ");
  display.println(receivedData.longitude);
  display.display();
}

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // ESP-NOW runs in the background
}
