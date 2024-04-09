
/*
 * 
 * XPLProArraysExample
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

#include <arduino.h>

#include <XPLPro.h>

#define XPLPOTS_MAXPOTS  3    //  adjust this as required for your needs.  Higher number of course costs memory.  Default is 10 if not specified
#include <XPLPotentiometers.h>
#include <XPLSwitches.h>
              

#define PIN_THROTTLE1 A0
#define PIN_THROTTLE2 A1

#define PIN_GEN1    49
#define PIN_GEN2    51


int drefRPM;

XPLPro XP(&Serial);     


XPLPotentiometers pots(NULL);             // Instead of providing a handler we supply NULL if a handler is not needed         
XPLSwitches switches(NULL);    

void setup() 
{
  Serial.begin(XPL_BAUDRATE);  
  XP.begin("XPLPro Arrays Example", &xplRegister, &xplShutdown, &xplInboundHandler);         

  pots.begin(&XP);
  switches.begin(&XP);

}

void loop() 
{
  XP.xloop();  
  pots.check();
  switches.check();


 
}      

void xplInboundHandler(inStruct *inData)
{
 /*
 * This deals with incoming data that we requested.
 */

  if (inData->handle == drefRPM)
  { switch (inData->element )
    {
      case 0:     // left engine
        // inData->inFloat is what will contain the new data (could also be inLong and other types depending on the dataref)
        //   Do something cool with the data here.
        break;

      case 1:     // right engine.  Maybe print to an lcd?  
        //  lcd.print(inData->inFloat);   
        break;

  
    }
  }     
     
}

void xplShutdown()
{
  
  
}


void xplRegister()          
{
  pots.clear();               // Reset pots
  switches.clear();           // Reset switches

// throttle_ratio is a dataref that utilizes array elements.  Left engine is element 0, right engine is element 1 and is retained by the potentiometer wrapper.  
// We can also write manually with XP.datarefWrite(handle, value, element) if not using the wrapper.
int drefThrottleRatio = XP.registerDataRef(F("sim/cockpit2/engine/actuators/throttle_ratio"));   // We don't need to save this but temporarily holding the handle value will prevent us from having to query the plugin multiple times for the same datare

  pots.addPin(PIN_THROTTLE1, XPLPOTS_DATAREFWRITE, drefThrottleRatio, 
    0,  // array element (left engine)
    10,  // precision, this rounds the pot value to the closest 10 in this example
    0, 1024, 0, 1);     // scaling request.  Plugin handles the math for us and converts the analogRead from 0-1024 to 0-1 as the dataref requires.

// throttle for engine 2, similar but for element 1 (right engine)
  pots.addPin(PIN_THROTTLE2, XPLPOTS_DATAREFWRITE, drefThrottleRatio, 1, 10, 0, 1024, 0, 1);   // Right engine is element 1
  
// We can also handle them manually without the use of the wrapper.  Example:
// int drefMixture;         // at the beginning of the code
//  drefMixture = XP.registerDataRef(F("sim/cockpit2/engine/actuators/mixture_ratio"))
//  
// then in the loop somewhere we can write to the dataref:
//  XP.datarefWrite(drefThrottle, .5, 0);       // 0 is left engine, set it to 50%
//  XP.datarefWrite(drefThrottle, .4, 1);       // 1 is right engine, set it to 40%
//


// now connect a switch on PIN_GEN1 and PIN_GEN2 to a dataref that controls engine 1 and 2 generators (array elements 0 and 1).
// We also could write to the handle without the switch wrapper of use a switch handler, etc.

  switches.addPin(PIN_GEN1,   XPLSWITCHES_DATAREFWRITE,           XP.registerDataRef(F("sim/cockpit2/electrical/generator_on")), 0 );      //connect PIN_GEN1 to engine 1 generator
  switches.addPin(PIN_GEN2,   XPLSWITCHES_DATAREFWRITE_INVERT,    XP.registerDataRef(F("sim/cockpit2/electrical/generator_on")), 1 );      //connect PIN_GEN2 to engine 2 generator.  I inverted this for demonstration purposes
  
  
// This examples requests an array dataref for inbound handling.
// See the inbound handler above for how to deal with incoming data
// Be mindful of the time between requests and the precision you need.  Some datarefs are very active and this can produce crazy amounts of 
// incoming data to deal with.
  drefRPM = XP.registerDataRef(F("sim/cockpit2/engine/indicators/engine_speed_rpm"));
  XP.requestUpdates(drefRPM, 500, 20, 0);            // request drefRPM to be updated every 500ms, 20rpm precision and element 0;
  XP.requestUpdates(drefRPM, 500, 20, 1);             // same for element 1.

 
}

