

// #include <Arduino.h>

// // Pin definitions
// #define ENA 18    // PWM speed control
// #define IN1 14    // Direction pin 1
// #define IN2 27    // Direction pin 2

// // PWM settings
// #define PWM_FREQ 5000     // 5 kHz
// #define PWM_CHANNEL 0
// #define PWM_RESOLUTION 8  // 8-bit: 0-255

// void setup() {
//   // Set up direction pins
//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);

//   // Set up PWM channel
//   ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
//   ledcAttachPin(ENA, PWM_CHANNEL);

//   Serial.begin(115200);
// }

// void loop() {
//   // Example: run motor forward at 70% speed for 3 sec
//   Serial.println("Running forward");
//   digitalWrite(IN1, HIGH);
//   digitalWrite(IN2, LOW);
//   ledcWrite(PWM_CHANNEL, 180); // 70% of 255 ~ 180
//   delay(3000);

//   // Stop motor for 1 sec
//   Serial.println("Stopping");
//   ledcWrite(PWM_CHANNEL, 0);
//   delay(1000);

//   // Run motor backward at 70% speed for 3 sec
//   Serial.println("Running backward");
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, HIGH);
//   ledcWrite(PWM_CHANNEL, 180);
//   delay(3000);

//   // Stop motor for 1 sec
//   Serial.println("Stopping");
//   ledcWrite(PWM_CHANNEL, 0);
//   delay(1000);
// }


#include <Arduino.h>

// Pin definitions
#define IN3 14    // Direction pin 1
#define IN4 27    // Direction pin 2
#define ENB 26    // Enable pin for Motor B

void setup() {
  // Set up pins
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable the motor channel
  digitalWrite(ENB, HIGH);  // Full speed

  Serial.begin(115200);
}

void loop() {
  // Run motor forward for 3 sec
  Serial.println("Running forward");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(3000);

  // Stop motor for 1 sec
  Serial.println("Stopping");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(1000);

  // Run motor backward for 3 sec
  Serial.println("Running backward");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(3000);

  // Stop motor for 1 sec
  Serial.println("Stopping");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(1000);
}

