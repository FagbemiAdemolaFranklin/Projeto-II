#define MQ2_PIN 32 // Analog input pin for MQ-2 sensor
#define LEDRED_PIN 15 // Digital output pin for red LED
#define LEDGREEN_PIN 16 // Digital output pin for green LED

// Threshold value for gas detection
#define GAS_THRESHOLD 310 // Gas threshold value for warning
// Threshold 300 a 10000 

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  analogReadResolution(10); // Set ADC resolution to 10-bit
  Serial.println("MQ-135 Sensor Initialized!");

  // Set I/O pin modes
  pinMode(MQ2_PIN, INPUT);
  pinMode(LEDRED_PIN, OUTPUT);
  pinMode(LEDGREEN_PIN, OUTPUT);
  digitalWrite(LEDRED_PIN, LOW);
  digitalWrite(LEDGREEN_PIN, LOW);
}

void loop() {
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
}