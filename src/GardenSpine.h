#pragma once

#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "spine_ca.h"

class GardenSpine {
 public:
	using MessageHandler = void (*)(const char* topic, const char* payload);
	static const size_t MAX_SUBSCRIPTIONS = 8;

	GardenSpine() : _mqtt(_network) {}

	void begin() {
		Serial.println("Garden Spine: starting");
#ifdef SPINE_CUSTOM_CA
		_network.setCACert(SPINE_CA);
#else
		_network.setCACert(SPINE_PROGRAMME_CA);
#endif
		_mqtt.setServer(SPINE_MQTT_HOST, SPINE_MQTT_PORT);
		_mqtt.setBufferSize(768);
		activeInstance() = this;
		_mqtt.setCallback(dispatchMessage);
		startWifi();
		waitForWifi(30000);
		if (WiFi.status() == WL_CONNECTED) {
			syncClock();
			connectMqtt();
		}
	}

	void loop() {
		if (WiFi.status() != WL_CONNECTED) {
			if (retryDue(_lastWifiRetryAt, 5000)) {
				_lastWifiRetryAt = millis();
				Serial.println("Wi-Fi: reconnecting");
				WiFi.reconnect();
			}
			return;
		}
		if (!_clockReady) syncClock();
		if (!_mqtt.connected()) {
			if (retryDue(_lastMqttRetryAt, 5000)) {
				_lastMqttRetryAt = millis();
				connectMqtt();
			}
			return;
		}
		_mqtt.loop();
	}

	bool connected() {
		return WiFi.status() == WL_CONNECTED && _mqtt.connected();
	}

	bool publish(const char* measurement, double value, const char* unit) {
		if (!isfinite(value)) {
			Serial.print("not published ");
			Serial.print(measurement);
			Serial.println(": sensor returned no number");
			return false;
		}
		char valueText[32];
		snprintf(valueText, sizeof(valueText), "%.2f", value);
		return publishValue(measurement, valueText, unit, false);
	}

	bool publish(const char* measurement, int value, const char* unit) {
		char valueText[24];
		snprintf(valueText, sizeof(valueText), "%d", value);
		return publishValue(measurement, valueText, unit, false);
	}

	bool publish(const char* measurement, const char* value, const char* unit) {
		char escaped[160];
		if (!escapeJson(value, escaped, sizeof(escaped))) {
			Serial.println("not published: text value is too long");
			return false;
		}
		return publishValue(measurement, escaped, unit, true);
	}

	bool subscribe(const char* topic, MessageHandler handler) {
		if (topic == nullptr || handler == nullptr) return false;
		for (size_t i = 0; i < _subscriptionCount; ++i) {
			if (strcmp(_subscriptions[i].topic, topic) == 0) {
				_subscriptions[i].handler = handler;
				return _mqtt.connected() ? _mqtt.subscribe(topic) : true;
			}
		}
		if (_subscriptionCount >= MAX_SUBSCRIPTIONS) {
			Serial.println("subscribe failed: maximum is 8 topics");
			return false;
		}
		_subscriptions[_subscriptionCount++] = {topic, handler};
		return _mqtt.connected() ? _mqtt.subscribe(topic) : true;
	}

 private:
	struct Subscription {
		const char* topic;
		MessageHandler handler;
	};

	WiFiClientSecure _network;
	PubSubClient _mqtt;
	Subscription _subscriptions[MAX_SUBSCRIPTIONS] = {};
	size_t _subscriptionCount = 0;
	char _incomingPayload[513] = {};
	unsigned long _lastWifiRetryAt = 0;
	unsigned long _lastMqttRetryAt = 0;
	bool _clockReady = false;

	static GardenSpine*& activeInstance() {
		static GardenSpine* instance = nullptr;
		return instance;
	}

	static bool retryDue(unsigned long lastAttempt, unsigned long interval) {
		return lastAttempt == 0 || millis() - lastAttempt >= interval;
	}

	void startWifi() {
		WiFi.mode(WIFI_STA);
		Serial.print("Wi-Fi: connecting to ");
		Serial.println(SPINE_WIFI_SSID);
		if (strlen(SPINE_WIFI_PASSWORD) == 0) {
			WiFi.begin(SPINE_WIFI_SSID);
		} else {
			WiFi.begin(SPINE_WIFI_SSID, SPINE_WIFI_PASSWORD);
		}
	}

	void waitForWifi(unsigned long timeoutMs) {
		unsigned long started = millis();
		while (WiFi.status() != WL_CONNECTED && millis() - started < timeoutMs) {
			delay(250);
			Serial.print(".");
		}
		Serial.println();
		if (WiFi.status() == WL_CONNECTED) {
			Serial.print("Wi-Fi: connected, IP ");
			Serial.println(WiFi.localIP());
		} else {
			Serial.println("Wi-Fi: not connected; loop() will keep trying");
		}
	}

	void syncClock() {
		Serial.print("Clock: synchronizing");
		configTime(0, 0, "pool.ntp.org", "time.google.com");
		unsigned long started = millis();
		while (time(nullptr) < 1704067200 && millis() - started < 10000) {
			delay(250);
			Serial.print(".");
		}
		_clockReady = time(nullptr) >= 1704067200;
		Serial.println(_clockReady ? " ready" : " unavailable; TLS may fail");
	}

	bool connectMqtt() {
		if (!_clockReady) return false;
		Serial.print("MQTT: connecting to ");
		Serial.println(SPINE_MQTT_HOST);
		if (!_mqtt.connect(SPINE_DEVICE_ID, SPINE_MQTT_USERNAME, SPINE_MQTT_PASSWORD)) {
			Serial.print("MQTT: failed, state ");
			Serial.println(_mqtt.state());
			return false;
		}
		Serial.println("MQTT: connected");
		for (size_t i = 0; i < _subscriptionCount; ++i) {
			bool ok = _mqtt.subscribe(_subscriptions[i].topic);
			Serial.print(ok ? "subscribed " : "subscribe failed ");
			Serial.println(_subscriptions[i].topic);
		}
		return true;
	}

	bool publishValue(const char* measurement, const char* value, const char* unit, bool quoted) {
		if (!_mqtt.connected()) {
			Serial.println("not published: MQTT is not connected");
			return false;
		}
		char topic[160];
		int topicLength = snprintf(
			topic,
			sizeof(topic),
			"garden/%s/%s/%s/%s",
			SPINE_ZONE,
			SPINE_DEVICE_TYPE,
			SPINE_DEVICE_ID,
			measurement
		);
		if (topicLength < 0 || static_cast<size_t>(topicLength) >= sizeof(topic)) {
			Serial.println("not published: topic is too long");
			return false;
		}

		char payload[384];
		const char* format = quoted
			? R"({"device_id":"%s","ts":null,"value":"%s","unit":"%s","fw":"%s"})"
			: R"({"device_id":"%s","ts":null,"value":%s,"unit":"%s","fw":"%s"})";
		int payloadLength = snprintf(
			payload,
			sizeof(payload),
			format,
			SPINE_DEVICE_ID,
			value,
			unit,
			SPINE_FIRMWARE_VERSION
		);
		if (payloadLength < 0 || static_cast<size_t>(payloadLength) >= sizeof(payload)) {
			Serial.println("not published: payload is too long");
			return false;
		}
		bool sent = _mqtt.publish(topic, payload);
		Serial.print(sent ? "published " : "publish failed ");
		Serial.print(measurement);
		Serial.print(" = ");
		Serial.print(value);
		Serial.print(" ");
		Serial.println(unit);
		return sent;
	}

	static bool escapeJson(const char* input, char* output, size_t capacity) {
		size_t used = 0;
		for (const char* cursor = input; *cursor != '\0'; ++cursor) {
			const char current = *cursor;
			const char* escaped = nullptr;
			if (current == '"') escaped = "\\\"";
			if (current == '\\') escaped = "\\\\";
			if (current == '\n') escaped = "\\n";
			if (current == '\r') escaped = "\\r";
			if (current == '\t') escaped = "\\t";
			if (static_cast<unsigned char>(current) < 0x20 && escaped == nullptr) return false;
			if (escaped != nullptr) {
				if (used + 2 >= capacity) return false;
				output[used++] = escaped[0];
				output[used++] = escaped[1];
			} else {
				if (used + 1 >= capacity) return false;
				output[used++] = current;
			}
		}
		output[used] = '\0';
		return true;
	}

	static void dispatchMessage(char* topic, byte* payload, unsigned int length) {
		GardenSpine* instance = activeInstance();
		if (instance == nullptr) return;
		size_t safeLength = length;
		if (safeLength >= sizeof(instance->_incomingPayload)) {
			safeLength = sizeof(instance->_incomingPayload) - 1;
		}
		memcpy(instance->_incomingPayload, payload, safeLength);
		instance->_incomingPayload[safeLength] = '\0';
		for (size_t i = 0; i < instance->_subscriptionCount; ++i) {
			Subscription& subscription = instance->_subscriptions[i];
			if (strcmp(subscription.topic, topic) == 0 && subscription.handler != nullptr) {
				subscription.handler(topic, instance->_incomingPayload);
			}
		}
	}
};
