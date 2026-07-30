# Build log

Update this twice each week. Write what you observed, not what you intended. A limitation you
found and wrote down is worth more than a claim that everything works.

**Evidence** means something a reader can check: a dashboard link, pasted serial output, a number
you measured. "It works" is not evidence.

<details>
<summary>Example of a useful entry</summary>

### What works

- `gm-01` publishes temperature and humidity every 60 s and has stayed alive for 3 days.
- The DHT11 responds to breath within about 20 s.

### What is broken or uncertain

- Temperature reads 1.8 °C above the lab thermometer. We think the ESP32 regulator warms it.
- Humidity above 90 %RH stops changing. We do not know yet whether that is the sensor's ceiling.

### Evidence

- Dashboard link: https://gardenspine.ikapo.fi/dev/gm-01
- Serial output: `published temperature = 23.40 celsius`
- Measurement or test: 30 min beside a reference thermometer, 10 paired readings, mean offset
  +1.8 °C, spread ±0.3 °C.

### Next action

- Move the DHT11 onto a 15 cm lead and repeat the same 30 min comparison.

</details>

---

## Week 1

### What works

-

### What is broken or uncertain

-

### Evidence

- Dashboard link:
- Serial output:
- Measurement or test:

### Next action

-

## Week 2

### What works

-

### What is broken or uncertain

-

### Evidence

- Dashboard link:
- Serial output:
- Measurement or test:

### Next action

-

## Week 3

### What works

-

### Known limitations

-

### Final evidence

- Dashboard link:
- Serial output:
- Measurement or test:
