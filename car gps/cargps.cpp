#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <SD.h>

// Obiekty
TinyGPSPlus gps;
HardwareSerial mySerial(1);
File gpxFile;

// Konfiguracja pinów
#define SD_CS 5           // Chip Select (CS) dla karty SD
#define BUTTON_PIN 4      // Przycisk zakończenia trasy

bool gpxClosed = false;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // GPS RX=16, TX=17
  pinMode(BUTTON_PIN, INPUT_PULLUP);       // Przycisk z pull-up

  // Inicjalizacja SPI z własnymi pinami dla SD (SCK=22, MISO=19, MOSI=23, CS=5)
  SPI.begin(15, 19, 23, SD_CS);

  // Inicjalizacja SD
  if (!SD.begin(SD_CS)) {
    Serial.println("❌ Błąd: nie można zainicjalizować karty SD.");
    while (true);
  }
  Serial.println("✅ Karta SD OK.");

  // Sprawdzamy, czy plik o nazwie "track.gpx" już istnieje, jeśli tak, tworzmy nowy plik
  String filename = "/track.gpx";
  int fileIndex = 1;
  while (SD.exists(filename)) {
    filename = "/track" + String(fileIndex) + ".gpx";
    fileIndex++;
  }

  // Tworzymy nowy plik GPX
  gpxFile = SD.open(filename, FILE_WRITE);
  if (gpxFile) {
    gpxFile.println("<gpx version=\"1.1\" creator=\"ESP32-GPS\">");
    gpxFile.println("  <trk>");
    gpxFile.println("    <name>Trasa ESP32</name>");
    gpxFile.println("    <trkseg>");
    gpxFile.flush();
    Serial.print("📍 Rozpoczęto zapis trasy w pliku: ");
    Serial.println(filename);
  } else {
    Serial.println("❌ Błąd otwarcia pliku GPX.");
  }
}

void loop() {
  while (mySerial.available() > 0) {
    gps.encode(mySerial.read());
  }

  if (gps.location.isUpdated() && !gpxClosed) {
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

      gpxFile.println("      </trkpt>");
      gpxFile.flush();
    }

    Serial.print("✅ Zapisano punkt: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.println(gps.location.lng(), 6);
  }

  // Przycisk – zakończenie zapisu
  if (!digitalRead(BUTTON_PIN) && !gpxClosed) {
    Serial.println("🛑 Przycisk naciśnięty – kończę zapis GPX...");
    endGpx();
    gpxClosed = true;
  }

  delay(1000);
}

// Zapisuje znacznik czasu w formacie ISO 8601
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

// Zamyka plik GPX
void endGpx() {
  if (gpxFile) {
    gpxFile.println("    </trkseg>");
    gpxFile.println("  </trk>");
    gpxFile.println("</gpx>");
    gpxFile.close();
    Serial.println("📁 Zakończono i zamknięto plik GPX.");
  }
}
