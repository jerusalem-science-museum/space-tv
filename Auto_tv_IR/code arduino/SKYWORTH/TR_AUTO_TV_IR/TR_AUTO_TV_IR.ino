/*
 *  based on https://learn.adafruit.com/using-an-infrared-library/hardware-needed
 *  D3 output !!! IR led via 1kphm, !!! can't change IO in code only in IRLibHardware.h
*/
#include <Arduino.h>
#include "SKYWORTH_code.h" // IR code list 

#define IR_SEND_PIN 3
#include <IRremote.hpp> // include the library

#define SERIAL_BAUD_RATE  (115200)   // vitesse du port série (Serial Monitor)
#define SERIAL_TIMEOUT    (1)        // timeout de lecture série (ms ou valeur utilisée par setTimeout)

const int IR_STABLE_TIME = 2000;
const int32_t DELAY_BEFORE_START = 2000;
// Nombre total d'envois souhaité (1 = une seule trame, 2 = 2 trames, etc.)
int sendCount = 2;
int sendCountstart = 5;


void BLINK() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(100);                      // wait for a second
}
// ===================== SEND FUNCTION =====================
// Envoie une trame IR "PulseDistanceWidth" avec les timings ci-dessus.
// Paramètres:
//   - data: le code IR (sur 24 bits ici)
//   - sendCount: nombre total d'envois (1 = une fois, 2 = deux fois, ...)
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
  delay(DELAY_BEFORE_START); // wait 10 sec to make sure tv is on 

  sendSamsungLGFromRaw(POWER,sendCount);
  Serial.println("POWER");   
  delay(IR_STABLE_TIME); // wait 10 sec to make sure tv is on 

}

void loop() {
}
