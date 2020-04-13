#include <Adafruit_CircuitPlayground.h>
#include "colors.h"
#include "pitches.h"
#include "touch.h"
#include "button.h"
#define BUTTON_COUNT 4
#define LEVEL_COUNT 10
#define LIGHT_DELAY 250

int currentLevel = 1;
bool failCondition = false;
Button* selectedButtons[LEVEL_COUNT] = { NULL };
Button buttons[BUTTON_COUNT] = {
  {A4, {0, 1}, GREEN, NOTE_G4},
  {A7, {3, 4}, RED, NOTE_E4},
  {A1, {5, 6}, YELLOW, NOTE_C4},
  {A3, {8, 9}, BLUE, NOTE_G3}
};

void setAllLights(int32_t color) {
  for (int lightIndex = 0; lightIndex < 10; lightIndex++) {
    CircuitPlayground.setPixelColor(lightIndex, color);
  }
  delay(LIGHT_DELAY);
}

void activateButton(Button button) {
  CircuitPlayground.clearPixels();
  delay(LIGHT_DELAY);
  for (int pixelIndex = 0, pixelCount = sizeof(button.pixelNumbers); pixelIndex < pixelCount; pixelIndex++) {
    CircuitPlayground.setPixelColor(button.pixelNumbers[pixelIndex], button.color);
  }

  CircuitPlayground.playTone(button.noteFrequency, NOTE_DURATION);
  delay(DEBOUNCE);
  CircuitPlayground.clearPixels();
  delay(LIGHT_DELAY);
}

void activateButtons(Button* buttons[], int maxButtons) {
  for (int index = 0; index < maxButtons; index++) {
    if (&buttons[index]) {
      activateButton(*buttons[index]);
    }
  }
}

bool getInput(Button* selectedButtons[], int maxButtons) {
  for (int selectedButtonIndex = 0; selectedButtonIndex < maxButtons; selectedButtonIndex++) {
    Button* pressedButton = NULL;

    while (!pressedButton) {
      for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++) {
        if (isButtonPressed(buttons[buttonIndex])) {
          pressedButton = &buttons[buttonIndex];
          activateButton(buttons[buttonIndex]);
          break;
        }
      }
    }

    if (selectedButtons[selectedButtonIndex] != pressedButton) {
      return false;
    }
  }
  return true;
}

boolean isButtonPressed(Button button) {
  return CircuitPlayground.readCap(button.capacitiveSensor) > CAP_THRESHOLD;
}

void setup() {
  int seed = 0;

  seed += analogRead(12);
  seed += analogRead(7);
  seed += analogRead(9);
  seed += analogRead(10);

  randomSeed(seed);
  
  for (int selectionIndex = 0; selectionIndex < LEVEL_COUNT; selectionIndex++) {
    selectedButtons[selectionIndex] = &buttons[random(BUTTON_COUNT)];
  }

  CircuitPlayground.begin();
  setAllLights(WHITE);
  CircuitPlayground.clearPixels();
}

void loop() {
  if (failCondition) {
    setAllLights(RED);
  } else if (currentLevel <= LEVEL_COUNT) {
    activateButtons(selectedButtons, currentLevel);
    if (getInput(selectedButtons, currentLevel)) {
      currentLevel++;
    } else {
      failCondition = true;
    }
  } else {
    setAllLights(GREEN);
  }
}
