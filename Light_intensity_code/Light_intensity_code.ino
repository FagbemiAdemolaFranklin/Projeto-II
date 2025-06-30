#include <Wire.h>
#include <BH1750.h>


BH1750 lightMeter(0x23);  // Usa o endereço com ADO em GND
// BH1750 lightMeter(0x5C) usa com endereco ADO em VCC 

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Sensor BH1750 iniciado com sucesso (ADO = GND).");
  } else {
    Serial.println("Erro ao iniciar sensor BH1750. Verifique endereço/conexões.");
  }
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Luminosidade: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
}