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

#define SW   (8)// sw pin 
#define LED  (7)// led sw pin 
#define SERIAL_BAUD_RATE  (115200)
#define SERIAL_TIMEOUT    (1)
const int IR_STABLE_TIME = 20;
const int PLAYER_STABLE_TIME = 1000; //ms watit after start for player stable
const int MAX_VOLUME_LEVEL = 30; //30 plus 0 
const uint32_t IDLE_TIME  = 10000; //ms idle time if no switch preased (visitor action) - lowr volume 

const int Next_Movie_Stable_Time = 500;// ms time waiting after change movie

int Current_Volume_Level = 0;//assume volume level = 0 No sound
int Optimal_Volume_Level = 16;//middle level 
uint32_t Idle_Time_Counter = 0;
boolean Is_Silent = true;// assume silent 
 
IRsend mySender;

bool first_entry = true;
long timer_player = 0;
const long TIME_TO_PLAY = 10000;
const int BOUNCE_TIME = 100;// ms time waiting befour re-enable pushbuton check
bool check;

bool PRESS_BUTTON(int IO) {

  // Check if the button is pressed
  if (digitalRead(IO) == LOW && check == LOW) {
     //Serial.println("press :");
     check = HIGH;         // Mark that the button is being pressed
    delay(BOUNCE_TIME); // Apply debounce delay
  }

  // Check if the button is released
  if (digitalRead(IO) == HIGH && check == HIGH) {
    //Serial.println("unpress");
    check = LOW;  // Reset the state for the next button press
    return HIGH;  // Indicate that the button was successfully pressed and released
  }
  return LOW; // Return false if the button is not in the desired state
}


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
  if ((Target_Volume_Level >= 0) && (Target_Volume_Level <= MAX_VOLUME_LEVEL)){
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
  pinMode(SW, INPUT_PULLUP);// define input pins, active low 
  pinMode(LED, OUTPUT);
  delay(PLAYER_STABLE_TIME);// wait for player stable and start 
  Idle_Time_Counter = millis();// reset idle counter 
  silent();
  set_volume_level(Optimal_Volume_Level);
  Serial.println ("init"); 
}

void loop() {

  if ( (millis() - timer_player) >= TIME_TO_PLAY){
    if(first_entry){
      digitalWrite(LED,HIGH);
      first_entry = false;
    }
    if(PRESS_BUTTON(SW)){
      digitalWrite(LED,LOW);
      mySender.send(NEC,IR_NEXT,0);//send Next IR command
      Serial.println ("NEXT");  
      Idle_Time_Counter = millis();// reset idle counter 
      delay(Next_Movie_Stable_Time); // wait 1 sec to make sure movie changed 

      timer_player = millis();
      first_entry = true;
    }
  }

}

