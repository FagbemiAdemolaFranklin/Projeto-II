#include <Arduino.h>

// Motor control pins
const int IN3 = 14;
const int IN4 = 27;
const int ENB = 26;

// Button pins
const int UP_BUTTON = 22;
const int DOWN_BUTTON = 21;

// Motor speed (0–255)
const int motorSpeed = 120;

void setup() {
  // Motor pins
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Buttons: use INPUT_PULLUP
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop() {
  // Read buttons (active LOW)
  bool upPressed = (digitalRead(UP_BUTTON) == LOW);
  bool downPressed = (digitalRead(DOWN_BUTTON) == LOW);

  if (upPressed && !downPressed) {
    // Move UP
    Serial.println("UP");
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, motorSpeed);
  } else if (downPressed && !upPressed) {
    // Move DOWN
    Serial.println("DOWN");
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, motorSpeed);
  } else {
    // Stop motor if no button pressed or both pressed
    Serial.println("STOP");
    analogWrite(ENB, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  delay(50);  // debounce + loop stability
}
