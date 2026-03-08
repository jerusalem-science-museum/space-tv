#include "const.h"
#include "basic_routines.h"

void setup() {

  Serial.begin(SERIAL_BAUD_RATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(LED_BUILTIN, OUTPUT);

  IrSender.begin();
  Serial.println("init");

}

void loop() {
  
  sendIR_NEC(LG_CODE.POWER,SEND_REPEAT);
  sendIR_PulseDistance(TCL_CODE.POWER,SEND_REPEAT);
  Serial.println("POWER");   
 
}
