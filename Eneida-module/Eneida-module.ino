// #include <SPI.h>
// #include <ADE9000RegMap.h>
// #include <ADE9000API.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include "esp_wifi.h"
// #include <ArduinoJson.h>
// #include <Preferences.h>
// #include <WebServer.h>
// #include <DNSServer.h>
// #include <ESPmDNS.h>


// /*Basic initializations*/
// ADE9000Class ade9000;
// #define SPI_SPEED 5000000     //SPI Speed
// #define CS_PIN 17 
// #define ADE9000_RESET_PIN 18 //Reset Pin on HW
// #define PM_0 3
// #define PM_1 8             //PM1
// //#define CLK 11
// //#define MOSI 9// EPM_SIMO
// //#define MISO  10 // EPM_SOMI
// #define ADE_ValMax 52702092.0
// #define ADE_PotValMax 20694066.0
// #define VRMS_ValMax 566.39
// #define IRMS_ValMax 245.5
// #define meio 0.5
// #define RESET_BUTTON_PIN 5  // GPIO for Reset Button
// /*Structure decleration */
// struct ActivePowerRegs powerRegs;     // Declare powerRegs of type ActivePowerRegs to store Active Power Register data
// struct CurrentRMSRegs curntRMSRegs;   //Current RMS
// struct VoltageRMSRegs vltgRMSRegs;    //Voltage RMS
// struct VoltageTHDRegs voltageTHDRegsnValues; //Voltage THD
// struct ResampledWfbData resampledData; // Resampled Data


// double airms=0, birms=0, cirms=0, VaRMS=0, VbRMS=0, VcRMS=0, Awatt=0, Bwatt=0, Cwatt=0;


// // Wi-Fi and MQTT config
// extern Preferences preferences;
// const char* default_ssid = "ESP32_Config_AP";
// const char* default_password = "123456789";
// const char* mqtt_server = "BULLET.local";  // MQTT server address
// int mqtt_port = 1884;



// WiFiClient espClient;
// PubSubClient client(espClient);
// WebServer server(80);

// /*Function Decleration*/
// void readRegisterData(void);
// void readResampledData(void);
// void resetADE9000(void);




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



// // Function to reconnect to MQTT broker
// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     if (client.connect("ESP32Client")) {
//       Serial.println("Connected to MQTT Broker!");
//       client.subscribe("homeassistant");  // Subscribe to LED control topic
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


// void setup() 
// {
//   Serial.begin(115200);
//   pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
//   pinMode(PM_1, OUTPUT);
//   pinMode(PM_0, OUTPUT); 
//   digitalWrite(PM_0, LOW);   //Set PM1 pin as output 
//   digitalWrite(PM_1, LOW);   //Set PM1 select pin low for PSM0 mode
//   pinMode(ADE9000_RESET_PIN, OUTPUT);
//   digitalWrite(ADE9000_RESET_PIN, HIGH); 

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
//   // client.setCallback(callback);  // Set MQTT callback for incoming messages
//   client.setKeepAlive(60);  // Set keep-alive interval to 60 seconds
//   analogReadResolution(10); // Set ADC resolution to 10-bit
  
//   void resetADE9000(); 
//   delay(1000);
//   ade9000.SPI_Init(SPI_SPEED, CS_PIN);//Initialize SPI
//   ade9000.SetupADE9000();             //Initialize ADE9000 registers according to values in ADE9000API.h
//   //ade9000.SPI_Write_16(ADDR_RUN,0x1); //Set RUN=1 to turn on DSP. Uncomment if SetupADE9000 function is not used
//   Serial.print("RUN Register: ");
//   Serial.println(ade9000.SPI_Read_16(ADDR_RUN),HEX);
// }

// void loop() {
//   if (!client.connected()) {
//     reconnect();  // Reconnect if disconnected
//   }
 
//   //readResampledData();
  
//   //ade9000.SPI_Write_32(ADDR_AIGAIN0, 5874);
//   //uint32_t value = ade9000.SPI_Read_32(ADDR_AIGAIN0);
//   //Serial.print("VaLUE:");
//   //Serial.println(value);

 
    
//   // Ensure the connection is kept alive
//   client.loop();  // Maintain MQTT connection
//   readRegisterData();
  
//   // Create JSON payload
//   StaticJsonDocument<200> doc;
//   doc["airms"] = airms;
//   doc["birms"] = birms;
//   doc["cirms"] = cirms;
//   doc["VaRMS"] = VaRMS;
//   doc["VbRMS"] = VbRMS;
//   doc["VcRMS"] = VcRMS;
//   doc["Awatt"] = Awatt;
//   doc["Bwatt"] = Bwatt;
//   doc["Cwatt"] = Cwatt;
//   char payload[128];
//   serializeJson(doc, payload);


//   // Publish
//   client.publish("home", payload);
  
//   Serial.print("Published: ");
//   Serial.println(payload);

//   delay(1000);

// }

// void readRegisterData()
// {
//  /*Read and Print Specific Register using ADE9000 SPI Library */
//   Serial.print("A_IRMS: "); 
//   airms=convertCurrent(ade9000.SPI_Read_32(ADDR_AIRMS));
//   Serial.println(airms); // AIRMS

//    Serial.print("B_IRMS: "); 
//   birms=convertCurrent(ade9000.SPI_Read_32(ADDR_BIRMS));
//   Serial.println(birms); // AIRMS
    
//   Serial.print("C_IRMS: "); 
//   cirms=convertCurrent(ade9000.SPI_Read_32(ADDR_CIRMS));
//   Serial.println(cirms); // AIRM


//  /*Read and Print RMS & WATT Register using ADE9000 Read Library*/
//   ade9000.ReadVoltageRMSRegs(&vltgRMSRegs);    //Template to read Power registers from ADE9000 and store data in Arduino MCU
//   ade9000.ReadActivePowerRegs(&powerRegs);
  
//   Serial.print("A_VRMS:");
//   VaRMS=convertVoltage(vltgRMSRegs.VoltageRMSReg_A);        
//   Serial.println(VaRMS); 
//   Serial.print("B_VRMS:");
//   VbRMS=convertVoltage(vltgRMSRegs.VoltageRMSReg_B);        
//   Serial.println(VbRMS); 
//   Serial.print("C_VRMS:");
//   VcRMS=convertVoltage(vltgRMSRegs.VoltageRMSReg_C);        
//   Serial.println(VcRMS); 

//   Serial.print("A_WATT:");        
//   Awatt=convertPower(powerRegs.ActivePowerReg_A);       
//   Serial.print(Awatt);
//   Serial.print("B_WATT:");        
//   Bwatt=convertPower(powerRegs.ActivePowerReg_B);       
//   Serial.print(Bwatt);
//   Serial.print("C_WATT:"); 
//   Cwatt=convertPower(powerRegs.ActivePowerReg_C);       
//   Serial.print(Cwatt);
// }

// double convertVoltage(int value){ 
//   double v_rms = (value / ADE_ValMax) * VRMS_ValMax;
//   return (v_rms);
// }

// double convertCurrent(unsigned int value){ 
//   double I_rms = (value/ADE_ValMax)*IRMS_ValMax*meio;
//   return (I_rms);
// }

// double convertPower(int value){
//   double P_rms = ((value*IRMS_ValMax*VRMS_ValMax)/ADE_PotValMax)*meio;
//   return(P_rms);
// }

// void resetADE9000(void)
// {
//  digitalWrite(ADE9000_RESET_PIN, LOW);
//  delay(50);
//  digitalWrite(ADE9000_RESET_PIN, HIGH);
//  delay(1000);
//  Serial.println("Reset Done");
// }













#include <SPI.h>
#include <ADE9000RegMap.h>
#include <ADE9000API.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <esp_wifi.h>

/* Basic initializations */
ADE9000Class ade9000;
#define SPI_SPEED 5000000
#define CS_PIN 17
#define ADE9000_RESET_PIN 18
#define PM_0 3
#define PM_1 8
#define ADE_ValMax 52702092.0
#define ADE_PotValMax 20694066.0
#define VRMS_ValMax 566.39
#define IRMS_ValMax 245.5
#define meio 0.5
#define RESET_BUTTON_PIN 5  // GPIO for Reset Button

struct ActivePowerRegs powerRegs;
struct CurrentRMSRegs curntRMSRegs;
struct VoltageRMSRegs vltgRMSRegs;
struct VoltageTHDRegs voltageTHDRegsnValues;
struct ResampledWfbData resampledData;

double airms = 0, birms = 0, cirms = 0, VaRMS = 0, VbRMS = 0, VcRMS = 0, Awatt = 0, Bwatt = 0, Cwatt = 0;

// Hardcoded Wi-Fi and MQTT configuration
const char* ssid = "BULLET0723";
const char* password = "0123456789";
const char* mqtt_server = "192.168.246.62";  // MQTT server address
int mqtt_port = 1883;
const char* mqtt_user = "project2";
const char* mqtt_password = "project2";
WiFiClient espClient;
PubSubClient client(espClient);

/* Function declarations */
void readRegisterData(void);
void readResampledData(void);
void resetADE9000(void);

void resetWiFi() {
  esp_wifi_deinit();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_err_t err = esp_wifi_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("Wi-Fi Init Failed: %s\n", esp_err_to_name(err));
  }
}

void connectWiFi() {
  resetWiFi();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

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
    Serial.println("\n❌ WiFi connection failed. Please check credentials.");
    // Optional: reboot or continue running in offline mode
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(WiFi.macAddress());
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
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
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PM_1, OUTPUT);
  pinMode(PM_0, OUTPUT);
  digitalWrite(PM_0, LOW);
  digitalWrite(PM_1, LOW);
  pinMode(ADE9000_RESET_PIN, OUTPUT);
  digitalWrite(ADE9000_RESET_PIN, HIGH);

  // Optional reset button logic (can remove if unused)
  // unsigned long pressedTime = 0;
  // if (digitalRead(RESET_BUTTON_PIN) == LOW) {
  //   Serial.println("⏳ Button pressed. Waiting to confirm...");
  //   pressedTime = millis();
  //   while (digitalRead(RESET_BUTTON_PIN) == LOW) {
  //     if (millis() - pressedTime >= 3000) {
  //       Serial.println("⚠️ Reset button held. (No Wi-Fi prefs to clear in hardcoded mode)");
  //       delay(2000);
  //       ESP.restart();
  //     }
  //   }
  // }

  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setKeepAlive(60);

  analogReadResolution(10);
  delay(1000);
  ade9000.SPI_Init(SPI_SPEED, CS_PIN);
  ade9000.SetupADE9000();
  // ade9000.SPI_Write_16(ADDR_RUN, 0x1); // Optional
}

void loop() {
  if (!client.connected()) {
    reconnect();  // Reconnect if disconnected
  }
 
  //readResampledData();
  
  //ade9000.SPI_Write_32(ADDR_AIGAIN0, 5874);
  //uint32_t value = ade9000.SPI_Read_32(ADDR_AIGAIN0);
  //Serial.print("VaLUE:");
  //Serial.println(value);

 
    
  // Ensure the connection is kept alive
  client.loop();  // Maintain MQTT connection
  readRegisterData();
  
  // Create JSON payload
  StaticJsonDocument<200> doc;
  doc["airms"] = airms;
  doc["birms"] = birms;
  doc["cirms"] = cirms;
  doc["VaRMS"] = VaRMS;
  doc["VbRMS"] = VbRMS;
  doc["VcRMS"] = VcRMS;
  doc["Awatt"] = Awatt;
  doc["Bwatt"] = Bwatt;
  doc["Cwatt"] = Cwatt;
  char payload[128];
  serializeJson(doc, payload);


  // Publish
  client.publish("eneida/01/power", payload);
  
  Serial.print("Published: ");
  Serial.println(payload);

  delay(2000);
}





void readRegisterData()
{
 /*Read and Print Specific Register using ADE9000 SPI Library */
  Serial.print("A_IRMS: "); 
  airms=convertCurrent(ade9000.SPI_Read_32(ADDR_AIRMS));
  Serial.println(airms); // AIRMS

   Serial.print("B_IRMS: "); 
  birms=convertCurrent(ade9000.SPI_Read_32(ADDR_BIRMS));
  Serial.println(birms); // AIRMS
    
  Serial.print("C_IRMS: "); 
  cirms=convertCurrent(ade9000.SPI_Read_32(ADDR_CIRMS));
  Serial.println(cirms); // AIRM


 /*Read and Print RMS & WATT Register using ADE9000 Read Library*/
  ade9000.ReadVoltageRMSRegs(&vltgRMSRegs);    //Template to read Power registers from ADE9000 and store data in Arduino MCU
  ade9000.ReadActivePowerRegs(&powerRegs);
  
  Serial.print("A_VRMS:");
  VaRMS=convertVoltage(vltgRMSRegs.VoltageRMSReg_A);        
  Serial.println(VaRMS); 
  Serial.print("B_VRMS:");
  VbRMS=convertVoltage(vltgRMSRegs.VoltageRMSReg_B);        
  Serial.println(VbRMS); 
  Serial.print("C_VRMS:");
  VcRMS=convertVoltage(vltgRMSRegs.VoltageRMSReg_C);        
  Serial.println(VcRMS); 

  Serial.print("A_WATT:");        
  Awatt=convertPower(powerRegs.ActivePowerReg_A);       
  Serial.print(Awatt);
  Serial.print("B_WATT:");        
  Bwatt=convertPower(powerRegs.ActivePowerReg_B);       
  Serial.print(Bwatt);
  Serial.print("C_WATT:"); 
  Cwatt=convertPower(powerRegs.ActivePowerReg_C);       
  Serial.print(Cwatt);
}

double convertVoltage(int value){ 
  double v_rms = (value / ADE_ValMax) * VRMS_ValMax;
  return (v_rms);
}

double convertCurrent(unsigned int value){ 
  double I_rms = (value/ADE_ValMax)*IRMS_ValMax*meio;
  return (I_rms);
}

double convertPower(int value){
  double P_rms = ((value*IRMS_ValMax*VRMS_ValMax)/ADE_PotValMax)*meio;
  return(P_rms);
}

void resetADE9000(void)
{
 digitalWrite(ADE9000_RESET_PIN, LOW);
 delay(50);
 digitalWrite(ADE9000_RESET_PIN, HIGH);
 delay(1000);
 Serial.println("Reset Done");
}




