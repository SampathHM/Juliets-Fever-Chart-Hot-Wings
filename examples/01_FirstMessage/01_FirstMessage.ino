// Tutorial 01 — Hello, spine.  https://gardenspine.ikapo.fi/tutorial/01
//
// Publishes one fixed temperature, once. No sensor needed.
// Success: your device shows a green "alive" badge on the developer view.
//
// Needs config.h in this folder. See the starter README, step 6.

#include <GardenSpine.h>

GardenSpine spine;
bool firstMessageSent = false;

void setup() {
	Serial.begin(115200);
	spine.begin();
}

void loop() {
	spine.loop();
	if (spine.connected() && !firstMessageSent) {
		// "temperature" is what you measured, 22.5 is the value, "celsius" is the registered unit.
		firstMessageSent = spine.publish("temperature", 22.5, "celsius");
	}
}
