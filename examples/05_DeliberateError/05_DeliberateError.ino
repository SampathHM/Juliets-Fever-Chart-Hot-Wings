// Tutorial 05 — When it breaks.  https://gardenspine.ikapo.fi/tutorial/05
//
// This sketch is WRONG ON PURPOSE. It sends "C" where the registry expects "celsius".
// Success: the message appears in the reject feed with a hint naming the fix.
//
// The backbone stores bad messages instead of dropping them. Flash this once, read the
// hint on your device page, then change "C" to "celsius" and flash again.

#include <GardenSpine.h>

GardenSpine spine;
bool sent = false;

void setup() {
	Serial.begin(115200);
	spine.begin();
}

void loop() {
	spine.loop();
	if (spine.connected() && !sent) {
		// Serial Monitor still says "published". The device did its job; the unit is wrong.
		sent = spine.publish("temperature", 23.4, "C");
	}
}
