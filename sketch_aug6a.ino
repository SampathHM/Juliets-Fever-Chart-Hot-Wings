#include <ESP32Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GardenSpine.h>

const int servoPin1 = 13;
const int servoPin2 = 12;
const int dhtPin = 14;
const int buzzerPin = 27;
const int btnUpPin = 19;
const int btnDownPin = 18;

const int sdaPin = 21;
const int sclPin = 22;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float BASE_TEMP = 24.0;
const int CENTER = 90;
const int SWING  = 20;
const int HALF_TIME = 100;

Servo servo1;
Servo servo2;
DHT dht(dhtPin, DHT11);
GardenSpine spine;

unsigned long lastReadTime = 0;
unsigned long lastAnimTime = 0;
unsigned long lastServoTime = 0;
unsigned long displaySwitchTime = 0;
unsigned long lastPublishTime = 0;
unsigned long lastBtnTime = 0;
unsigned long editBaseTempTimeout = 0;    // Timer to exit edit mode

float currentTemp = 0;
float currentHumidity = 0;
int animFrame = 0;
int servoState = 0;
int errorCount = 0;
bool validReading = false;
bool wasHot = false;
bool wasCool = true;
bool showFace = true;
bool editingBaseTemp = false;             // Are we changing the base temp?

const int ANIM_INTERVAL = 200;
const int TOTAL_FRAMES = 25;
const int DISPLAY_INTERVAL = 2000;
const unsigned long PUBLISH_INTERVAL = 60000;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  
  pinMode(btnUpPin, INPUT_PULLUP);
  pinMode(btnDownPin, INPUT_PULLUP);

  Wire.begin(sdaPin, sclPin);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Starting..");
  display.display();

  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo1.attach(servoPin1, 500, 2400);
  servo2.attach(servoPin2, 500, 2400);

  dht.begin();
  delay(2000);

  servo1.write(CENTER);
  servo2.write(CENTER);

  spine.begin();

  for (int i = 0; i < 5; i++) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (!isnan(temp)) {
      currentTemp = temp;
      if (!isnan(hum)) currentHumidity = hum;
      validReading = true;
      Serial.print("Initial Temp: ");
      Serial.println(currentTemp);
      break;
    }
    delay(2000);
  }

  displaySwitchTime = millis() + DISPLAY_INTERVAL;
  lastPublishTime = millis();
}

// ================== SIMPLE BEEP ==================
void tripleBeep() {
  for (int b = 0; b < 3; b++) {
    for (int i = 0; i < 100; i++) {
      digitalWrite(buzzerPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(buzzerPin, LOW);
      delayMicroseconds(500);
    }
    delay(150);
  }
  digitalWrite(buzzerPin, LOW);
}

// ================== BUTTON HANDLER ==================
void checkButtons() {
  if (millis() - lastBtnTime < 300) return;   // Debounce

  bool isUp = digitalRead(btnUpPin) == LOW;
  bool isDown = digitalRead(btnDownPin) == LOW;

  if (isUp || isDown) {
    lastBtnTime = millis();
    editBaseTempTimeout = millis() + 3000;  // Reset 3-second timeout

    if (!editingBaseTemp) {
      // FIRST PRESS: Just enter edit mode, show current value
      editingBaseTemp = true;
      Serial.print("Showing Base Temp: ");
      Serial.println(BASE_TEMP);
    } else {
      // SUBSEQUENT PRESSES: Change the value
      if (isUp) {
        BASE_TEMP += 0.5;
        Serial.print("Base temp UP: ");
      } else if (isDown) {
        BASE_TEMP -= 0.5;
        if (BASE_TEMP < 10.0) BASE_TEMP = 10.0;
        Serial.print("Base temp DOWN: ");
      }
      Serial.println(BASE_TEMP);
    }

    // Update OLED with base temp
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(30, 5);
    display.println("BASE TEMP");
    
    display.setTextSize(3);
    display.setCursor(25, 25);
    display.print(BASE_TEMP, 1);
    display.setTextSize(2);
    display.print(" C");
    
    display.display();
  }
}

// ================== DHT READ FUNCTION ==================
void readDHT() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp)) {
    errorCount++;
    Serial.print("DHT read failed. Error count: ");
    Serial.println(errorCount);

    if (errorCount > 10 && !validReading) {
      display.clearDisplay();
      display.setCursor(10, 25);
      display.setTextSize(2);
      display.println("DHT Error");
      display.display();
    }
  } else {
    currentTemp = temp;
    if (!isnan(hum)) currentHumidity = hum;
    validReading = true;
    errorCount = 0;
    Serial.print("Temperature: ");
    Serial.print(currentTemp);
    Serial.print(" C  Humidity: ");
    Serial.print(currentHumidity);
    Serial.println(" %");

    if (currentTemp > BASE_TEMP && !wasHot) {
      wasHot = true;
      wasCool = false;
      Serial.println(">> Temperature went HIGH!");
      tripleBeep();
    }
    else if (currentTemp <= BASE_TEMP && !wasCool) {
      wasCool = true;
      wasHot = false;
      Serial.println(">> Temperature back to NORMAL");
      tripleBeep();
    }
  }
}

// ================== PUBLISH TO SERVER ==================
void publishData() {
  if (!validReading) return;
  spine.publish("temperature", currentTemp, "celsius");
  spine.publish("humidity", currentHumidity, "percent-rh");
  Serial.println(">> Data sent to server");
}

// ================== BIG TEMPERATURE DISPLAY ==================
void displayBigTemp(float temp) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(40, 5);
  display.println("TEMP");
  display.setTextSize(3);
  display.setCursor(5, 25);
  display.print(temp, 1);
  display.setTextSize(2);
  display.setCursor(105, 30);
  display.print("C");
  display.display();
}

// ================== HOT FACE (ANGRY) ==================
void displayHotOpen(float temp) {
  display.clearDisplay();
  display.fillCircle(32, 35, 18, SSD1306_WHITE);
  display.fillCircle(32, 38, 8, SSD1306_BLACK);
  display.fillCircle(32, 38, 4, SSD1306_WHITE);
  display.fillCircle(96, 35, 18, SSD1306_WHITE);
  display.fillCircle(96, 38, 8, SSD1306_BLACK);
  display.fillCircle(96, 38, 4, SSD1306_WHITE);
  display.fillTriangle(10, 12, 54, 18, 54, 22, SSD1306_WHITE);
  display.fillTriangle(118, 12, 74, 18, 74, 22, SSD1306_WHITE);
  display.fillRoundRect(44, 58, 40, 6, 2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("HOT! ");
  display.display();
}

void displayHotBlink(float temp) {
  display.clearDisplay();
  display.fillRoundRect(14, 33, 36, 6, 3, SSD1306_WHITE);
  display.fillRoundRect(78, 33, 36, 6, 3, SSD1306_WHITE);
  display.fillTriangle(10, 12, 54, 18, 54, 22, SSD1306_WHITE);
  display.fillTriangle(118, 12, 74, 18, 74, 22, SSD1306_WHITE);
  display.fillRoundRect(44, 58, 40, 6, 2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("HOT! ");
  display.display();
}

void displayHotHalf(float temp) {
  display.clearDisplay();
  display.fillRoundRect(14, 30, 36, 14, 5, SSD1306_WHITE);
  display.fillCircle(32, 38, 5, SSD1306_BLACK);
  display.fillRoundRect(78, 30, 36, 14, 5, SSD1306_WHITE);
  display.fillCircle(96, 38, 5, SSD1306_BLACK);
  display.fillTriangle(10, 12, 54, 18, 54, 22, SSD1306_WHITE);
  display.fillTriangle(118, 12, 74, 18, 74, 22, SSD1306_WHITE);
  display.fillRoundRect(44, 58, 40, 6, 2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("HOT! ");
  display.display();
}

// ================== COOL FACE (HAPPY) ==================
void displayCoolOpen(float temp) {
  display.clearDisplay();
  display.fillCircle(32, 30, 18, SSD1306_WHITE);
  display.fillCircle(32, 30, 10, SSD1306_BLACK);
  display.fillCircle(28, 26, 4, SSD1306_WHITE);
  display.fillCircle(96, 30, 18, SSD1306_WHITE);
  display.fillCircle(96, 30, 10, SSD1306_BLACK);
  display.fillCircle(92, 26, 4, SSD1306_WHITE);
  display.fillCircle(64, 50, 12, SSD1306_WHITE);
  display.fillRect(40, 42, 50, 12, SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Cool ");
  display.display();
}

void displayCoolBlink(float temp) {
  display.clearDisplay();
  display.fillRoundRect(14, 28, 36, 6, 3, SSD1306_WHITE);
  display.fillRoundRect(78, 28, 36, 6, 3, SSD1306_WHITE);
  display.fillCircle(64, 50, 12, SSD1306_WHITE);
  display.fillRect(40, 42, 50, 12, SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Cool ");
  display.display();
}

void displayCoolHalf(float temp) {
  display.clearDisplay();
  display.fillRoundRect(14, 26, 36, 14, 5, SSD1306_WHITE);
  display.fillCircle(32, 32, 6, SSD1306_BLACK);
  display.fillCircle(30, 30, 2, SSD1306_WHITE);
  display.fillRoundRect(78, 26, 36, 14, 5, SSD1306_WHITE);
  display.fillCircle(96, 32, 6, SSD1306_BLACK);
  display.fillCircle(94, 30, 2, SSD1306_WHITE);
  display.fillCircle(64, 50, 12, SSD1306_WHITE);
  display.fillRect(40, 42, 50, 12, SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Cool ");
  display.display();
}

// ================== ANIMATION HANDLER ==================
void animateHot(float temp) {
  if (animFrame <= 21) displayHotOpen(temp);
  else if (animFrame == 22) displayHotHalf(temp);
  else if (animFrame == 23) displayHotBlink(temp);
  else if (animFrame == 24) displayHotHalf(temp);
}

void animateCool(float temp) {
  if (animFrame <= 21) displayCoolOpen(temp);
  else if (animFrame == 22) displayCoolHalf(temp);
  else if (animFrame == 23) displayCoolBlink(temp);
  else if (animFrame == 24) displayCoolHalf(temp);
}

// ================== MAIN LOOP ==================
void loop() {

  // Check buttons first
  checkButtons();

  // Auto exit edit mode after 3 seconds of no pressing
  if (editingBaseTemp && millis() > editBaseTempTimeout) {
    editingBaseTemp = false;
  }

  // Keep MQTT connection alive
  spine.loop();

  // Send data to server every 60 seconds
  if (millis() - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = millis();
    publishData();
  }

  // Read temperature every 5 seconds
  if (millis() - lastReadTime >= 5000) {
    lastReadTime = millis();
    readDHT();
  }

  // Animation frame update
  if (millis() - lastAnimTime >= ANIM_INTERVAL) {
    lastAnimTime = millis();
    animFrame++;
    if (animFrame >= TOTAL_FRAMES) animFrame = 0;
  }

  // Display switching (only if not editing base temp)
  if (!editingBaseTemp) {
    if (millis() - displaySwitchTime >= DISPLAY_INTERVAL) {
      displaySwitchTime = millis();
      showFace = !showFace;
    }
  }

  // Update display (only if not editing base temp)
  if (validReading && !editingBaseTemp) {
    if (showFace) {
      if (currentTemp > BASE_TEMP) animateHot(currentTemp);
      else animateCool(currentTemp);
    } else {
      displayBigTemp(currentTemp);
    }
  }

  // Servo control
  if (currentTemp > BASE_TEMP) {
    if (millis() - lastServoTime >= HALF_TIME) {
      lastServoTime = millis();

      if (servoState == 0) {
        servo1.write(CENTER - SWING);
        servo2.write(CENTER + SWING);
        servoState = 1;
      } else {
        servo1.write(CENTER + SWING);
        servo2.write(CENTER - SWING);
        servoState = 0;
      }
    }
  } else {
    servo1.write(CENTER);
    servo2.write(CENTER);
    servoState = 0;
  }
}