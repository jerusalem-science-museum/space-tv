/*
 *  based on https://learn.adafruit.com/using-an-infrared-library/hardware-needed
 *  D3 output !!! IR led via 1kphm, !!! can't change IO in code only in IRLibHardware.h
*/
#include <Arduino.h>
#include "TCL_code.h" // IR code list 

#define IR_SEND_PIN 3
#include <IRremote.hpp> // include the library

#define SERIAL_BAUD_RATE  (115200)   // vitesse du port série (Serial Monitor)
#define SERIAL_TIMEOUT    (1)        // timeout de lecture série (ms ou valeur utilisée par setTimeout)

// ===================== IR CONSTANTS =====================
// Fréquence porteuse IR (modulation de la LED IR). 38 kHz est très courant.
static const uint16_t IR_KHZ = 38;

// --- Timings en microsecondes (µs) ---
// "Header" = séquence de début de trame : ON (mark) puis OFF (space)
static const uint16_t IR_HDR_MARK  = 3850;  // durée ON du header (porteuse active)
static const uint16_t IR_HDR_SPACE = 4000;  // durée OFF du header (silence)

// Définition d'un bit '1' : ON court + OFF long (ici, l'OFF est plus long que pour '0')
static const uint16_t IR_ONE_MARK  = 450;   // durée ON pour un bit 1
static const uint16_t IR_ONE_SPACE = 2000;  // durée OFF pour un bit 1

// Définition d'un bit '0' : ON court + OFF plus court

static const uint16_t IR_ZERO_MARK  = 450;  // durée ON pour un bit 0
static const uint16_t IR_ZERO_SPACE = 1050; // durée OFF pour un bit 0

// Nombre de bits à envoyer depuis "data" (ici 24 bits)
static const uint8_t  IR_BITS = 24;

// Période entre répétitions (ms) quand on demande plusieurs envois
static const uint16_t IR_REPEAT_PERIOD_MS = 110;

// Ordre des bits : LSB first = bit de poids faible envoyé en premier
static const uint8_t  IR_FLAGS = PROTOCOL_IS_LSB_FIRST;

/////////////////////////// DELAYS ////////////////////
const int32_t START_STABLE_TIME  = 500; //ms wait after start for player stable
const int32_t DELAY_BEFORE_START = 10000;

///////////NUMBER OF REPEAT ///////////
// Nombre total d'envois souhaité (1 = une seule trame, 2 = 2 trames, etc.)
int sendCountstart = 5;

// ===================== SEND FUNCTION =====================
// Envoie une trame IR "PulseDistanceWidth" avec les timings ci-dessus.
// Paramètres:
//   - data: le code IR (sur 24 bits ici)
//   - sendCount: nombre total d'envois (1 = une fois, 2 = deux fois, ...)
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


void setup() {
  // Initialise la communication série pour debug/monitoring
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(LED_BUILTIN, OUTPUT);


  // Initialise l'émetteur IR (broche d'émission définie par IR_SEND_PIN / config IRremote)
  // NOTE: selon la config IRremote, la broche est prise depuis PinDefinitionsAndMore.h
  IrSender.begin();

  // Coupe la LED de feedback (sinon elle clignote à chaque envoi IR)
  disableLEDFeedback();
  // Message de debug pour confirmer l'initialisation
  Serial.println("init");
  delay(DELAY_BEFORE_START);

  sendIR_PulseDistance(POWER,sendCountstart);
  Serial.println("POWER");   
  delay(START_STABLE_TIME); // wait 10 sec to make sure tv is on 
  

}

void loop() {
}
