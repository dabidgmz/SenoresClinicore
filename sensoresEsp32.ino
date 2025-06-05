#include <WiFi.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include "HX711.h"

// LCD I2C
LiquidCrystal_I2C LCD(0x27, 16, 2);


#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define TRIG_PIN 12
#define ECHO_PIN 13

#define DT_PIN 16
#define SCK_PIN 14
HX711 scale;


#define LED_VERDE 2
#define LED_AMARILLO 5
#define LED_ROJO 10

// Buzzer
#define BUZZER_PIN 3


const float ALTURA_SENSOR_CM = 220.0;
const float HX711_FACTOR = 2280.0;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sistema...");

  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(HX711_FACTOR);
  scale.tare(); 

  
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 

  Wire.begin(21, 20);
  LCD.init();
  LCD.backlight();
  LCD.setCursor(0, 0);
  LCD.print("Inicializando...");
  delay(1000);

  if (!scale.is_ready()) {
    Serial.println("HX711 no está listo.");
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("HX711 no listo");
    return;
  }

  Serial.println("HX711 listo.");
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Todo listo");
  delay(1000);
}

void actualizarSemaforo(float temp) {
  // Apagar todos por defecto
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  if (temp > 40.0) {
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(BUZZER_PIN, HIGH); 
  } else if (temp >= 30.0) {
    digitalWrite(LED_AMARILLO, HIGH);
  } else {
    digitalWrite(LED_VERDE, HIGH);
  }
}

void loop() {
  delay(2000);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Error al leer DHT!");
  } else {
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" °C, Hum: ");
    Serial.print(hum);
    Serial.println(" %");

  
    actualizarSemaforo(temp);
  }


  long duration = 0;
  float distancia = 0.0;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    Serial.println("¡No se recibió eco!");
    distancia = 0.0;
  } else {
    distancia = duration * 0.0343 / 2.0;
  }

  float altura_persona = ALTURA_SENSOR_CM - distancia;
  if (altura_persona < 0) altura_persona = 0;

  Serial.print("Altura persona: ");
  Serial.print(altura_persona, 1);
  Serial.println(" cm");

  // Peso
  float peso = 0;
  if (scale.is_ready()) {
    peso = scale.get_units(5);  
    Serial.print("Peso: ");
    Serial.print(peso, 2);
    Serial.println(" kg");
  } else {
    Serial.println("HX711 no está listo.");
  }

  // Mostrar en LCD
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Altura: ");
  LCD.print(altura_persona, 1);
  LCD.print("cm");

  LCD.setCursor(0, 1);
  LCD.print("Peso: ");
  LCD.print(peso, 1);
  LCD.print("kg");
}
