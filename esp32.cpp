#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Tworzymy obiekt czujnika BMP280
Adafruit_BMP280 bmp; // Domyślnie używa I2C

void setup() {
  // Uruchamiamy komunikację szeregowa
  Serial.begin(115200);
  
  // Inicjalizujemy komunikację I2C i czujnik BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("Nie udało się znaleźć czujnika BMP280!");
    while (1); // Zatrzymujemy program, jeśli czujnik nie został znaleziony
  }

  Serial.println("Czujnik BMP280 połączony!");
}

void loop() {
  // Odczytujemy temperaturę, ciśnienie i wysokość
  float temperature = bmp.readTemperature();  // Temperatura w °C
 
  // Wyświetlamy wyniki w monitorze szeregowym
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  

  delay(2000); // Czekamy 2 sekundy przed kolejnym odczytem
}
