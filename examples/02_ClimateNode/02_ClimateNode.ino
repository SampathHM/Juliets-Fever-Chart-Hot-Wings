// Tutorial 02 — Publish a real reading.  https://gardenspine.ikapo.fi/tutorial/02
//
// Reads a DHT11 and publishes temperature and humidity as two separate messages.
// Success: a humidity number that changes when you breathe near the sensor.
//
// Wiring: DHT11 VCC -> 3V3, DATA -> GPIO 4, GND -> GND. Disconnect USB before rewiring.
// Needs the "DHT sensor library" and "Adafruit Unified Sensor".

#include <DHT.h>
#include <GardenSpine.h>

const int SENSOR_PIN = 4;

// YOUR CALL: how often to measure.
const unsigned long SAMPLE_MS = 60000;

DHT dht(SENSOR_PIN, DHT11);
GardenSpine spine;
unsigned long lastReading = 0;

void setup() {
	Serial.begin(115200);
	dht.begin();
	spine.begin();
}

void loop() {
	spine.loop();

	bool timeToPublish = millis() - lastReading > SAMPLE_MS;

	if (timeToPublish) {
		lastReading = millis();

		// One measurement per message. Temperature and humidity travel separately.
		spine.publish("temperature", dht.readTemperature(), "celsius");
		spine.publish("humidity", dht.readHumidity(), "percent-rh");
	}
}
