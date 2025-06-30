#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_wifi.h"
#include <DHT.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Wire.h>

#define RELAY_PIN  22
#define SWITCH_BUTTON_PIN 27 
#define RESET_BUTTON_PIN 5  // GPIO for Reset Button

Preferences preferences;
const char* default_ssid = "ESP32_Config_AP";
const char* default_password = "123456789";
const char* mqtt_server = "192.168.246.62";  // MQTT server address
int mqtt_port = 1883;
const char* mqtt_user = "project2";
const char* mqtt_password = "project2";


const char* commandTopic = "esp32/appliance/set";
const char* stateTopic = "esp32/appliance/state";

bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // milliseconds

 
String applianceState = "OFF";

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);


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


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(WiFi.macAddress());
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT Broker!");
      client.subscribe(commandTopic);  // ✅ Subscribes to correct command topic
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

void publishState(String blindState){
  StaticJsonDocument<200> doc;
  doc["appliance_state"] = applianceState;
  char buffer[128];
  serializeJson(doc, buffer);
  client.publish(stateTopic, buffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("📩 Received on topic: ");
  Serial.print(topic);
  Serial.print(" | Payload: ");
  Serial.println(message);

  if (String(topic) == commandTopic) {
    message.trim();
    message.toUpperCase();

    if (message == "ON") {
      digitalWrite(RELAY_PIN, HIGH);  // Turn ON the appliance
      applianceState = "ON";
    } else if (message == "OFF") {
      digitalWrite(RELAY_PIN, LOW);   // Turn OFF the appliance
      applianceState = "OFF";
    }

    publishState(applianceState);  // Report new state
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SWITCH_BUTTON_PIN, INPUT_PULLUP);  // Pull-up for active-LOW button

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
  digitalWrite(RELAY_PIN, LOW);
}

// void loop() {
//   if (!client.connected()) {
//     reconnect();  // Reconnect if disconnected
//   }
    
//   // Ensure the connection is kept alive
//   client.loop();  // Maintain MQTT connection
//   digitalWrite(RELAY_PIN, HIGH); // Turns ON the relay (energizes it)
//   delay(10000);                   // Relay stays ON for 10 seconds
//   digitalWrite(RELAY_PIN, LOW);  // Turns OFF the relay (de-energizes)
//   delay(5000);                   // Relay stays OFF for 5 seconds
// }


// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();  // Handle MQTT

//   // === Button Debounce Logic ===
//   int reading = digitalRead(SWITCH_BUTTON_PIN);

//   if (reading != lastButtonState) {
//     lastDebounceTime = millis();
//   }

//   if ((millis() - lastDebounceTime) > debounceDelay) {
//     if (reading == LOW && lastButtonState == HIGH) {
//       // Button pressed - Toggle relay state
//       if (applianceState == "OFF") {
//         digitalWrite(RELAY_PIN, HIGH);
//         applianceState = "ON";
//       } else {
//         digitalWrite(RELAY_PIN, LOW);
//         applianceState = "OFF";
//       }

//       publishState(applianceState);  // Update MQTT
//       Serial.print("Manual toggle: ");
//       Serial.println(applianceState);
//     }
//   }

//   lastButtonState = reading;
// }

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int reading = digitalRead(SWITCH_BUTTON_PIN);
  Serial.print("Button state: ");
  Serial.println(reading);  // DEBUG

  if (reading != lastButtonState) {
    // Reset debounce timer
    lastDebounceTime = millis();
  }

  // If the reading is stable for longer than debounceDelay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    static bool lastStableState = HIGH;  // Track last stable state

    // Only toggle if the state changed and is now pressed (LOW)
    if (reading != lastStableState) {
      lastStableState = reading;

      if (reading == LOW) {  // Button press detected
        if (applianceState == "OFF") {
          digitalWrite(RELAY_PIN, HIGH);
          applianceState = "ON";
        } else {
          digitalWrite(RELAY_PIN, LOW);
          applianceState = "OFF";
        }

        publishState(applianceState);
        Serial.print("Manual toggle: ");
        Serial.println(applianceState);
      }
    }
  }

  lastButtonState = reading;  // Always update this for debounce tracking
}






// #define RELAY_PIN  5

// void setup() {
//   pinMode(RELAY_PIN, OUTPUT);
// }

// void loop() {
//   digitalWrite(RELAY_PIN, HIGH); // Turns ON the relay (energizes it)
//   delay(2000);                   // Relay stays ON for 2 seconds
//   digitalWrite(RELAY_PIN, LOW);  // Turns OFF the relay (de-energizes)
//   delay(2000);                   // Relay stays OFF for 2 seconds
// }


// #define RELAY_PIN 5

// void setup() {
//   pinMode(RELAY_PIN, OUTPUT);
//   digitalWrite(RELAY_PIN, LOW); // Try LOW first
// }

// void loop() {
//   // nothing
// }
