"""Laptop publisher for the Garden Spine.

Same topic grammar and payload contract as GardenSpine.h, so a team without hardware can
reach the dashboard on day one. See the starter README, "No board yet?".
"""

import json
import ssl

import paho.mqtt.client as mqtt


class GardenSpineLaptop:
	def __init__(self, config):
		self.config = config
		self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=config.DEVICE_ID)
		self.client.username_pw_set(config.MQTT_USERNAME, config.MQTT_PASSWORD)
		self.client.tls_set(ca_certs=config.CA_FILE, cert_reqs=ssl.CERT_REQUIRED)

	def begin(self):
		self.client.connect(self.config.MQTT_HOST, self.config.MQTT_PORT)
		self.client.loop_start()

	def publish(self, measurement, value, unit):
		"""Publish one measurement. The broker stamps the time, so ts stays null."""
		topic = f"garden/{self.config.ZONE}/{self.config.DEVICE_TYPE}/{self.config.DEVICE_ID}/{measurement}"
		payload = {
			"device_id": self.config.DEVICE_ID,
			"ts": None,
			"value": value,
			"unit": unit,
			"fw": self.config.FIRMWARE_VERSION,
		}
		return self.client.publish(topic, json.dumps(payload, separators=(",", ":")))
