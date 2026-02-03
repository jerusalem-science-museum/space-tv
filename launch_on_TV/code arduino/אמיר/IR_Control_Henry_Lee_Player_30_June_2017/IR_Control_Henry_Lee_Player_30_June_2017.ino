/* Control HD Video player bought from Henry lee May 2017
 * See H:\web_order\הזמנות מהנרי לי לתערוכת אופניים\נגני וידיאו ו LGP
 *  based on https://learn.adafruit.com/using-an-infrared-library/hardware-needed
 *  D3 output !!! IR led via 300, !!! can't change IO in code only in IRLibHardware.h
 *  D8, D9 input for switch (SW pulled up active low)  
 *  15 June 2017 add volume control 
 */ 
#include "Henry_Lee_Video_Player_Code.h" // IR code list 
// see "טבלאות קודים נגן ווידיאו תערוכת אופניים 2017.xls" , lib: "H:\Amir Design\Arduino Sketch\IR Control"

#include <IRLibSendBase.h>    // First include the send base
//Now include only the protocols you wish to actually use.
//The lowest numbered protocol should be first but remainder 
//can be any order.
#include <IRLib_P01_NEC.h>    
#include <IRLibCombo.h>     // After all protocols, include this
// All of the above automatically creates a universal sending
// class called "IRsend" containing only the protocols you want.
// Now declare an instance of that sender.

#define SW_0  (8)// define input swith 0 
#define SW_1  (9)// define input swith 1 
#define SERIAL_BAUD_RATE  (9600)
#define SERIAL_TIMEOUT    (1)
const int IR_STABLE_TIME = 50;
const int PLAYER_STABLE_TIME = 1000; //ms watit after start for player stable
const int MAX_VOLUME_LEVEL = 30; //30 plus 0 
const uint32_t IDLE_TIME  = 300000; //ms idle time if no switch preased (visitor action) - lowr volume 
const int DebounceTime = 100;// ms time waiting befour re-enable pushbuton check
const int Next_Movie_Stable_Time = 500;// ms time waiting after change movie
int sw_0_state;//switch (push buton 0) state - active low
int sw_1_state;//switch (push buton 0) state - active low

int Current_Volume_Level = 0;//assume volume level = 0 No sound
int Optimal_Volume_Level = 20;//middle level 
uint32_t Idle_Time_Counter = 0;
boolean Is_Silent = true;// assume silent 
 
IRsend mySender;


void silent(){
  for (int i=0; i <= MAX_VOLUME_LEVEL; i++){
      mySender.send(NEC,IR_VOL_DN,0);//send volume down IR command
     // delay (IR_STABLE_TIME);//stable 
       }
   Current_Volume_Level = 0;//
   Is_Silent = true; // 
   Serial.println ("Now in silent");
 }

void set_volume_level(int Target_Volume_Level){
  if ((Target_Volume_Level >= 0)&& (Target_Volume_Level <= MAX_VOLUME_LEVEL)){
    if (Target_Volume_Level > Current_Volume_Level) {
      while (Current_Volume_Level < Target_Volume_Level){
          mySender.send(NEC,IR_VOL_UP,0);//send volume UP IR command
          delay (IR_STABLE_TIME);//stable 
          Current_Volume_Level ++ ; //increase current volume level 
         }
      
    }
    if (Current_Volume_Level > Target_Volume_Level ) {
      while (Current_Volume_Level > Target_Volume_Level){
          mySender.send(NEC,IR_VOL_DN,0);//send volume down IR command
          delay (IR_STABLE_TIME);//stable 
          Current_Volume_Level -- ; //decrease  current volume level 
         }
     
     
   }
   if (Current_Volume_Level == 0) {Is_Silent = true;}
   else {Is_Silent = false;};
  }
  Serial.print ("Current Volume Level is ");
  Serial.print ("\t"); //tab
  Serial.println (Current_Volume_Level);
  
}  

 
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(SW_0, INPUT_PULLUP);// define input pins, active low 
  pinMode(SW_1, INPUT_PULLUP);
  delay(PLAYER_STABLE_TIME);// wait for player stable and start 
  silent();
  Idle_Time_Counter = millis();// reset idle counter 
}

void loop() {
    if ( ((millis() - Idle_Time_Counter) >= IDLE_TIME )&&(Is_Silent == false)){
      silent();
      Is_Silent = true  ;   
       }

  sw_0_state = digitalRead(SW_0);// read switch 
  if (sw_0_state == 0) {
    delay(DebounceTime);
    sw_0_state = digitalRead(SW_0);// read again
    if (sw_0_state == 0) {
      //if (Is_Silent == true) {set_volume_level(Optimal_Volume_Level);}; // set volume level 
      mySender.send(NEC,IR_NEXT,0);//send Next IR command
      Serial.println ("NEXT");
      if (Is_Silent == true) {set_volume_level(Optimal_Volume_Level);}; // set volume level 
      do{sw_0_state = digitalRead(SW_0);   
      } while(sw_0_state == 0);// wait until switch not preased 
      Idle_Time_Counter = millis();// reset idle counter 
      delay(Next_Movie_Stable_Time); // wait 1 sec to make sure movie changed 
    }
  }
}

