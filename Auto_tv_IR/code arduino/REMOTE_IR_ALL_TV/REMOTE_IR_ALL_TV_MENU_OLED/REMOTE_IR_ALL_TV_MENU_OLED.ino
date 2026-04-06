#include <Arduino.h>
#include <Wire.h>
#include <IRremote.hpp>
#include <U8g2lib.h>

#include "const.h"
#include "basic_routines.h"
#define ENCODER_DEBUG
// =========================================================
// OLED 1.3" SH1106 128x64 I2C
// =========================================================
#define OLED_ADDR 0x3C
U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// ===================== PINS =====================
const uint8_t ENC_CLK_PIN  = 10;
const uint8_t ENC_DT_PIN   = 11;
const uint8_t ENC_SW_PIN   = 12;   // press to validate
const uint8_t BACK_BTN_PIN = A0;   // separate back button

int lastState = 0;
int accum = 0;

const int8_t transitionTable[16] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
 -1,  0,  0,  1,
  0,  1, -1,  0
};
// ===================== UI =====================
enum ScreenState {
  SCREEN_MODEL_SELECT,
  SCREEN_OPTION_SELECT,
  SCREEN_SEND_CONFIRM
};

ScreenState currentScreen = SCREEN_MODEL_SELECT;

struct BrandItem {
  const char* name;
};

BrandItem brands[] = {
  {"LG"},
  {"TCL"},
  {"MAG"},
  {"SENSOR"},
  {"BT_SPK"}
};

const uint8_t BRAND_COUNT = sizeof(brands) / sizeof(brands[0]);

struct ActionItem {
  const char* label;
};

ActionItem commonActions[] = {
  {"POWER"},
  {"SOURCE"},
  {"MUTE"},
  {"UP"},
  {"DOWN"},
  {"OK"},
  {"VOL+"},
  {"VOL-"},
  {"LEFT"},
  {"RIGHT"}
};

ActionItem tclActions[] = {
  {"POWER"},
  {"SOURCE"},
  {"MUTE"},
  {"UP"},
  {"DOWN"},
  {"OK"},
  {"VOL+"},
  {"VOL-"},
  {"HOME"},
  {"LEFT"},
  {"RIGHT"},
  {"BACK"},
  {"123"}
};

ActionItem magActions[] = {
  {"POWER"},
  {"SOURCE"},
  {"MUTE"},
  {"UP"},
  {"DOWN"},
  {"OK"},
  {"VOL+"},
  {"VOL-"},
  {"HOME"},
  {"LEFT"},
  {"RIGHT"}
};

ActionItem btActions[] = {
  {"POWER"},
  {"MUTE"},
  {"UP"},
  {"DOWN"},
  {"PLAY/PAUSE"},
  {"MODE"},
  {"PREV"},
  {"NEXT"},
  {"BT"},
  {"3D"},
  {"LINK"}
};

int modelIndex = 0;
int optionIndex = 0;
int lastEncodedState = 0;

bool lastSwState = HIGH;
bool lastBackState = HIGH;
unsigned long lastButtonTime = 0;
const unsigned long debounceMs = 180;

unsigned long messageUntil = 0;

// =========================================================
// Encoder
// =========================================================
int8_t readEncoderStep(){
    int a = digitalRead(ENC_CLK_PIN);
    int b = digitalRead(ENC_DT_PIN);
    int currentState = (a << 1) | b;

    if (currentState != lastState) {
      int index = (lastState << 2) | currentState;
      accum += transitionTable[index];
      lastState = currentState;

      if (accum >= 4) {
        accum = 0;
        Serial.println("1");
        return 1;
      } 
      else if (accum <= -4) {
        accum = 0;
        Serial.println("-1");
        return -1;
      }
    }
    return 0;
}
    



bool buttonPressed(uint8_t pin, bool &lastState) {
  bool state = digitalRead(pin);
  bool pressed = false;

  if (lastState == HIGH && state == LOW && millis() - lastButtonTime > debounceMs) {
    pressed = true;
    lastButtonTime = millis();
  }

  lastState = state;
  return pressed;
}

// =========================================================
// Actions per model
// =========================================================
ActionItem* getActionListForModel(int model, uint8_t &count) {
  switch (model) {
    case 0: // LG
    case 3: // SENSOR
      count = sizeof(commonActions) / sizeof(commonActions[0]);
      return commonActions;
    case 1: // TCL
      count = sizeof(tclActions) / sizeof(tclActions[0]);
      return tclActions;
    case 2: // MAG
      count = sizeof(magActions) / sizeof(magActions[0]);
      return magActions;
    case 4: // BT speaker
      count = sizeof(btActions) / sizeof(btActions[0]);
      return btActions;
    default:
      count = sizeof(commonActions) / sizeof(commonActions[0]);
      return commonActions;
  }
}

void sendSelectedIR(int model, int action) {
  switch (model) {
    case 0: // LG -> NEC
      switch (action) {
        case 0: sendIR_NEC(LG_CODE.POWER, SEND_REPEAT); break;
        case 1: sendIR_NEC(LG_CODE.SOURCE, SEND_REPEAT); break;
        case 2: sendIR_NEC(LG_CODE.Mute, SEND_REPEAT); break;
        case 3: sendIR_NEC(LG_CODE.UP, SEND_REPEAT); break;
        case 4: sendIR_NEC(LG_CODE.DOWN, SEND_REPEAT); break;
        case 5: sendIR_NEC(LG_CODE.OK, SEND_REPEAT); break;
        case 6: sendIR_NEC(LG_CODE.VOL_UP, SEND_REPEAT); break;
        case 7: sendIR_NEC(LG_CODE.VOL_DOWN, SEND_REPEAT); break;
        case 8: sendIR_NEC(LG_CODE.LEFT, SEND_REPEAT); break;
        case 9: sendIR_NEC(LG_CODE.RIGHT, SEND_REPEAT); break;
      }
      break;

    case 1: // TCL -> PulseDistance 24 bits
      switch (action) {
        case 0:  sendIR_PulseDistance(TCL_CODE.POWER, SEND_REPEAT); break;
        case 1:  sendIR_PulseDistance(TCL_CODE.SOURCE, SEND_REPEAT); break;
        case 2:  sendIR_PulseDistance(TCL_CODE.Mute, SEND_REPEAT); break;
        case 3:  sendIR_PulseDistance(TCL_CODE.UP, SEND_REPEAT); break;
        case 4:  sendIR_PulseDistance(TCL_CODE.DOWN, SEND_REPEAT); break;
        case 5:  sendIR_PulseDistance(TCL_CODE.OK, SEND_REPEAT); break;
        case 6:  sendIR_PulseDistance(TCL_CODE.VOL_UP, SEND_REPEAT); break;
        case 7:  sendIR_PulseDistance(TCL_CODE.VOL_DOWN, SEND_REPEAT); break;
        case 8:  sendIR_PulseDistance(TCL_CODE.HOME, SEND_REPEAT); break;
        case 9:  sendIR_PulseDistance(TCL_CODE.LEFT, SEND_REPEAT); break;
        case 10: sendIR_PulseDistance(TCL_CODE.RIGHT, SEND_REPEAT); break;
        case 11: sendIR_PulseDistance(TCL_CODE.Back, SEND_REPEAT); break;
        case 12: sendIR_PulseDistance(TCL_CODE.BUTTON_123, SEND_REPEAT); break;
      }
      break;

    case 2: // MAG -> NEC
      switch (action) {
        case 0: sendIR_NEC(MAG_CODE.POWER, SEND_REPEAT); break;
        case 1: sendIR_NEC(MAG_CODE.SOURCE, SEND_REPEAT); break;
        case 2: sendIR_NEC(MAG_CODE.Mute, SEND_REPEAT); break;
        case 3: sendIR_NEC(MAG_CODE.UP, SEND_REPEAT); break;
        case 4: sendIR_NEC(MAG_CODE.DOWN, SEND_REPEAT); break;
        case 5: sendIR_NEC(MAG_CODE.OK, SEND_REPEAT); break;
        case 6: sendIR_NEC(MAG_CODE.VOL_UP, SEND_REPEAT); break;
        case 7: sendIR_NEC(MAG_CODE.VOL_DOWN, SEND_REPEAT); break;
        case 8: sendIR_NEC(MAG_CODE.HOME, SEND_REPEAT); break;
        case 9: sendIR_NEC(MAG_CODE.LEFT, SEND_REPEAT); break;
        case 10: sendIR_NEC(MAG_CODE.RIGHT, SEND_REPEAT); break;
      }
      break;

    case 3: // SENSOR -> NEC
      switch (action) {
        case 0: sendIR_NEC(SENSOR_CODE.POWER, SEND_REPEAT); break;
        case 1: sendIR_NEC(SENSOR_CODE.SOURCE, SEND_REPEAT); break;
        case 2: sendIR_NEC(SENSOR_CODE.Mute, SEND_REPEAT); break;
        case 3: sendIR_NEC(SENSOR_CODE.UP, SEND_REPEAT); break;
        case 4: sendIR_NEC(SENSOR_CODE.DOWN, SEND_REPEAT); break;
        case 5: sendIR_NEC(SENSOR_CODE.OK, SEND_REPEAT); break;
        case 6: sendIR_NEC(SENSOR_CODE.VOL_UP, SEND_REPEAT); break;
        case 7: sendIR_NEC(SENSOR_CODE.VOL_DOWN, SEND_REPEAT); break;
        case 8: sendIR_NEC(SENSOR_CODE.LEFT, SEND_REPEAT); break;
        case 9: sendIR_NEC(SENSOR_CODE.RIGHT, SEND_REPEAT); break;
      }
      break;

    case 4: // BT speaker -> NEC
      switch (action) {
        case 0: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.POWER, SEND_REPEAT); break;
        case 1: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.Mute, SEND_REPEAT); break;
        case 2: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.UP, SEND_REPEAT); break;
        case 3: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.DOWN, SEND_REPEAT); break;
        case 4: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.PLAY_PAUSE, SEND_REPEAT); break;
        case 5: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.MODE, SEND_REPEAT); break;
        case 6: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.PREVIOUS, SEND_REPEAT); break;
        case 7: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.NEXT, SEND_REPEAT); break;
        case 8: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.BT, SEND_REPEAT); break;
        case 9: sendIR_NEC(PURE_ACOUSTICS_BT_CODE._3D, SEND_REPEAT); break;
        case 10: sendIR_NEC(PURE_ACOUSTICS_BT_CODE.LINK, SEND_REPEAT); break;
      }
      break;
  }
}

// =========================================================
// Display helpers
// =========================================================
void drawHeader(const char* title) {
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(0, 8, title);
  display.drawHLine(0, 10, 128);
}

void drawMenuList(const char* title, const char* const items[], uint8_t count, int selected) {
  display.firstPage();
  do {
    drawHeader(title);
    display.setFont(u8g2_font_6x10_tr);

    int firstVisible = 0;
    if (selected >= 4) {
      firstVisible = selected - 3;
    }

    for (int i = 0; i < 4; i++) {
      int idx = firstVisible + i;
      if (idx >= count) break;

      int y = 20 + i * 11;

      if (idx == selected) {
        display.setDrawColor(1);
        display.drawBox(0, y - 8, 128, 10);
        display.setDrawColor(0);
        display.drawStr(2, y, "> ");
        display.drawStr(16, y, items[idx]);
        display.setDrawColor(1);
      } else {
        display.drawStr(2, y, "  ");
        display.drawStr(16, y, items[idx]);
      }
    }
  } while (display.nextPage());
}

void renderConfirmScreen() {
  uint8_t actionCount = 0;
  ActionItem* actions = getActionListForModel(modelIndex, actionCount);

  display.firstPage();
  do {
    drawHeader("Sent");
    display.setFont(u8g2_font_6x10_tr);

    display.drawStr(0, 22, "Model:");
    display.drawStr(42, 22, brands[modelIndex].name);

    display.drawStr(0, 36, "Action:");
    display.drawStr(42, 36, actions[optionIndex].label);

    display.drawStr(0, 54, "Back=return");
  } while (display.nextPage());
}

void renderScreen() {
  static const char* brandNames[BRAND_COUNT] = {"LG", "TCL", "MAG", "SENSOR", "BT_SPK"};

  uint8_t actionCount = 0;
  ActionItem* actions = getActionListForModel(modelIndex, actionCount);

  if (currentScreen == SCREEN_MODEL_SELECT) {
    drawMenuList("Choose model", brandNames, BRAND_COUNT, modelIndex);
    return;
  }

  const char* actionLabels[16];
  for (uint8_t i = 0; i < actionCount; i++) {
    actionLabels[i] = actions[i].label;
  }

  if (currentScreen == SCREEN_OPTION_SELECT) {
    drawMenuList(brands[modelIndex].name, actionLabels, actionCount, optionIndex);
    return;
  }

  if (currentScreen == SCREEN_SEND_CONFIRM) {
    renderConfirmScreen();
  }
}

void handleEncoderNavigation() {
  int8_t step = readEncoderStep();
  if (step == 0) return;

  if (currentScreen == SCREEN_MODEL_SELECT) {
    modelIndex += step;
    if (modelIndex < 0) modelIndex = BRAND_COUNT - 1;
    if (modelIndex >= BRAND_COUNT) modelIndex = 0;

    //if (modelIndex < 0) modelIndex = 0;
    //if (modelIndex >= BRAND_COUNT) modelIndex = BRAND_COUNT;

  } else if (currentScreen == SCREEN_OPTION_SELECT) {
    uint8_t actionCount = 0;
    getActionListForModel(modelIndex, actionCount);
    optionIndex += step;
    if (optionIndex < 0) optionIndex = actionCount - 1;
    if (optionIndex >= actionCount) optionIndex = 0;

    // if (optionIndex < 0) optionIndex = 0;
     //if (optionIndex >= actionCount) optionIndex = actionCount;
  }
}

void handleButtons() {
  if (buttonPressed(ENC_SW_PIN, lastSwState)) {
    if (currentScreen == SCREEN_MODEL_SELECT) {
      optionIndex = 0;
      currentScreen = SCREEN_OPTION_SELECT;
    } else if (currentScreen == SCREEN_OPTION_SELECT) {
      sendSelectedIR(modelIndex, optionIndex);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(40);
      digitalWrite(LED_BUILTIN, LOW);
      currentScreen = SCREEN_SEND_CONFIRM;
      messageUntil = millis() + 1200;
    } else if (currentScreen == SCREEN_SEND_CONFIRM) {
      currentScreen = SCREEN_OPTION_SELECT;
    }
  }

  if (buttonPressed(BACK_BTN_PIN, lastBackState)) {
    if (currentScreen == SCREEN_OPTION_SELECT) {
      currentScreen = SCREEN_MODEL_SELECT;
    } else if (currentScreen == SCREEN_SEND_CONFIRM) {
      currentScreen = SCREEN_OPTION_SELECT;
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENC_CLK_PIN, INPUT_PULLUP);
  pinMode(ENC_DT_PIN, INPUT_PULLUP);
  pinMode(ENC_SW_PIN, INPUT_PULLUP);
  pinMode(BACK_BTN_PIN, INPUT_PULLUP);

  lastEncodedState = (digitalRead(ENC_CLK_PIN) << 1) | digitalRead(ENC_DT_PIN);

  IrSender.begin(IR_SEND_PIN);
  Serial.begin(115200);
  Wire.begin();
  display.setI2CAddress(OLED_ADDR << 1);
  display.begin();

  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(0, 12, "Remote IR init...");
  } while (display.nextPage());
  delay(600);
  Serial.println("init");
}

void loop() {
  handleEncoderNavigation();
  handleButtons();

  if (currentScreen == SCREEN_SEND_CONFIRM && millis() > messageUntil) {
    currentScreen = SCREEN_OPTION_SELECT;
  }

  renderScreen();
  delay(5);
}
