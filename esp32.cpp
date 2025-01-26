#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <WiFi.h>

#include <HTTPClient.h>

Adafruit_BMP280 bmp;

const char* ssid = "";
const char* password = "";

const char* serverUrl = "/odbierz.php";


void setup() {
  // Uruchamiamy komunikację szeregowa
  Serial.begin(115200);

  temperatureSensor();
  
  connectToWifi();

}

void loop() {
  
  //wyswietlenie temperatury
  float temperature = bmp.readTemperature();  
 
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  



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
