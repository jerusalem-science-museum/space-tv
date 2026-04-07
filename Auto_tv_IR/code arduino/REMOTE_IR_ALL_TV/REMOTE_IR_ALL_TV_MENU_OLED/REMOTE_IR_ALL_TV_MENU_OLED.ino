//#include <Arduino.h>
#include <Wire.h>
#include <IRremote.hpp>
#include <U8g2lib.h>

#include "const.h"
#include "basic_routines.h"


// =========================================================
// Setup / Loop
// =========================================================
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENC_CLK_PIN, INPUT_PULLUP);
  pinMode(ENC_DT_PIN, INPUT_PULLUP);
  pinMode(ENC_SW_PIN, INPUT_PULLUP);
  pinMode(BACK_BTN_PIN, INPUT_PULLUP);

  lastState = (digitalRead(ENC_CLK_PIN) << 1) | digitalRead(ENC_DT_PIN);

  IrSender.begin(IR_SEND_PIN);

  Wire.begin();
  display.setI2CAddress(OLED_ADDR << 1);
  display.begin();

  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(0, 12, "Remote IR init...");
  } while (display.nextPage());

  delay(400);
  screenDirty = true;
}

void loop() {
  handleEncoderNavigation();
  handleButtons();

  if (currentScreen == SCREEN_SEND_CONFIRM && millis() > messageUntil) {
    currentScreen = SCREEN_OPTION_SELECT;
    screenDirty = true;
  }

  if (screenDirty) {
    renderScreen();
    screenDirty = false;
  }
}