#ifndef ROUTINES
#define ROUtINES

#include "const.h"

void sendIR_NEC(uint32_t data, uint8_t sendCount) {
  if (sendCount == 0) return; // sécurité : si 0, on n'envoie rien
  // Dans IRremote, on doit fournir "numberOfRepeats" = répétitions après le 1er envoi.
  // Exemple: sendCount=1 -> repeats=0 ; sendCount=3 -> repeats=2
  uint8_t repeats = sendCount - 1;
  IrSender.sendNECRaw(data,repeats);
}

void sendIR_PulseDistance(uint32_t data, uint8_t sendCount) {
  if (sendCount == 0) return; // sécurité : si 0, on n'envoie rien

  // Dans IRremote, on doit fournir "numberOfRepeats" = répétitions après le 1er envoi.
  // Exemple: sendCount=1 -> repeats=0 ; sendCount=3 -> repeats=2
  uint8_t repeats = sendCount - 1;

  IrSender.sendPulseDistanceWidth(
    IR_KHZ,                         // fréquence porteuse (kHz)
    IR_HDR_MARK, IR_HDR_SPACE,      // header mark/space (µs)
    IR_ONE_MARK, IR_ONE_SPACE,      // définition du bit 1 (µs)
    IR_ZERO_MARK, IR_ZERO_SPACE,    // définition du bit 0 (µs)
    data,                           // donnée à envoyer
    IR_BITS,                        // nombre de bits (24)
    IR_FLAGS,                       // ordre des bits (LSB first)
    IR_REPEAT_PERIOD_MS,            // période entre envois (ms)
    repeats                         // répétitions après le 1er envoi
  );

}

bool sendSamsungLGFromRaw(uint32_t raw, int_fast8_t repeats = 0) {
    // Format attendu:
    // [31:24] = ~command
    // [23:16] = command
    // [15:0]  = address
    uint16_t address = raw & 0xFFFF;
    uint8_t command  = (raw >> 16) & 0xFF;
    uint8_t invCmd   = (raw >> 24) & 0xFF;

    // Vérification simple de cohérence
    if ((uint8_t)~command != invCmd) {
        return false; // raw invalide
    }

    IrSender.sendSamsungLG(address, command, repeats);
    return true;
}



// =========================================================
// Encoder
// =========================================================
int8_t readEncoderStep() {
  int a = digitalRead(ENC_CLK_PIN);
  int b = digitalRead(ENC_DT_PIN);
  int currentState = (a << 1) | b;

  if (currentState != lastState) {
    int index = (lastState << 2) | currentState;
    accum += transitionTable[index];
    lastState = currentState;

    if (accum >= 4) {
      accum = 0;
      return 1;
    } else if (accum <= -4) {
      accum = 0;
      return -1;
    }
  }

  return 0;
}

bool buttonPressed(uint8_t pin, bool &lastStateRef, unsigned long &lastTimeRef) {
  bool state = digitalRead(pin);
  bool pressed = false;

  if (lastStateRef == HIGH && state == LOW && millis() - lastTimeRef > debounceMs) {
    pressed = true;
    lastTimeRef = millis();
  }

  lastStateRef = state;
  return pressed;
}

// =========================================================
// Actions per model
// =========================================================
ActionItem* getActionListForModel(int model, uint8_t &count) {
  switch (model) {
    case 0: // LG
    case 1: //TCL
    case 2: //MAG
    case 3: // SENSOR
    case 5: //SKYWORTH
      count = sizeof(commonActions) / sizeof(commonActions[0]);
      return commonActions;

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

    case 1: // TCL -> PulseDistance
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
        //case 12: sendIR_PulseDistance(TCL_CODE.BUTTON_123, SEND_REPEAT); break;
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

    case 5: // SKYWORTH
      switch (action) {
        case 0: sendSamsungLGFromRaw(SKYWORTH_CODE.POWER, SEND_REPEAT); break;
        case 1: sendSamsungLGFromRaw(SKYWORTH_CODE.SOURCE, SEND_REPEAT); break;
        case 2: sendSamsungLGFromRaw(SKYWORTH_CODE.Mute, SEND_REPEAT); break;
        case 3: sendSamsungLGFromRaw(SKYWORTH_CODE.UP, SEND_REPEAT); break;
        case 4: sendSamsungLGFromRaw(SKYWORTH_CODE.DOWN, SEND_REPEAT); break;
        case 5: sendSamsungLGFromRaw(SKYWORTH_CODE.OK, SEND_REPEAT); break;
        case 6: sendSamsungLGFromRaw(SKYWORTH_CODE.VOL_UP, SEND_REPEAT); break;
        case 7: sendSamsungLGFromRaw(SKYWORTH_CODE.VOL_DOWN, SEND_REPEAT); break;
        case 8: sendSamsungLGFromRaw(SKYWORTH_CODE.HOME, SEND_REPEAT); break;
        case 9: sendSamsungLGFromRaw(SKYWORTH_CODE.LEFT, SEND_REPEAT); break;
        case 10: sendSamsungLGFromRaw(SKYWORTH_CODE.RIGHT, SEND_REPEAT); break;
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
        display.drawStr(2, y, ">");
        display.drawStr(16, y, items[idx]);
        display.setDrawColor(1);
      } else {
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
  static const char* brandNames[BRAND_COUNT] = {"LG", "TCL", "MAG", "SENSOR", "BT_SPK","SKYWORTH"};

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

// =========================================================
// Navigation
// =========================================================
void handleEncoderNavigation() {
  int8_t step = readEncoderStep();
  if (step == 0) return;

  if (currentScreen == SCREEN_MODEL_SELECT) {
    int oldIndex = modelIndex;
    modelIndex += step;

    if (modelIndex < 0) modelIndex = 0;
    if (modelIndex >= BRAND_COUNT) modelIndex = BRAND_COUNT - 1;

    if (modelIndex != oldIndex) {
      screenDirty = true;
    }

  } else if (currentScreen == SCREEN_OPTION_SELECT) {
    uint8_t actionCount = 0;
    getActionListForModel(modelIndex, actionCount);

    int oldIndex = optionIndex;
    optionIndex += step;

    if (optionIndex < 0) optionIndex = 0;
    if (optionIndex >= actionCount) optionIndex = actionCount - 1;

    if (optionIndex != oldIndex) {
      screenDirty = true;
    }
  }
}

void handleButtons() {
  if (buttonPressed(ENC_SW_PIN, lastSwState, lastSwTime)) {
    if (currentScreen == SCREEN_MODEL_SELECT) {
      optionIndex = 0;
      currentScreen = SCREEN_OPTION_SELECT;
      screenDirty = true;

    } else if (currentScreen == SCREEN_OPTION_SELECT) {
      sendSelectedIR(modelIndex, optionIndex);

      currentScreen = SCREEN_SEND_CONFIRM;
      messageUntil = millis() + 1200;
      screenDirty = true;

    } else if (currentScreen == SCREEN_SEND_CONFIRM) {
      currentScreen = SCREEN_OPTION_SELECT;
      screenDirty = true;
    }
  }

  if (buttonPressed(BACK_BTN_PIN, lastBackState, lastBackTime)) {
    if (currentScreen == SCREEN_OPTION_SELECT) {
      currentScreen = SCREEN_MODEL_SELECT;
      screenDirty = true;

    } else if (currentScreen == SCREEN_SEND_CONFIRM) {
      currentScreen = SCREEN_OPTION_SELECT;
      screenDirty = true;
    }
  }
}



#endif
