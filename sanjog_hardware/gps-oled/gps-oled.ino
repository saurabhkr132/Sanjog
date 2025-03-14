#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

// Choose two Arduino pins to use for software serial
int RXPin = 4;
int TXPin = 16;

// Default baud rate of NEO-6M is 9600
int GPSBaud = 9600;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1  // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Function to extract latitude and longitude
void extractGPSData(String nmeaSentence, String &latitude, String &longitude) {
  if (nmeaSentence.startsWith("$GPRMC")) {
    int commas[12];  // Store comma positions
    int index = 0;

    // Find comma positions in the sentence
    for (int i = 0; i < nmeaSentence.length(); i++) {
      if (nmeaSentence[i] == ',') {
        commas[index++] = i;
      }
      if (index >= 12) break;  // Stop after finding required commas
    }

    if (index >= 9) {  // Ensure we found enough fields
      // Extract latitude and longitude
      latitude = nmeaSentence.substring(commas[2] + 1, commas[3]);  // Latitude value
      latitude += " " + nmeaSentence.substring(commas[3] + 1, commas[4]);  // N/S

      longitude = nmeaSentence.substring(commas[4] + 1, commas[5]);  // Longitude value
      longitude += " " + nmeaSentence.substring(commas[5] + 1, commas[6]);  // E/W
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }

  gpsSerial.begin(GPSBaud);

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  String gpsSentence = "";
  String latitude = "N/A";
  String longitude = "N/A";

  // Read a full GPS sentence
  while (gpsSerial.available()) {
    char gpsChar = gpsSerial.read();
    Serial.write(gpsChar);  // Print to Serial Monitor

    if (gpsChar == '$') {
      gpsSentence = "";  // Start a new sentence
    }

    gpsSentence += gpsChar;

    if (gpsChar == '\n') {
      break;  // Sentence complete
    }
  }

  // Process GPS sentence
  extractGPSData(gpsSentence, latitude, longitude);

  // Display only latitude and longitude
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Latitude:");
  display.println(latitude);
  display.println();
  display.println("Longitude:");
  display.println(longitude);
  display.display();

  delay(2000);  // Wait before refreshing
}
