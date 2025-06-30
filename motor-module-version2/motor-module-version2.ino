#include <Arduino.h>

// Pin definitions
const int IN3 = 14;    // Direction pin 1
const int IN4 = 27;    // Direction pin 2
const int ENB = 26;    // Enable pin (PWM)

// Desired speed (0–255)
const int motorSpeed = 100; // lower = slower

void setup() {
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);  // IMPORTANT: also set as OUTPUT

  Serial.begin(115200);
}

void loop() {
  // Run motor forward
  Serial.println("Running forward");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, motorSpeed);  // Control speed
  delay(3000);

  // Stop motor
  Serial.println("Stopping");
  analogWrite(ENB, 0);  // Stop PWM
  delay(1000);

  // Run motor backward
  Serial.println("Running backward");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, motorSpeed);  // Control speed
  delay(3000);

  // Stop motor
  Serial.println("Stopping");
  analogWrite(ENB, 0);
  delay(1000);
}
