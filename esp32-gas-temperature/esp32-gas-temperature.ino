
#include <WiFi.h>
Serial Monitor

#include <PubSubClient.h>

#include "esp_wifi.h"
#include <DHT.h>
#include <ArduinoJson.h>

#define MQ2_PIN 32 // Analog input pin for MQ-2 sensor
#define LEDRED_PIN 15 // Digital output pin for red LED
#define LEDGREEN_PIN 16 // Digital output pin for green LED
#define LEDYELLOW_PIN 18 // Digital output pin for green LED
// Threshold value for gas detection
#define GAS_THRESHOLD 310 // Gas threshold value for warning
// Threshold 300 a 10000 
#define MOTOR_PIN 22 // Digital output to control motor

#define DHTPIN 4         // GPIO pin where the DHT is connected
#define DHTTYPE DHT11    // Change to DHT11 if needed
DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi credentials
const char* ssid = "BULLET0723";
const char* password = "123456789";

// MQTT Broker details (EMQX Broker or Home Assistant IP)
const char* mqtt_server = "192.168.160.43";  // Change to Home Assistant's IP if needed
const int mqtt_port = 1884;  // Default MQTT port

WiFiClient espClient;
PubSubClient client(espClient);

// LED control on GPIO4 (example)
const int ledPin = 4;

// Function to reset Wi-Fi configuration
void resetWiFi() {
    esp_wifi_deinit();  // Deinitialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();  // Default Wi-Fi config
    esp_err_t err = esp_wifi_init(&cfg);  // Reinitialize with default config
    if (err != ESP_OK) {
        Serial.printf("Wi-Fi Init Failed: %s\n", esp_err_to_name(err));
    }
}

// MQTT callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

    if (String(topic) == "homeassistant") {
        if (message == "LED_ON") {
            digitalWrite(LEDYELLOW_PIN, HIGH);
            client.publish("home", "LED_ON");
        } else if (message == "LED_OFF") {
            digitalWrite(LEDYELLOW_PIN, LOW);
            client.publish("home", "LED_OFF");
        } else if (message == "MOTOR_ON") {
            digitalWrite(MOTOR_PIN, HIGH);
            client.publish("home", "MOTOR_ON");
        } else if (message == "MOTOR_OFF") {
            digitalWrite(MOTOR_PIN, LOW);
            client.publish("home", "MOTOR_OFF");
        }
    }

}

// Function to connect to Wi-Fi
void setup_wifi() {
    resetWiFi();  // Reset Wi-Fi configuration

    WiFi.mode(WIFI_STA);  // Set Wi-Fi mode to Station
    WiFi.begin(ssid, password);  // Start Wi-Fi connection

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

// Function to reconnect to MQTT broker
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT Broker!");
            client.subscribe("homeassistant");  // Subscribe to LED control topic
        } else {
            Serial.print("Failed to connect to MQTT. Error code: ");
            Serial.println(client.state());
            // Add more verbose logging for debugging
            if (client.state() == MQTT_CONNECT_BAD_CREDENTIALS) {
                Serial.println("Check the MQTT username/password in Home Assistant.");
            } else if (client.state() == MQTT_CONNECT_UNAUTHORIZED) {
                Serial.println("Check MQTT broker permissions for the user.");
            } else if (client.state() == MQTT_CONNECT_UNAVAILABLE) {
                Serial.println("The MQTT broker is unavailable or misconfigured.");
            }
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(ledPin, OUTPUT);  // Initialize GPIO4 as output (LED)
    setup_wifi();  // Connect to Wi-Fi
    client.setServer(mqtt_server, mqtt_port);  // Set MQTT server address
    client.setCallback(callback);  // Set MQTT callback for incoming messages
    client.setKeepAlive(60);  // Set keep-alive interval to 60 seconds
    analogReadResolution(10); // Set ADC resolution to 10-bit
    Serial.println("MQ-135 Sensor Initialized!");

    // Set I/O pin modes
    pinMode(MQ2_PIN, INPUT);
    pinMode(LEDRED_PIN, OUTPUT);
    pinMode(LEDYELLOW_PIN, OUTPUT);
    pinMode(LEDGREEN_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(LEDRED_PIN, LOW);
    digitalWrite(LEDGREEN_PIN, LOW);
}

int read_gas_sensor(){

    // Read sensor value from MQ-2 sensor
  int gasValue = analogRead(MQ2_PIN);
  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  // Check gas value against threshold
  if (gasValue > GAS_THRESHOLD) {
    Serial.println("Warning! High gas or smoke levels detected!");
    digitalWrite(LEDRED_PIN, HIGH); // Turn on red LED
    digitalWrite(LEDGREEN_PIN, LOW); // Turn off green LED
    } else {
    digitalWrite(LEDRED_PIN, LOW); // Turn off red LED
    digitalWrite(LEDGREEN_PIN, HIGH); // Turn on green LED
  }

  // Small delay
  delay(500);
  return gasValue;
}


void read_temperature_and_humidity(float &temperature, float &humidity) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        temperature = 0.0;
        humidity = 0.0;
    }
}

void loop() {
    if (!client.connected()) {
        reconnect();  // Reconnect if disconnected
    }
    
    // Ensure the connection is kept alive
    client.loop();  // Maintain MQTT connection

    int gas_value = read_gas_sensor();
    float temperature, humidity;
    read_temperature_and_humidity(temperature, humidity);

    // Create JSON payload
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["gas_value"] = gas_value;
    char payload[128];
    serializeJson(doc, payload);


    // Publish
    client.publish("home", payload);
    
    Serial.print("Published: ");
    Serial.println(payload);

    delay(10000);  // Send every 10 seconds
}
