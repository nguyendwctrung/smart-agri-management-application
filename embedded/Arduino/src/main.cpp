#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <MQUnifiedsensor.h>

// ===== CONFIG =====
#define TIME_WAIT 3000   // giảm xuống để test nhanh
#define RX_ZIGBEE 10
#define TX_ZIGBEE 11

#define DHTPIN 2
#define DHTTYPE DHT11

#define BOARD "Arduino UNO"
#define PIN A0
#define VOLTAGE 5
#define ADC_BIT_RESOLUTION 10
#define RATIO_MQ135_CLEAN_AIR 3.6

#define SOIL_PIN A1

BH1750 LightSensor;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial ZigbeeCom(RX_ZIGBEE, TX_ZIGBEE);
MQUnifiedsensor MQ135(BOARD, VOLTAGE, ADC_BIT_RESOLUTION, PIN, "MQ-135");

String Packet(float lux, float temperature, float humidity, float co2, float smoke, float soil) {
  return String(lux) + "," +
         String(temperature) + "," +
         String(humidity) + "," +
         String(co2) + "," +
         String(smoke) + "," +
         String(soil);
}

// ===== SOIL =====
float calculate_soil_moisture_percentage(int value) {
  return 100 - (value / 1023.0) * 100;
}

void setup() {
  Serial.begin(9600);
  ZigbeeCom.begin(9600);

  Wire.begin();
  LightSensor.begin();
  dht.begin();

  MQ135.setRegressionMethod(1);
  MQ135.init();

  Serial.println("Calibrating MQ135...");

  float r0 = 0;
  for (int i = 0; i < 10; i++) {
    MQ135.update();
    r0 += MQ135.calibrate(RATIO_MQ135_CLEAN_AIR);
    delay(500);
  }
  MQ135.setR0(r0 / 10.0);

  Serial.println("MQ135 Ready!");

  pinMode(SOIL_PIN, INPUT);
}

void loop() {
  // ===== MQ135 =====
  MQ135.update();

  // CO2
  MQ135.setA(605.18);
  MQ135.setB(-3.937);
  float co2 = MQ135.readSensor();

  // Smoke
  MQ135.setA(110.47);
  MQ135.setB(-2.862);
  float smoke = MQ135.readSensor();

  float lux = LightSensor.readLightLevel();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float soil = calculate_soil_moisture_percentage(analogRead(SOIL_PIN));

  if (isnan(lux) || isnan(temperature) || isnan(humidity)) {
    Serial.println("Sensor error!");
    delay(1000);
    return;
  }

  Serial.print("Lux: "); Serial.print(lux);
  Serial.print(" | Temp: "); Serial.print(temperature);
  Serial.print(" | Hum: "); Serial.print(humidity);
  Serial.print(" | CO2: "); Serial.print(co2);
  Serial.print(" | Smoke: "); Serial.print(smoke);
  Serial.print(" | Soil: "); Serial.println(soil);

  String data = Packet(lux, temperature, humidity, co2, smoke, soil);

  ZigbeeCom.println(data);

  Serial.print("Sent: ");
  Serial.println(data);

  delay(TIME_WAIT);
}