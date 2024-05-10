
/*
 * 
 * XPLProGearStatus
 *
 * This example lights LEDs for each of the Nose, Left Main, and Right Main landing gear when they are down and locked.  
 * The dataref associated with the gear position is an array of floats, so this is a good example for that case.
 * 
 * Created by Curiosity Workshop for XPL/Pro arduino->XPlane system.
 * 
 * This sketch was developed and tested on an Arduino Mega.
 * 
   To report problems, download updates and examples, suggest enhancements or get technical support:
  
      discord:  https://discord.gg/RacvaRFsMW
      patreon:  www.patreon.com/curiosityworkshop
      YouTube:  https://youtube.com/channel/UCISdHdJIundC-OSVAEPzQIQ
 * 
 * 
 */


#include <Arduino.h>

#include <XPLPro.h>

XPLPro XP(&Serial);

#define PIN_GEAR_L    22
#define PIN_GEAR_R    23
#define PIN_GEAR_N    24
#define PIN_GEAR_TRAN 25

unsigned drefGearDeploy;
unsigned drefGearUnsafe;


void setup()
{

  pinMode(PIN_GEAR_L, OUTPUT);
  pinMode(PIN_GEAR_R, OUTPUT);
  pinMode(PIN_GEAR_N, OUTPUT);
  pinMode(PIN_GEAR_TRAN, OUTPUT);

  Serial.begin(XPL_BAUDRATE);  

  XP.begin("Gear LED", &xplRegister, &xplShutdown, &xplInboundHandler);
  digitalWrite(PIN_GEAR_L, LOW);
  digitalWrite(PIN_GEAR_R, LOW);
  digitalWrite(PIN_GEAR_N, LOW);
  digitalWrite(PIN_GEAR_TRAN, LOW);
 }

void loop()
{
  XP.xloop();
 
}


void xplInboundHandler(inStruct *inData)
{

  
 //Transition

  if(inData->handle == drefGearUnsafe)
  {
     if(inData->inLong == 1) digitalWrite(PIN_GEAR_TRAN, HIGH);  else    digitalWrite(PIN_GEAR_TRAN, LOW);
        
  }

  //Gear

  if(inData->handle == drefGearDeploy)
  { switch (inData->element)
    {
      case 0:     // nose gear
        if (inData->inFloat == 1)    digitalWrite(PIN_GEAR_N, HIGH);   else digitalWrite(PIN_GEAR_N, LOW);
        break;

      case 1:     // Left Main 
        if (inData->inFloat == 1)    digitalWrite(PIN_GEAR_L, HIGH);   else digitalWrite(PIN_GEAR_L, LOW);   
        break;

      case 2:     // Right Main
        if (inData->inFloat == 1)    digitalWrite(PIN_GEAR_R, HIGH);  else digitalWrite(PIN_GEAR_R, LOW);   
        break;

    }
    
  }
 


}



void xplRegister()
{

  drefGearUnsafe   = XP.registerDataRef(F("sim/cockpit2/annunciators/gear_unsafe"));  // This dataref is 1 for unsafe, 0 for safe.
  XP.requestUpdates(drefGearUnsafe, 100, 1);                       

  drefGearDeploy   = XP.registerDataRef(F("sim/flightmodel2/gear/deploy_ratio"));   // This dataref is an array of the position of each landing gear.  
  XP.requestUpdates(drefGearDeploy, 100, .1, 0);                                     // We are interest in the first 3,  0=Nose, 1 = Left, 2 = Right     
  XP.requestUpdates(drefGearDeploy, 100, .1, 1);                                    // I am using a precision of .1 so we can tell the difference between Down, Up and somewhere in between.
  XP.requestUpdates(drefGearDeploy, 100, .1, 2);
 
}
 

void xplShutdown()
{
 
}

