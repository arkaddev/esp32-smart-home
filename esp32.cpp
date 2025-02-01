#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include <WiFi.h>

#include <HTTPClient.h>

Adafruit_BMP280 bmp;

const char* ssid = "";
const char* password = "";

const char* serverUrl = "/receive.php";



// Ustawienia dla OLED (128x64 pikseli, I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Adres I2C dla Twojego wyświetlacza: 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  // Uruchamiamy komunikację szeregowa
  Serial.begin(115200);

  temperatureSensor();
  
  connectToWifi();

  displayOnOled();

}

void loop() {
  
  //wyswietlenie temperatury
  float temperature = bmp.readTemperature();  
 
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  

display.clearDisplay();
display.setTextSize(3);      // Ustawienie większego rozmiaru tekstu (np. 2)
  display.setTextColor(SSD1306_WHITE);  // Kolor tekstu (biały)
  display.setCursor(0, 0);     // Ustawienie pozycji kursora

  // Wyświetl tekst na OLED
  display.println(temperature);
  
  // Zaktualizuj ekran
  display.display();




// Wysłanie danych do serwera
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"temperature\": " + String(temperature) + "}";
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    Serial.println("Dane wysłane, odpowiedź: " + String(httpResponseCode));
  } else {
    Serial.println("Błąd wysyłania danych: " + String(httpResponseCode));
  }

  http.end();


  delay(5000); 
  
  }



void temperatureSensor(){

  if (!bmp.begin(0x76)) {
    Serial.println("Nie udało się znaleźć czujnika BMP280!");
    while (1);
  }

  Serial.println("Czujnik BMP280 połączony!");
}

void connectToWifi(){

  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);  
    Serial.println("Łączenie z Wi-Fi...");
  }

  Serial.println("Połączono z Wi-Fi!");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP()); 
}

void displayOnOled(){

// Inicjalizacja wyświetlacza z adresem 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Adres 0x3C
    Serial.println(F("Nie udało się zainicjować wyświetlacza OLED"));
    for (;;);  // Jeśli nie uda się zainicjować, zatrzymaj program
  }

  // Wyczyść ekran
  display.clearDisplay();

  // Ustawienie tekstu na większy rozmiar
  display.setTextSize(2);      // Ustawienie większego rozmiaru tekstu (np. 2)
  display.setTextColor(SSD1306_WHITE);  // Kolor tekstu (biały)
  display.setCursor(0, 0);     // Ustawienie pozycji kursora

  // Wyświetl tekst na OLED
  display.println(F("Hello, World!"));
  
  // Zaktualizuj ekran
  display.display();


}
