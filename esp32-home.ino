// #include <WiFi.h>
// #include <PubSubClient.h>

// const char* ssid = "Ademola's Galaxy S20 FE";
// const char* password = "123456789";
// const char* mqtt_server = "192.84.13.146"; // Your MQTT broker's IP
// const int mqtt_port = 1883;

// WiFiClient espClient;
// PubSubClient client(espClient);

// void setup() {
//     Serial.begin(115200);
//     WiFi.begin(ssid, password);
    
//     Serial.print("Connecting to WiFi...");
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         Serial.print(".");
//     }
//     Serial.println("\nConnected to WiFi");

//     client.setServer(mqtt_server, mqtt_port);

//     Serial.print("Connecting to MQTT...");
//     while (!client.connected()) {
//         Serial.print(".");
//         if (client.connect("ESP32Client")) {
//             Serial.println("\nConnected to MQTT");
//         } else {
//             Serial.print("Failed, rc=");
//             Serial.print(client.state());
//             Serial.println(" Retrying in 5 seconds...");
//             Serial.println(WiFi.localIP());
//             delay(3000);
//         }
//     }
// }

// void loop() {
//     if (!client.connected()) {
//         Serial.println("MQTT Disconnected! Reconnecting...");
//         while (!client.connect("ESP32Client")) {
//             Serial.print(".");
//             delay(5000);
//         }
//         Serial.println("\nReconnected to MQTT");
//     }

//     client.publish("home/livingroom/temp", "25.5");
//     Serial.println("Published Temperature: 25.5");
//     delay(2000);
// }




// #include <WiFi.h>
// #include <PubSubClient.h>
// #include "esp_wifi.h"

// void resetWiFi() {
//     // Deinitialize Wi-Fi first
//     esp_wifi_deinit();

//     // Get the default Wi-Fi configuration
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

//     // Reinitialize Wi-Fi with default config
//     esp_err_t err = esp_wifi_init(&cfg);
//     if (err != ESP_OK) {
//         Serial.printf("Wi-Fi Init Failed: %s\n", esp_err_to_name(err));
//     }
// }

// // Wi-Fi credentials
// const char* ssid = "BULLET0723";  
// const char* password = "123456789";  

// // MQTT Broker details (Home Assistant IP)
// const char* mqtt_server = "192.84.13.146";  // Change to Home Assistant's IP
// const char* mqtt_user = "project2";    // MQTT username (check HA config)
// const char* mqtt_password = "project2";  

// WiFiClient espClient;
// PubSubClient client(espClient);

// // Callback function to handle incoming MQTT messages
// void callback(char* topic, byte* payload, unsigned int length) {
//     Serial.print("Message arrived on topic: ");
//     Serial.print(topic);
//     Serial.print(" | Message: ");
//     String message;
    
//     for (int i = 0; i < length; i++) {
//         message += (char)payload[i];
//     }
//     Serial.println(message);

//     // Example: Control an LED based on MQTT message
//     if (String(topic) == "home/led") {
//         if (message == "ON") {
//             digitalWrite(4, HIGH);  // Turn on LED (GPIO4)
//         } else if (message == "OFF") {
//             digitalWrite(4, LOW);   // Turn off LED
//         }
//     }
// }

// // Function to connect to Wi-Fi
// void setup_wifi() {

//   // Reset Wi-Fi Driver
//   resetWiFi();

//   // Start Wi-Fi connection
//   WiFi.mode(WIFI_STA);
//   WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");

//   Serial.print("Connecting");
//   Serial.print("Connecting to WiFi...");
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//       delay(1000);
//       Serial.print(".");
//   }
//   Serial.println("\nConnected to WiFi!");
// }

// // Function to connect to MQTT broker
// void reconnect() {
//     while (!client.connected()) {
//         Serial.print("Attempting MQTT connection...");
//         if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
//             Serial.println("Connected!");

//             // Subscribe to topics
//             client.subscribe("home/led");  
//         } else {
//             Serial.print("Failed. Error code: ");
//             Serial.print(client.state());
//             Serial.println(" Retrying in 5 seconds...");
//             delay(5000);
//         }
//     }
// }

// void setup() {
//     Serial.begin(115200);
//     pinMode(4, OUTPUT); // Set GPIO2 as output (for LED example)

//     setup_wifi();
//     client.setServer(mqtt_server, 1883);  
//     client.setCallback(callback);
// }

// void loop() {
//     if (!client.connected()) {
//         reconnect();
//     }
//     client.loop();

//     // Publish example sensor data
//     float temperature = random(20, 30); // Simulated temperature
//     char tempString[8];
//     dtostrf(temperature, 1, 2, tempString);
//     client.publish("home/sensor/temperature", tempString);

//     delay(2000); // Publish every 5 seconds
// }






// #include "WiFi.h"

// void setup() {
//     Serial.begin(115200);
//     Serial.println("Scanning for Wi-Fi networks...");

//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect();
//     delay(100);

//     int n = WiFi.scanNetworks();
//     Serial.println("Scan done!");
    
//     if (n == 0) {
//         Serial.println("No networks found.");
//     } else {
//         Serial.print(n);
//         Serial.println(" networks found:");
//         for (int i = 0; i < n; i++) {
//             Serial.print(i + 1);
//             Serial.print(": ");
//             Serial.print(WiFi.SSID(i));
//             Serial.print(" (");
//             Serial.print(WiFi.RSSI(i));
//             Serial.println(" dBm)");
//         }
//     }
// }

// void loop() {
// }


// #include <WiFi.h>
// #include <PubSubClient.h>
// #include "esp_wifi.h"

// // Wi-Fi credentials
// const char* ssid = "BULLET0723";
// const char* password = "123456789";

// // MQTT Broker details (Home Assistant IP)
// const char* mqtt_server = "broker.emqx.io";  // Change to Home Assistant's IP


// WiFiClient espClient;
// PubSubClient client(espClient);

// // LED control on GPIO4 (example)
// const int ledPin = 4;

// // Function to reset Wi-Fi configuration
// void resetWiFi() {
//     esp_wifi_deinit();  // Deinitialize Wi-Fi
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();  // Default Wi-Fi config
//     esp_err_t err = esp_wifi_init(&cfg);  // Reinitialize with default config
//     if (err != ESP_OK) {
//         Serial.printf("Wi-Fi Init Failed: %s\n", esp_err_to_name(err));
//     }
// }

// // MQTT callback function to handle incoming messages
// void callback(char* topic, byte* payload, unsigned int length) {
//     Serial.print("Message arrived on topic: ");
//     Serial.print(topic);
//     Serial.print(" | Message: ");
    
//     String message;
//     for (int i = 0; i < length; i++) {
//         message += (char)payload[i];
//     }
//     Serial.println(message);

//     // Control LED based on MQTT message
//     if (String(topic) == "home/led") {
//         if (message == "ON") {
//             digitalWrite(ledPin, HIGH);  // Turn on LED
//         } else if (message == "OFF") {
//             digitalWrite(ledPin, LOW);   // Turn off LED
//         }
//     }
// }

// // Function to connect to Wi-Fi
// void setup_wifi() {
//     resetWiFi();  // Reset Wi-Fi configuration

//     WiFi.mode(WIFI_STA);  // Set Wi-Fi mode to Station
//     WiFi.begin(ssid, password);  // Start Wi-Fi connection

//     Serial.print("Connecting to WiFi...");
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         Serial.print(".");
//     }
//     Serial.println("\nConnected to WiFi!");
// }

// // Function to reconnect to MQTT broker
// // void reconnect() {
// //     while (!client.connected()) {
// //         Serial.print("Attempting MQTT connection...");
// //         if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
// //             Serial.println("Connected to MQTT Broker!");
// //             client.subscribe("home/led");  // Subscribe to LED control topic
// //         } else {
// //             Serial.print("Failed to connect to MQTT. Error code: ");
// //             Serial.print(client.state());
// //             Serial.println(" Retrying in 5 seconds...");
// //             delay(5000);
// //         }
// //     }
// // }


// // Function to reconnect to MQTT broker
// void reconnect() {
//     while (!client.connected()) {
//         Serial.print("Attempting MQTT connection...");
//         if (client.connect("ESP32Client")) {
//             Serial.println("Connected to MQTT Broker!");
//             client.subscribe("home/led");  // Subscribe to LED control topic
//         } else {
//             Serial.print("Failed to connect to MQTT. Error code: ");
//             Serial.println(client.state());
//             // Add more verbose logging for debugging
//             if (client.state() == MQTT_CONNECT_BAD_CREDENTIALS) {
//                 Serial.println("Check the MQTT username/password in Home Assistant.");
//             } else if (client.state() == MQTT_CONNECT_UNAUTHORIZED) {
//                 Serial.println("Check MQTT broker permissions for the user.");
//             } else if (client.state() == MQTT_CONNECT_UNAVAILABLE) {
//                 Serial.println("The MQTT broker is unavailable or misconfigured.");
//             }
//             Serial.println(" Retrying in 5 seconds...");
//             delay(5000);
//         }
//     }
// }

// void setup() {
//     Serial.begin(115200);
    
//     pinMode(ledPin, OUTPUT);  // Initialize GPIO4 as output (LED)
    
//     setup_wifi();  // Connect to Wi-Fi
//     client.setServer(mqtt_server, 1883);  // Set MQTT server address
//     client.setCallback(callback);  // Set MQTT callback for incoming messages
// }

// void loop() {
//     if (!client.connected()) {
//         reconnect();  // Reconnect if disconnected
//     }
//     client.loop();  // Maintain MQTT connection

//     // Publish simulated sensor data every 2 seconds
//     float temperature = random(20, 30);  // Simulated temperature
//     char tempString[8];
//     dtostrf(temperature, 1, 2, tempString);  // Convert temperature to string
//     client.publish("home/sensor/temperature", tempString);

//     delay(2000);  // Publish every 2 seconds
// }




#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_wifi.h"

// Wi-Fi credentials
const char* ssid = "BULLET0723";
const char* password = "123456789";

// MQTT Broker details (EMQX Broker or Home Assistant IP)
const char* mqtt_server = "broker.emqx.io";  // Change to Home Assistant's IP if needed
const int mqtt_port = 1883;  // Default MQTT port

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
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(" | Message: ");
    
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    // Control LED based on MQTT message
    if (String(topic) == "home/led") {
        if (message == "ON") {
            digitalWrite(ledPin, HIGH);  // Turn on LED
        } else if (message == "OFF") {
            digitalWrite(ledPin, LOW);   // Turn off LED
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
            client.subscribe("home/led");  // Subscribe to LED control topic
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
    
    pinMode(ledPin, OUTPUT);  // Initialize GPIO4 as output (LED)
    
    setup_wifi();  // Connect to Wi-Fi
    client.setServer(mqtt_server, mqtt_port);  // Set MQTT server address
    client.setCallback(callback);  // Set MQTT callback for incoming messages
    client.setKeepAlive(60);  // Set keep-alive interval to 60 seconds
}

void loop() {
    if (!client.connected()) {
        reconnect();  // Reconnect if disconnected
    }
    
    // Ensure the connection is kept alive
    client.loop();  // Maintain MQTT connection

    // Publish simulated sensor data every 2 seconds
    float temperature = random(20, 30);  // Simulated temperature
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);  // Convert temperature to string
    client.publish("home/sensor/temperature", tempString);

    delay(2000);  // Publish every 2 seconds
}
