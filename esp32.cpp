#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <WiFi.h>

Adafruit_BMP280 bmp;

const char* ssid = "";
const char* password = "";




void setup() {
  // Uruchamiamy komunikację szeregowa
  Serial.begin(115200);

  temperatureSensor();
  
  connectToWifi();

}

void loop() {
  
  float temperature = bmp.readTemperature();  // Temperatura w °C
 
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  

  delay(2000); 
  
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
