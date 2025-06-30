
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_wifi.h"
#include <DHT.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

// Pin Definitions
#define MQ2_PIN 32 // Analog input pin for MQ-2 sensor
#define LEDRED_PIN 15
#define LEDGREEN_PIN 16
#define LEDYELLOW_PIN 18
#define MOTOR_PIN 22
#define RESET_BUTTON_PIN 5  // GPIO for Reset Button
#define numSensors 5  // Set the actual number of sensors
#define GAS_THRESHOLD 310 // Gas threshold value for warning  // Threshold 300 a 10000 
#define DHTPIN 4         // GPIO pin where the DHT is connected
#define DHTTYPE DHT11    // Change to DHT11 if needed
DHT dht(DHTPIN, DHTTYPE);
// Wi-Fi and MQTT config
Preferences preferences;
const char* default_ssid = "ESP32_Config_AP";
const char* default_password = "123456789";
const char* mqtt_server = "192.168.160.43";  // MQTT server address
int mqtt_port = 1884;



WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

// Sensor Configuration Definitions
#define MAX_SENSORS 5  // Limit of sensors
#define EEPROM_SENSOR_ADDR 0  // Starting EEPROM address for storing sensor configs

// Sensor types (DHT11 = 1, MQ2 = 2)
#define SENSOR_TYPE_DHT11 1
#define SENSOR_TYPE_MQ2 2



// Web page to configure Wi-Fi and Sensors
// void handleConfigPage() {
//   String html = "<html><body>";
//   html += "<h1>Configure Wi-Fi</h1>";
//   html += "<form action='/setWiFi' method='get'>";
//   html += "SSID: <input type='text' name='ssid'><br>";
//   html += "Password: <input type='password' name='password'><br>";
//   html += "<input type='submit' value='Set Wi-Fi'><br>";
//   html += "</form>";
//   html += "</body></html>";
//   server.send(200, "text/html", html);
// }


void handleConfigPage() {
  String html = R"rawliteral(
  <html>
    <head>
      <title>Wi-Fi Configuration</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
        body {
          display: flex;
          justify-content: center;
          align-items: center;
          height: 100vh;
          margin: 0;
          font-family: Arial, sans-serif;
          background-color: #121212;
          color: #ffffff;
          transition: background-color 0.3s, color 0.3s;
        }
        .container {
          background: #1e1e1e;
          padding: 30px;
          border-radius: 10px;
          box-shadow: 0 0 10px rgba(255,255,255,0.1);
          width: 300px;
        }
        h2 {
          text-align: center;
          margin-bottom: 20px;
        }
        input[type='text'],
        input[type='password'] {
          padding: 10px;
          background-color: #2c2c2c;
          color: #ffffff;
          border: 1px solid #444;
          border-radius: 5px;
          width: 100%;
        }
        input[type='submit'] {
          padding: 10px;
          background-color: #007BFF;
          color: white;
          border: none;
          border-radius: 5px;
          cursor: pointer;
          margin-top: 10px;
        }
        .theme-toggle {
          position: absolute;
          top: 20px;
          right: 20px;
        }
      </style>
    </head>
    <body>
      <div class="theme-toggle">
        <label>
          <input type="checkbox" id="toggleTheme" checked> Dark Mode
        </label>
      </div>

      <div class="container">
        <h2>Configure Wi-Fi</h2>
        <form action='/setWiFi' method='get' style='display: flex; flex-direction: column; gap: 15px;'>
          <label>SSID:<input type='text' name='ssid'></label>
          <label>Password:<input type='password' name='password'></label>
          <input type='submit' value='Set Wi-Fi'>
        </form>
      </div>

      <script>
        const toggle = document.getElementById('toggleTheme');
        toggle.addEventListener('change', () => {
          if (toggle.checked) {
            document.body.style.backgroundColor = '#121212';
            document.body.style.color = '#ffffff';
            document.querySelector('.container').style.background = '#1e1e1e';
            document.querySelectorAll('input[type="text"], input[type="password"]').forEach(input => {
              input.style.backgroundColor = '#2c2c2c';
              input.style.color = '#ffffff';
              input.style.border = '1px solid #444';
            });
          } else {
            document.body.style.backgroundColor = '#ffffff';
            document.body.style.color = '#000000';
            document.querySelector('.container').style.background = '#f4f4f4';
            document.querySelectorAll('input[type="text"], input[type="password"]').forEach(input => {
              input.style.backgroundColor = '#ffffff';
              input.style.color = '#000000';
              input.style.border = '1px solid #ccc';
            });
          }
        });
      </script>
    </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}


// Save Wi-Fi Configuration
void saveWiFiConfig(String ssid, String password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putBool("wifiConfigured", true);
  preferences.end();
}

// Load Wi-Fi Configuration
bool loadWiFiCredentials(String &ssid, String &password) {
  preferences.begin("wifi", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  bool configured = preferences.getBool("wifiConfigured", false);
  preferences.end();
  return (ssid != "" && password != "" && configured);
}



// Clear Wi-Fi Configuration
void clearWiFiConfig() {
  preferences.begin("wifi", false);
  preferences.clear();  // Clears all keys
  preferences.end();
}

// Reset Wi-Fi hardware
void resetWiFi() {
  esp_wifi_deinit();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_err_t err = esp_wifi_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("Wi-Fi Init Failed: %s\n", esp_err_to_name(err));
  }
}

// Connect to Wi-Fi
void connectWiFi(String ssid, String password) {
  resetWiFi();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Connecting to WiFi");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi connection failed. Starting AP...");
    startAccessPoint();
  }
}

// Start Access Point Mode
void startAccessPoint() {
  WiFi.softAP(default_ssid, default_password);
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  Serial.println("🔧 Access Point Mode Started: ESP32_Config_AP");

  server.on("/", HTTP_GET, handleConfigPage);
  server.on("/setWiFi", HTTP_GET, []() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    if (ssid.length() > 0 && password.length() > 0) {
      saveWiFiConfig(ssid, password);
      server.send(200, "text/plain", "WiFi Configured. Restarting...");
      delay(2000);
      ESP.restart();
    } else {
      server.send(400, "text/plain", "Missing SSID or password");
    }
  });


  server.begin();
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

// void callback(char* topic, byte* payload, unsigned int length) {
//   String message;
//   for (unsigned int i = 0; i < length; i++) {
//     message += (char)payload[i];
//   }

//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.print("] ");
//   Serial.println(message);

//     if (String(topic) == "homeassistant") {
//       if (message == "LED_ON") {
//         digitalWrite(LEDYELLOW_PIN, HIGH);
//         client.publish("home", "LED_ON");
//       } else if (message == "LED_OFF") {
//         digitalWrite(LEDYELLOW_PIN, LOW);
//         client.publish("home", "LED_OFF");
//       } else if (message == "MOTOR_ON") {
//         digitalWrite(MOTOR_PIN, HIGH);
//         client.publish("home", "MOTOR_ON");
//       } else if (message == "MOTOR_OFF") {
//         digitalWrite(MOTOR_PIN, LOW);
//         client.publish("home", "MOTOR_OFF");
//       }
//     }

// }

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  String topicStr = String(topic);

  if (topicStr == "esp32/led/set") {
    if (message == "ON") {
      digitalWrite(LEDYELLOW_PIN, HIGH);
      client.publish("home", "ON");
    } else if (message == "OFF") {
      digitalWrite(LEDYELLOW_PIN, LOW);
      client.publish("home", "OFF");
    }
  } else if (topicStr == "esp32/motor/set") {
    if (message == "ON") {
      digitalWrite(MOTOR_PIN, HIGH);
      client.publish("home", "ON");
    } else if (message == "OFF") {
      digitalWrite(MOTOR_PIN, LOW);
      client.publish("home", "OFF");
    }
  }
}


// Function to reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Broker!");
      client.subscribe("homeassistant");  // Subscribe to LED control topic
      client.subscribe("esp32/led/set");
      client.subscribe("esp32/motor/set");
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
  pinMode(LEDYELLOW_PIN, OUTPUT);
  pinMode(LEDRED_PIN, OUTPUT);
  pinMode(LEDGREEN_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  

  // Reset if button is held
  unsigned long pressedTime = 0;
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    Serial.println("⏳ Button pressed. Waiting to confirm...");
    pressedTime = millis();
    while (digitalRead(RESET_BUTTON_PIN) == LOW) {
      if (millis() - pressedTime >= 3000) {
        Serial.println("⚠️ Reset button held. Clearing Wi-Fi credentials...");
        clearWiFiConfig();
        delay(2000);
        ESP.restart();
      }
    }
  }

  String ssid, password;
  if (loadWiFiCredentials(ssid, password)) {
    connectWiFi(ssid, password);
  } else {
    startAccessPoint();
  }

  // ⚠️ Stay in AP mode until credentials are provided
  while (!loadWiFiCredentials(ssid, password)) {
    server.handleClient();  // Allow web configuration
    delay(100);             // Give time to web server
  }

  client.setServer(mqtt_server, mqtt_port);  // Set MQTT server address
  client.setCallback(callback);  // Set MQTT callback for incoming messages
  client.setKeepAlive(60);  // Set keep-alive interval to 60 seconds
  analogReadResolution(10); // Set ADC resolution to 10-bit
  Serial.println("MQ-135 Sensor Initialized!");
}

void loop() {
  //server.handleClient();

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


