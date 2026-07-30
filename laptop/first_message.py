"""Tutorial 01 without hardware.  https://gardenspine.ikapo.fi/tutorial/01

Success: your device shows a green "alive" badge on the developer view.

    python -m pip install paho-mqtt
    cp config.py.example config.py     # then fill in your credentials
    python first_message.py
"""

import config
from GardenSpineLaptop import GardenSpineLaptop

spine = GardenSpineLaptop(config)
spine.begin()
spine.publish("temperature", 22.5, "celsius").wait_for_publish()
print("published temperature = 22.5 celsius")
