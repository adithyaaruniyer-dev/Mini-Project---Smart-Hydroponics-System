/*
 * Hydroponics Monitoring System
 * Sensors: DS18B20 Temperature + HC-SR04 Ultrasonic
 * Display: 0.96" OLED (128x64)
 * 
 * Required Libraries:
 * - OneWire by Paul Stoffregen
 * - DallasTemperature by Miles Burton
 * - Adafruit SSD1306
 * - Adafruit GFX Library
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin Definitions
#define ONE_WIRE_BUS 2      // DS18B20 Data pin
#define TRIG_PIN 9          // HC-SR04 Trigger pin
#define ECHO_PIN 10         // HC-SR04 Echo pin

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temperature sensor setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables
float temperature = 0.0;
float waterLevel = 0.0;
const float TANK_HEIGHT = 30.0;  // Total tank height in cm (adjust to your tank)

void setup() {
  Serial.begin(9600);
  
  // Initialize DS18B20
  sensors.begin();
  
  // Initialize HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Display startup message
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.println(F("Hydroponics"));
  display.setCursor(20, 35);
  display.println(F("Monitor v1.0"));
  display.display();
  delay(2000);
}

void loop() {
  // Read temperature
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  
  // Read water level
  waterLevel = getWaterLevel();
  
  // Display on OLED
  updateDisplay();
  
  // Print to Serial Monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C | Water Level: ");
  Serial.print(waterLevel);
  Serial.println(" cm");
  
  delay(1000);  // Update every second
}

float getWaterLevel() {
  long duration;
  float distance;
  
  // Clear trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read echo
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate distance in cm
  distance = duration * 0.034 / 2;
  
  // Convert to water level (distance from sensor to water surface)
  // Water level = Tank height - distance from sensor
  float level = TANK_HEIGHT - distance;
  
  // Ensure level is within valid range
  if (level < 0) level = 0;
  if (level > TANK_HEIGHT) level = TANK_HEIGHT;
  
  return level;
}

void updateDisplay() {
  display.clearDisplay();
  
  // Title
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println(F("HYDROPONICS"));
  
  // Draw separator line
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Temperature
  display.setTextSize(1);
  display.setCursor(0, 18);
  display.println(F("Temperature:"));
  display.setTextSize(2);
  display.setCursor(10, 28);
  display.print(temperature, 1);
  display.print(F(" C"));
  
  // Water Level
  display.setTextSize(1);
  display.setCursor(0, 46);
  display.println(F("Water Level:"));
  display.setTextSize(2);
  display.setCursor(10, 54);
  display.print(waterLevel, 1);
  display.print(F(" cm"));
  
  display.display();
}