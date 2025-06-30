#define LIMIT_SWITCH_PIN D3  // GPIO5 or GPIO23

void setup() {
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); // Internal pull-up enabled
  Serial.begin(115200);
}

void loop() {
  if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
    Serial.println("Limit switch pressed!");
  } else {
    Serial.println("Limit switch not pressed.");
  }
  delay(300);
}
