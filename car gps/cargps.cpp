#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// GPS i Serial
TinyGPSPlus gps;
HardwareSerial mySerial(1);

// SD i przycisk
#define SD_CS 5
#define BUTTON_PIN 4
File gpxFile;

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Zmienne stanu
bool isLogging = false;
bool gpxClosed = false;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
String filename = "";

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // SD karta
  SPI.begin(15, 19, 23, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("❌ Błąd: nie można zainicjalizować karty SD.");
    while (true);
  }
  Serial.println("✅ Karta SD OK.");

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ Błąd: OLED nie działa.");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED gotowy.");
  display.display();
  delay(1000);
}

void loop() {
  while (mySerial.available() > 0) {
    gps.encode(mySerial.read());
  }

  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (!isLogging) {
          startGpx();
          isLogging = true;
        } else if (!gpxClosed) {
          endGpx();
          isLogging = false;
          gpxClosed = true;
        }
      }
    }
  }

  lastButtonState = reading;

  if (gps.location.isUpdated() && isLogging) {
    if (gpxFile) {
      gpxFile.print("      <trkpt lat=\"");
      gpxFile.print(gps.location.lat(), 6);
      gpxFile.print("\" lon=\"");
      gpxFile.print(gps.location.lng(), 6);
      gpxFile.println("\">");

      gpxFile.print("        <ele>");
      gpxFile.print(gps.altitude.meters());
      gpxFile.println("</ele>");

      gpxFile.print("        <time>");
      writeTime(gpxFile);
      gpxFile.println("</time>");

      // 🔽 DODANE: prędkość
      gpxFile.print("        <speed>");
      gpxFile.print(gps.speed.kmph(), 1);
      gpxFile.println("</speed>");

      gpxFile.println("      </trkpt>");
      gpxFile.flush();
    }

    Serial.print("✅ Zapisano punkt: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.println(gps.location.lng(), 6);
  }

  updateDisplayStatus(); // aktualizacja OLED
  delay(1000);
}

void startGpx() {
  filename = "/track.gpx";
  int fileIndex = 1;
  while (SD.exists(filename)) {
    filename = "/track" + String(fileIndex) + ".gpx";
    fileIndex++;
  }

  gpxFile = SD.open(filename, FILE_WRITE);
  if (gpxFile) {
    gpxFile.println("<gpx version=\"1.1\" creator=\"ESP32-GPS\">");
    gpxFile.println("  <trk>");
    gpxFile.println("    <name>Trasa ESP32</name>");
    gpxFile.println("    <trkseg>");
    gpxFile.flush();
    Serial.print("▶️ Rozpoczęto zapis do: ");
    Serial.println(filename);
    gpxClosed = false;
  } else {
    Serial.println("❌ Błąd otwarcia pliku GPX.");
  }
}

void writeTime(File &file) {
  char buffer[30];
  if (gps.date.isValid() && gps.time.isValid()) {
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ",
            gps.date.year(), gps.date.month(), gps.date.day(),
            gps.time.hour(), gps.time.minute(), gps.time.second());
    file.print(buffer);
  } else {
    file.print("1970-01-01T00:00:00Z");
  }
}

void endGpx() {
  if (gpxFile) {
    gpxFile.println("    </trkseg>");
    gpxFile.println("  </trk>");
    gpxFile.println("</gpx>");
    gpxFile.close();
    Serial.println("🛑 Zapis zakończony i plik zamknięty.");
  }
}

void updateDisplayStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);

  if (isLogging) {
    display.println("Zapis trasy...");
    display.println(filename);
  } else if (gpxClosed) {
    display.println("Plik zamkniety");
  } else {
    display.println("Gotowy do zapisu");
  }

  display.display();
}
