// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include "esp_wifi.h"
// #include <ArduinoJson.h>
// #include <Preferences.h>
// #include <WebServer.h>
// #include <DNSServer.h>
// #include <ESPmDNS.h>


// #define RESET_BUTTON_PIN 5  // GPIO for Reset Button
// // Motor control pins
// const int IN3 = 14;
// const int IN4 = 27;
// const int ENB = 26;

// // Button pins
// const int UP_BUTTON = 22;
// const int DOWN_BUTTON = 21;
// //  Limit switches
// const int TOP_LIMIT_PIN = 32;     // adjust GPIO numbers as needed
// const int BOTTOM_LIMIT_PIN = 25;


// // Speed: 0–255
// const int motorSpeed = 120;

// // WiFi & MQTT config
// Preferences preferences;
// const char* default_ssid = "ESP32_Config_AP";
// const char* default_password = "123456789";
// const char* mqtt_server = "192.168.246.62";  // MQTT server address
// int mqtt_port = 1883;
// const char* mqtt_user = "project2";
// const char* mqtt_password = "project2";

// WiFiClient espClient;
// PubSubClient client(espClient);
// WebServer server(80);

// // State
// String blindState = "STOP";  // "UP", "DOWN", "STOP"
// String lastManualState = "STOP"; 

// const char* commandTopic = "esp32/blind/set";
// const char* stateTopic = "esp32/blind/state";

// void handleConfigPage() {
//   String html = R"rawliteral(
//   <html>
//     <head>
//       <title>Wi-Fi Configuration</title>
//       <meta name="viewport" content="width=device-width, initial-scale=1">
//       <style>
//         body {
//           display: flex;
//           justify-content: center;
//           align-items: center;
//           height: 100vh;
//           margin: 0;
//           font-family: Arial, sans-serif;
//           background-color: #121212;
//           color: #ffffff;
//           transition: background-color 0.3s, color 0.3s;
//         }
//         .container {
//           background: #1e1e1e;
//           padding: 30px;
//           border-radius: 10px;
//           box-shadow: 0 0 10px rgba(255,255,255,0.1);
//           width: 300px;
//         }
//         h2 {
//           text-align: center;
//           margin-bottom: 20px;
//         }
//         input[type='text'],
//         input[type='password'] {
//           padding: 10px;
//           background-color: #2c2c2c;
//           color: #ffffff;
//           border: 1px solid #444;
//           border-radius: 5px;
//           width: 100%;
//         }
//         input[type='submit'] {
//           padding: 10px;
//           background-color: #007BFF;
//           color: white;
//           border: none;
//           border-radius: 5px;
//           cursor: pointer;
//           margin-top: 10px;
//         }
//         .theme-toggle {
//           position: absolute;
//           top: 20px;
//           right: 20px;
//         }
//       </style>
//     </head>
//     <body>
//       <div class="theme-toggle">
//         <label>
//           <input type="checkbox" id="toggleTheme" checked> Dark Mode
//         </label>
//       </div>

//       <div class="container">
//         <h2>Configure Wi-Fi</h2>
//         <form action='/setWiFi' method='get' style='display: flex; flex-direction: column; gap: 15px;'>
//           <label>SSID:<input type='text' name='ssid'></label>
//           <label>Password:<input type='password' name='password'></label>
//           <input type='submit' value='Set Wi-Fi'>
//         </form>
//       </div>

//       <script>
//         const toggle = document.getElementById('toggleTheme');
//         toggle.addEventListener('change', () => {
//           if (toggle.checked) {
//             document.body.style.backgroundColor = '#121212';
//             document.body.style.color = '#ffffff';
//             document.querySelector('.container').style.background = '#1e1e1e';
//             document.querySelectorAll('input[type="text"], input[type="password"]').forEach(input => {
//               input.style.backgroundColor = '#2c2c2c';
//               input.style.color = '#ffffff';
//               input.style.border = '1px solid #444';
//             });
//           } else {
//             document.body.style.backgroundColor = '#ffffff';
//             document.body.style.color = '#000000';
//             document.querySelector('.container').style.background = '#f4f4f4';
//             document.querySelectorAll('input[type="text"], input[type="password"]').forEach(input => {
//               input.style.backgroundColor = '#ffffff';
//               input.style.color = '#000000';
//               input.style.border = '1px solid #ccc';
//             });
//           }
//         });
//       </script>
//     </body>
//   </html>
//   )rawliteral";

//   server.send(200, "text/html", html);
// }


// // Save Wi-Fi Configuration
// void saveWiFiConfig(String ssid, String password) {
//   preferences.begin("wifi", false);
//   preferences.putString("ssid", ssid);
//   preferences.putString("password", password);
//   preferences.putBool("wifiConfigured", true);
//   preferences.end();
// }

// // Load Wi-Fi Configuration
// bool loadWiFiCredentials(String &ssid, String &password) {
//   preferences.begin("wifi", true);
//   ssid = preferences.getString("ssid", "");
//   password = preferences.getString("password", "");
//   bool configured = preferences.getBool("wifiConfigured", false);
//   preferences.end();
//   return (ssid != "" && password != "" && configured);
// }



// // Clear Wi-Fi Configuration
// void clearWiFiConfig() {
//   preferences.begin("wifi", false);
//   preferences.clear();  // Clears all keys
//   preferences.end();
// }

// // Reset Wi-Fi hardware
// void resetWiFi() {
//   esp_wifi_deinit();
//   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//   esp_err_t err = esp_wifi_init(&cfg);
//   if (err != ESP_OK) {
//     Serial.printf("Wi-Fi Init Failed: %s\n", esp_err_to_name(err));
//   }
// }

// // Connect to Wi-Fi
// void connectWiFi(String ssid, String password) {
//   resetWiFi();
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid.c_str(), password.c_str());

//   Serial.print("Connecting to WiFi");
//   int retries = 0;
//   while (WiFi.status() != WL_CONNECTED && retries < 20) {
//     delay(500);
//     Serial.print(".");
//     retries++;
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\n✅ Connected to WiFi!");
//     Serial.print("IP Address: ");
//     Serial.println(WiFi.localIP());
//   } else {
//     Serial.println("\n❌ WiFi connection failed. Starting AP...");
//     startAccessPoint();
//   }
// }

// // Start Access Point Mode
// void startAccessPoint() {
//   WiFi.softAP(default_ssid, default_password);
//   IPAddress local_IP(192, 168, 4, 1);
//   IPAddress gateway(192, 168, 4, 1);
//   IPAddress subnet(255, 255, 255, 0);
//   WiFi.softAPConfig(local_IP, gateway, subnet);

//   Serial.println("🔧 Access Point Mode Started: ESP32_Config_AP");

//   server.on("/", HTTP_GET, handleConfigPage);
//   server.on("/setWiFi", HTTP_GET, []() {
//     String ssid = server.arg("ssid");
//     String password = server.arg("password");
//     if (ssid.length() > 0 && password.length() > 0) {
//       saveWiFiConfig(ssid, password);
//       server.send(200, "text/plain", "WiFi Configured. Restarting...");
//       delay(2000);
//       ESP.restart();
//     } else {
//       server.send(400, "text/plain", "Missing SSID or password");
//     }
//   });


//   server.begin();
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     String clientId = "ESP32Client-" + String(WiFi.macAddress());
//     if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
//       Serial.println("Connected to MQTT Broker!");
//       client.subscribe(commandTopic);  // ✅ Subscribes to correct command topic
//     } else {
//       Serial.print("Failed to connect to MQTT. Error code: ");
//       Serial.println(client.state());
//       // Add more verbose logging for debugging
//       if (client.state() == MQTT_CONNECT_BAD_CREDENTIALS) {
//         Serial.println("Check the MQTT username/password in Home Assistant.");
//       } else if (client.state() == MQTT_CONNECT_UNAUTHORIZED) {
//         Serial.println("Check MQTT broker permissions for the user.");
//       } else if (client.state() == MQTT_CONNECT_UNAVAILABLE) {
//         Serial.println("The MQTT broker is unavailable or misconfigured.");
//       }
//       Serial.println(" Retrying in 5 seconds...");
//       delay(5000);
//     }
//   }
// }

// void publishState(String blindState){
//   StaticJsonDocument<200> doc;
//   doc["blind_state"] = blindState;
//   char buffer[128];
//   serializeJson(doc, buffer);
//   client.publish(stateTopic, buffer);
// }

// void moveUp() {
//   if (digitalRead(TOP_LIMIT_PIN) == LOW) {  // Active LOW => pressed
//     Serial.println("Top limit reached! Cannot move UP.");
//     stopMotor();
//     return;
//   }

//   Serial.println("Moving UP");
//   digitalWrite(IN3, HIGH);
//   digitalWrite(IN4, LOW);
//   analogWrite(ENB, motorSpeed);
//   publishState("UP");
  
// }

// void moveDown() {
//   if (digitalRead(BOTTOM_LIMIT_PIN) == LOW) {
//     Serial.println("Bottom limit reached! Cannot move DOWN.");
//     stopMotor();
//     return;
//   }

//   Serial.println("Moving DOWN");
//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, HIGH);
//   analogWrite(ENB, motorSpeed);
//   publishState("DOWN");
// }

// void stopMotor() {
//   Serial.println("Stopping");
//   analogWrite(ENB, 0);
//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, LOW);
//   publishState("STOP");
// }

// // MQTT callback: react to HA commands
// void callback(char* topic, byte* payload, unsigned int length) {
//   String command = "";
//   for (uint i = 0; i < length; i++) {
//     command += (char)payload[i];
//   }
//   Serial.print("Received: ");
//   Serial.println(command);

//   if (command == "UP") {
//     moveUp();
//   } else if (command == "DOWN") {
//     moveDown();
//   } else if (command == "STOP") {
//     stopMotor();
//   }
// }


// void setup() {
//   Serial.begin(115200);

//   // Motor pins
//   pinMode(IN3, OUTPUT);
//   pinMode(IN4, OUTPUT);
//   pinMode(ENB, OUTPUT);

//   // Buttons
//   pinMode(UP_BUTTON, INPUT_PULLUP);   // Active LOW
//   pinMode(DOWN_BUTTON, INPUT_PULLUP);
//   pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
//   // ✅ NEW: Limit switches as INPUT_PULLUP
//   pinMode(TOP_LIMIT_PIN, INPUT_PULLUP);
//   pinMode(BOTTOM_LIMIT_PIN, INPUT_PULLUP);


//   // Reset if button is held
//   unsigned long pressedTime = 0;
//   if (digitalRead(RESET_BUTTON_PIN) == LOW) {
//     Serial.println("⏳ Button pressed. Waiting to confirm...");
//     pressedTime = millis();
//     while (digitalRead(RESET_BUTTON_PIN) == LOW) {
//       if (millis() - pressedTime >= 3000) {
//         Serial.println("⚠️ Reset button held. Clearing Wi-Fi credentials...");
//         clearWiFiConfig();
//         delay(2000);
//         ESP.restart();
//       }
//     }
//   }

//   String ssid, password;
//   if (loadWiFiCredentials(ssid, password)) {
//     connectWiFi(ssid, password);
//   } else {
//     startAccessPoint();
//   }

//   // ⚠️ Stay in AP mode until credentials are provided
//   while (!loadWiFiCredentials(ssid, password)) {
//     server.handleClient();  // Allow web configuration
//     delay(100);             // Give time to web server
//   }

//   client.setServer(mqtt_server, mqtt_port);  // Set MQTT server address
//   client.setCallback(callback);  // Set MQTT callback for incoming messages
//   client.setKeepAlive(60);  // Set keep-alive interval to 60 seconds
//   analogReadResolution(10); // Set ADC resolution to 10-bit

  
// }



// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();

//   // ✅ NEW: Emergency stop if moving and hits limit switch
//   if (blindState == "UP" && digitalRead(TOP_LIMIT_PIN) == LOW) {
//     Serial.println("Emergency stop: Top limit switch hit");
//     stopMotor();
//   }

//   if (blindState == "DOWN" && digitalRead(BOTTOM_LIMIT_PIN) == LOW) {
//     Serial.println("Emergency stop: Bottom limit switch hit");
//     stopMotor();
//   }

//   bool upPressed = digitalRead(UP_BUTTON) == LOW;
//   bool downPressed = digitalRead(DOWN_BUTTON) == LOW;

//   if (upPressed && !downPressed) {
//     if (lastManualState != "UP") {
//       moveUp();
//       lastManualState = "UP";
//     }
//   } else if (downPressed && !upPressed) {
//     if (lastManualState != "DOWN") {
//       moveDown();
//       lastManualState = "DOWN";
//     }
//   } else {
//     if (lastManualState != "STOP" && blindState != "UP" && blindState != "DOWN") {
//       stopMotor();
//       lastManualState = "STOP";
//     }
//   }
// }





#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_wifi.h"
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>


#define RESET_BUTTON_PIN 5  // GPIO for Reset Button
// Motor control pins
const int IN3 = 14;
const int IN4 = 27;
const int ENB = 26;

// Button pins
const int UP_BUTTON = 22;
const int DOWN_BUTTON = 21;
// ✅ NEW: Limit switches
const int TOP_LIMIT_PIN = 32;     // adjust GPIO numbers as needed
const int BOTTOM_LIMIT_PIN = 25;
bool isAtTop = false;
bool isAtBottom = false;
bool movingUp = false;
bool movingDown = false;
bool triggeredByButton = false;




// Speed: 0–255
const int motorSpeed = 120;

// WiFi & MQTT config
Preferences preferences;
const char* default_ssid = "ESP32_Config_AP";
const char* default_password = "123456789";
const char* mqtt_server = "192.168.246.62";  // MQTT server address
int mqtt_port = 1883;
const char* mqtt_user = "project2";
const char* mqtt_password = "project2";

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

// State
String blindState = "STOP";  // "UP", "DOWN", "STOP"
String lastManualState = "STOP"; 

const char* commandTopic = "esp32/blind/set";
const char* stateTopic = "esp32/blind/state";

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
  doc["blind_state"] = blindState;
  char buffer[128];
  serializeJson(doc, buffer);
  client.publish(stateTopic, buffer);
}

void moveUp() {
  if (digitalRead(TOP_LIMIT_PIN) == LOW) {
    Serial.println("Top limit already reached! Cannot move UP.");
    publishState("UP");
    return;
  }

  Serial.println("Moving UP");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, motorSpeed);

  blindState = "UP";
  movingUp = true;
  movingDown = false;
  isAtBottom = false;
  
}

void moveDown() {
  if (digitalRead(BOTTOM_LIMIT_PIN) == LOW) {
    Serial.println("Bottom limit already reached! Cannot move DOWN.");
    publishState("DOWN");
    return;
  }

  Serial.println("Moving DOWN");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, motorSpeed);

  blindState = "DOWN";
  movingDown = true;
  movingUp = false;
  isAtTop = false;

  

}




void stopMotor() {
  Serial.println("Stopping");
  analogWrite(ENB, 0);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  movingUp = false;
  movingDown = false;
  blindState = "STOP";
}


// MQTT callback: react to HA commands
void callback(char* topic, byte* payload, unsigned int length) {
  String command = "";
  for (uint i = 0; i < length; i++) {
    command += (char)payload[i];
  }

  Serial.print("Received MQTT command: ");
  Serial.println(command);

  // Movement triggered from Home Assistant (not buttons)
  triggeredByButton = false;

  if (command == "UP") {
    if (digitalRead(TOP_LIMIT_PIN) == HIGH) {
      moveUp();
    } else {
      Serial.println("MQTT UP ignored: Top limit already pressed");
      stopMotor();  // optional
    }
  } else if (command == "DOWN") {
    if (digitalRead(BOTTOM_LIMIT_PIN) == HIGH) {
      moveDown();
    } else {
      Serial.println("MQTT DOWN ignored: Bottom limit already pressed");
      stopMotor();
    }
  } else if (command == "STOP") {
    stopMotor();
  } else {
    Serial.println("Unknown command from MQTT");
  }
}





void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Buttons
  pinMode(UP_BUTTON, INPUT_PULLUP);   // Active LOW
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  // ✅ NEW: Limit switches as INPUT_PULLUP
  pinMode(TOP_LIMIT_PIN, INPUT_PULLUP);
  pinMode(BOTTOM_LIMIT_PIN, INPUT_PULLUP);


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

  
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ✅ Emergency stop if moving and hits limit switch
  if (movingUp && digitalRead(TOP_LIMIT_PIN) == LOW) {
    Serial.println("Top limit switch hit — stopping");
    isAtTop = true;
    isAtBottom = false;
    movingUp = false;
    stopMotor();
    publishState("UP");
  }

  if (movingDown && digitalRead(BOTTOM_LIMIT_PIN) == LOW) {
    Serial.println("Bottom limit switch hit — stopping");
    isAtBottom = true;
    isAtTop = false;
    movingDown = false;
    stopMotor();
    publishState("DOWN");
  }

  // ✅ Manual button control logic
  bool upPressed = digitalRead(UP_BUTTON) == LOW;
  bool downPressed = digitalRead(DOWN_BUTTON) == LOW;

  if (upPressed && !downPressed && !isAtTop) {
    if (!movingUp) {
      triggeredByButton = true;   // 🔁 track that button started this
      moveUp();
    }
  } else if (downPressed && !upPressed && !isAtBottom) {
    if (!movingDown) {
      triggeredByButton = true;
      moveDown();
    }
  } else if (!upPressed && !downPressed && triggeredByButton) {
    if (movingUp || movingDown) {
      stopMotor();
      triggeredByButton = false;  // ✅ reset flag
    }
  }
}






