#ifndef CONST
#define CONST


#include "ALL_CODE_REMOTE.h"
#define IR_SEND_PIN 3

#define SERIAL_BAUD_RATE  (115200)   // vitesse du port série (Serial Monitor)
#define SERIAL_TIMEOUT    (10)        // timeout de lecture série (ms ou valeur utilisée par setTimeout)

const int IR_STABLE_TIME = 2000;
const uint8_t SEND_REPEAT = 2; // Nombre total d'envois souhaité (1 = une seule trame, 2 = 2 trames, etc.)

LG_CODE_IR     LG_CODE;
TCL_CODE_IR    TCL_CODE;
SENSOR_CODE_IR SENSOR_CODE;
MAG_CODE_IR    MAG_CODE;
PURE_ACOUSTICS_BT_CODE_IR PURE_ACOUSTICS_BT_CODE;
SKYWORTH_CODE_IR SKYWORTH_CODE;

// ===================== IR CONSTANTS =====================
// Fréquence porteuse IR (modulation de la LED IR). 38 kHz est très courant.
static const uint16_t IR_KHZ = 38;

// --- Timings en microsecondes (µs) ---
// "Header" = séquence de début de trame : ON (mark) puis OFF (space)
static const uint16_t IR_HDR_MARK  = 3850;  // durée ON du header (porteuse active)
static const uint16_t IR_HDR_SPACE = 4000;  // durée OFF du header (silence)

// Définition d'un bit '1' : ON court + OFF long (ici, l'OFF est plus long que pour '0')//////
static const uint16_t IR_ONE_MARK  = 450;   // durée ON pour un bit 1
static const uint16_t IR_ONE_SPACE = 2000;  // durée OFF pour un bit 1
//////////////////////////////////////////////////////////////////

///// Définition d'un bit '0' : ON court + OFF plus court///////////
static const uint16_t IR_ZERO_MARK  = 450;  // durée ON pour un bit 0
static const uint16_t IR_ZERO_SPACE = 1050; // durée OFF pour un bit 0
////////////////////////////////////////////////////////////////////
static const uint8_t  IR_BITS = 24;// Nombre de bits à envoyer depuis "data" (ici 24 bits)
static const uint16_t IR_REPEAT_PERIOD_MS = 110;// Période entre répétitions (ms) quand on demande plusieurs envois
//const uint8_t PROTOCOL_IS_LSB_FIRST = 0x00;
static const uint8_t  IR_FLAGS = PROTOCOL_IS_LSB_FIRST;// Ordre des bits : LSB first = bit de poids faible envoyé en premier

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

// ===================== ENCODER =====================
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
  {"BT_SPK"},
  {"SKYWORTH"}
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
  {"HOME"},
  {"LEFT"},
  {"RIGHT"},
  {"BACK"}
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

bool lastSwState = HIGH;
bool lastBackState = HIGH;

unsigned long lastSwTime = 0;
unsigned long lastBackTime = 0;
const unsigned long debounceMs = 120;

unsigned long messageUntil = 0;
bool screenDirty = true;

#endif