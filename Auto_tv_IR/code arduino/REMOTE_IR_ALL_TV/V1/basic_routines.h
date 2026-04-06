#ifndef ROUTINES
#define ROUtINES

#include <IRremote.hpp> // include the library
#include <Arduino.h>
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


#endif
