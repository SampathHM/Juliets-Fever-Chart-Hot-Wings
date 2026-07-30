// Tutorial 04 — Make it move.  https://gardenspine.ikapo.fi/tutorial/04
//
// Opens a servo when the visitor counter publishes, then folds it back without delay().
// Success: the horn moves on an incoming message while publishing keeps working.
//
// Wiring: servo red -> 5V/VIN, brown -> GND, signal -> GPIO 18, shared ground.
// Never power a servo from the ESP32 3V3 pin. Needs the "ESP32Servo" library.

#include <ESP32Servo.h>
#include <GardenSpine.h>

const char* COUNTER_TOPIC = "garden/entrance/counter/gk-01/count";

// CHANGE THESE to fit your own safe movement.
const int SERVO_PIN = 18;
const int REST_ANGLE = 25;
const int ACTIVE_ANGLE = 70;
const unsigned long GESTURE_MS = 700;

GardenSpine spine;
Servo wings;
unsigned long foldAt = 0;

void reactToCounter(const char* topic, const char* payload) {
	(void)topic;
	(void)payload;
	// Move now, and remember when to move back. Do not wait here.
	wings.write(ACTIVE_ANGLE);
	foldAt = millis() + GESTURE_MS;
}

void setup() {
	Serial.begin(115200);
	wings.attach(SERVO_PIN);
	wings.write(REST_ANGLE);
	spine.subscribe(COUNTER_TOPIC, reactToCounter);
	spine.begin();
}

void loop() {
	spine.loop();

	// Fold the wings once the gesture time is over. This is what delay() would have blocked.
	if (foldAt != 0 && static_cast<long>(millis() - foldAt) >= 0) {
		wings.write(REST_ANGLE);
		foldAt = 0;
	}
}
