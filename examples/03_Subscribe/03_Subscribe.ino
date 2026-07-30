// Tutorial 03 — Read someone else.  https://gardenspine.ikapo.fi/tutorial/03
//
// Subscribes to the entrance visitor counter and prints every message it publishes.
// Success: another team's data appears in your Serial Monitor.
//
// You may subscribe before or after spine.begin(). The helper stores the topic and
// subscribes again after every reconnect, so you do not have to.

#include <GardenSpine.h>

const char* COUNTER_TOPIC = "garden/entrance/counter/gk-01/count";

GardenSpine spine;

// GardenSpine calls this when a message arrives on a topic you subscribed to.
void showCounter(const char* topic, const char* payload) {
	Serial.print("received ");
	Serial.println(topic);
	Serial.print("payload  ");
	Serial.println(payload);
}

void setup() {
	Serial.begin(115200);
	spine.subscribe(COUNTER_TOPIC, showCounter);
	spine.begin();
}

void loop() {
	// Incoming messages only arrive while loop() runs. Keep this call unblocked.
	spine.loop();
}
