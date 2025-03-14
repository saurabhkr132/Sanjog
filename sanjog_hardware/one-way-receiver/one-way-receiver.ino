// ESP NOW - One Way Receiver

#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a structured object
struct_message myData;

// Callback function executed when data is received
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  if (len == sizeof(myData)) {  // Ensure correct data length
    memcpy(&myData, incomingData, sizeof(myData));

    Serial.println("\nData received:");
    Serial.print("Character Value: ");
    Serial.println(myData.a);
    Serial.print("Integer Value: ");
    Serial.println(myData.b);
    Serial.print("Float Value: ");
    Serial.println(myData.c);
    Serial.print("Boolean Value: ");
    Serial.println(myData.d);
    Serial.println();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("\nData received:");
    display.print("Character Value: ");
    display.println(myData.a);
    display.print("Integer Value: ");
    display.println(myData.b);
    display.print("Float Value: ");
    display.println(myData.c);
    display.print("Boolean Value: ");
    display.println(myData.d);
    display.println();
    display.display();
    delay(2000);
    display.clearDisplay();
  } else {
    Serial.print("Received incorrect data size: ");
    Serial.println(len);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("Received incorrect data size: ");
    display.println(len);
    display.display();
    delay(2000);
    display.clearDisplay();
  }
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  delay(1000);  // Allow time for Serial Monitor to start

  // initialize the OLED object
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(F("SSD1306 allocation failed"));
    display.display();
    delay(2000);
    display.clearDisplay();
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer and setup screen
  display.clearDisplay();

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Error initializing ESP-NOW");
    display.display();
    delay(2000);
    display.clearDisplay();
    ESP.restart();  // Restart ESP32 if ESP-NOW fails
  }

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  delay(1000); // Small delay to keep serial output readable
}
