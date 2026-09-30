#include <EEPROM.h>

const int latchButtonPin = 2;
const int resetButtonPin = 3;
const int outputPin = 8;
const int masterEnablePin = 4;
const int redPin = 9;
const int greenPin = 6;
const int bluePin = 5;

const int COLOR_ADDRESS = 0;
const int BRIGHTNESS_ADDRESS = 1;

int currentLButtonState;
int lastLButtonState = HIGH;
unsigned long outputPulseEndTime = 0;
bool outputIsActive = false;

int currentRButtonState;
int lastRButtonState = LOW;
unsigned long resetButtonPressStartTime = 0;
bool resetButtonLongPressActivated = false;
const unsigned long longPressThreshold = 1000;

unsigned long lastBrightnessCycleTime = 0;
const unsigned long brightnessCycleInterval = 500;

const int colors[][3] = {
  { 255, 0, 0 },      // Red
  { 0, 255, 0 },      // Green
  { 0, 0, 255 },      // Blue
  { 255, 255, 0 },    // Yellow
  { 0, 255, 255 },    // Cyan
  { 255, 0, 255 },    // Magenta
  { 255, 255, 255 },  // White
  { 255, 40, 0 },     // Gamecube Orange
  { 1, 1, 1 }         // Rainbow
};
const int numColors = sizeof(colors) / sizeof(colors[0]);
int currentColorIndex = 0;

const int brightnessLevels[] = { 0, 30, 80, 150, 255 };
const int numBrightnessLevels = sizeof(brightnessLevels) / sizeof(brightnessLevels[0]);
int currentBrightnessIndex = (numBrightnessLevels - 1);

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(latchButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(masterEnablePin, INPUT_PULLUP);

  setRGBColor(0, 0, 0); 
  digitalWrite(outputPin, HIGH);

  byte savedColorIndex = EEPROM.read(COLOR_ADDRESS);
  currentColorIndex = (savedColorIndex < numColors) ? savedColorIndex : 0;

  byte savedBrightnessIndex = EEPROM.read(BRIGHTNESS_ADDRESS);
  currentBrightnessIndex = (savedBrightnessIndex < numBrightnessLevels) ? savedBrightnessIndex : (numBrightnessLevels - 1);

  currentLButtonState = digitalRead(latchButtonPin);
  lastLButtonState = currentLButtonState;
}

void loop() {
  handleLatchButton();
  handleResetButton();
  updateLEDs();
}

void handleLatchButton() {
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50; 
  int reading = digitalRead(latchButtonPin);

  if (reading != lastLButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentLButtonState) {
      currentLButtonState = reading;
      outputIsActive = true;
      digitalWrite(outputPin, LOW); 
      
      if (currentLButtonState == LOW) {
        outputPulseEndTime = millis() + 500;
      } else {
        outputPulseEndTime = millis() + 4000;
      }
    }
  }
  lastLButtonState = reading;

  if (outputIsActive && millis() >= outputPulseEndTime) {
    digitalWrite(outputPin, HIGH);
    outputIsActive = false;
  }
}

void handleResetButton() {
  currentRButtonState = digitalRead(resetButtonPin);

  if (currentRButtonState == HIGH && lastRButtonState == LOW) {
    resetButtonPressStartTime = millis();
    resetButtonLongPressActivated = false;
    lastBrightnessCycleTime = millis();
  }

  if (currentRButtonState == LOW && lastRButtonState == HIGH) {
    if (resetButtonLongPressActivated) {
      EEPROM.update(BRIGHTNESS_ADDRESS, currentBrightnessIndex);
    } else {
      unsigned long pressDuration = millis() - resetButtonPressStartTime;
      if (pressDuration > 50 && pressDuration < longPressThreshold) {
        currentColorIndex = (currentColorIndex + 1) % numColors;
        EEPROM.update(COLOR_ADDRESS, currentColorIndex);
      }
    }
    resetButtonPressStartTime = 0;
    resetButtonLongPressActivated = false;
  }

  if (currentRButtonState == HIGH) {
    if (!resetButtonLongPressActivated) {
      if (millis() - resetButtonPressStartTime >= longPressThreshold) {
        resetButtonLongPressActivated = true;
      }
    } else {
      if (millis() - lastBrightnessCycleTime >= brightnessCycleInterval) {
        currentBrightnessIndex = (currentBrightnessIndex + 1) % numBrightnessLevels;
        lastBrightnessCycleTime = millis();
      }
    }
  }
  lastRButtonState = currentRButtonState;
}

void updateLEDs() {
  int masterEnableState = digitalRead(masterEnablePin);
  
  if (masterEnableState == LOW && currentLButtonState == LOW) { 
    if (currentColorIndex == numColors - 1) {
      // 3.0 Second smooth cycle
      float hue = (float)(millis() % 3000) / 3000.0;
      showRainbow(hue);
    } else {
      setRGBColor(colors[currentColorIndex][0], colors[currentColorIndex][1], colors[currentColorIndex][2]);
    }
  } else { 
    setRGBColor(0, 0, 0);
  }
}

void showRainbow(float h) {
  float r, g, b;
  int i = floor(h * 6);
  float f = h * 6 - i;
  float q = 1 - f;
  float t = f;
  
  switch(i % 6){
    case 0: r = 1; g = t; b = 0; break;
    case 1: r = q; g = 1; b = 0; break;
    case 2: r = 0; g = 1; b = t; break;
    case 3: r = 0; g = q; b = 1; break;
    case 4: r = t; g = 0; b = 1; break;
    case 5: r = 1; g = 0; b = q; break;
  }
  setRGBColor(r * 255, g * 255, b * 255);
}

void setRGBColor(int redValue, int greenValue, int blueValue) {
  int actualBrightnessLevel = brightnessLevels[currentBrightnessIndex];
  
  int scaledRed   = (redValue > 0)   ? map(redValue, 0, 255, 255, 255 - actualBrightnessLevel) : 255;
  int scaledGreen = (greenValue > 0) ? map(greenValue, 0, 255, 255, 255 - actualBrightnessLevel) : 255;
  int scaledBlue  = (blueValue > 0)  ? map(blueValue, 0, 255, 255, 255 - actualBrightnessLevel) : 255;

  analogWrite(redPin, scaledRed);
  analogWrite(greenPin, scaledGreen);
  analogWrite(bluePin, scaledBlue);
}