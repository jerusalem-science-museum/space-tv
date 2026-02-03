/*
 *  based on https://learn.adafruit.com/using-an-infrared-library/hardware-needed
 *  D3 output !!! IR led via 1kphm, !!! can't change IO in code only in IRLibHardware.h
*/
#include <Arduino.h>
#include "LG_code.h" // IR code list 

#define IR_SEND_PIN 3
#include <IRremote.hpp> // include the library

#define SERIAL_BAUD_RATE  (115200)   // vitesse du port série (Serial Monitor)
#define SERIAL_TIMEOUT    (1)        // timeout de lecture série (ms ou valeur utilisée par setTimeout)

const int IR_STABLE_TIME = 2000;
const int POWER_STABLE_TIME = 10000;
const int32_t DELAY_BEFORE_START = 50000;
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
void sendIR_NEC(uint32_t data, uint8_t sendCount) {
  if (sendCount == 0) return; // sécurité : si 0, on n'envoie rien
  // Dans IRremote, on doit fournir "numberOfRepeats" = répétitions après le 1er envoi.
  // Exemple: sendCount=1 -> repeats=0 ; sendCount=3 -> repeats=2
  uint8_t repeats = sendCount - 1;

  IrSender.sendNECRaw(data,repeats);

  BLINK();
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

  sendIR_NEC(POWER,sendCountstart);
  Serial.println("POWER");   
  delay(POWER_STABLE_TIME); // wait 10 sec to make sure tv is on 

  sendIR_NEC(SOURCE,sendCount);
  Serial.println("SOURCE");   
  delay(IR_STABLE_TIME); // wait 10 sec to make sure tv is on 

  sendIR_NEC(RIGHT,sendCount);
  Serial.println("RIGHT");   
  delay(IR_STABLE_TIME); // wait 10 sec to make sure tv is on 

  sendIR_NEC(OK,sendCount);
  Serial.println("OK");   
  delay(IR_STABLE_TIME); // wait 10 sec to make sure tv is on 


}

void loop() {
}
