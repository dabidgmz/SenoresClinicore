#include <DHT.h>
#include <DHT_U.h>
#include "HX711.h"

// Temperatura
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Altura
#define TRIG_PIN 21
#define ECHO_PIN 26

// Bascula
#define DT_PIN 16
#define SCK_PIN 14
HX711 scale;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S2!");

  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  scale.begin(DT_PIN, SCK_PIN);  

  if (!scale.is_ready()) {
    Serial.println("HX711 no está listo.");
    return;
  }

  Serial.println("HX711 listo.");
  scale.set_scale();  
  scale.tare();

  Serial.println("Coloca un peso...");
}

void loop() {
  delay(2000);


  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Error al leer el sensor DHT!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(hum);
    Serial.println(" %");
  }


  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.0343 / 2;

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- HX711 ---
  if (scale.is_ready()) {
    float peso = scale.get_units(5);  
    Serial.print("Peso: ");
    Serial.print(peso);
    Serial.println(" kg (aproximado)");
  } else {
    Serial.println("HX711 no está listo.");
  }
}
