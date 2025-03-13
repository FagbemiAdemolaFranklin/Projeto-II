#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 4  // GPIO where the data wire is connected
#define DHTTYPE DHT11  // Change to DHT22 if using that sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    Serial.println("DHT Sensor Test");
    dht.begin();
}

void loop() {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" °C | Humidity: ");
    Serial.print(hum);
    Serial.println(" %");

    delay(2000);  // Wait 2 seconds before next reading
}
