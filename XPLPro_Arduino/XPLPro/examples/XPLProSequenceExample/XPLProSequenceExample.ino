

/*
 * 
 * XPLProSequenceExample
 * 
 * This sketch runs a series of commands or dataref writes over time when triggered  
 *
 * 
 * Created by Curiosity Workshop for XPL/Pro arduino->XPlane system.
 * 
 * This sketch was developed and tested on an Arduino Mega.
 * 
   To report problems, download updates and examples, suggest enhancements or get technical support:
  
      discord:  https://discord.gg/RacvaRFsMW
      patreon:  www.patreon.com/curiosityworkshop
      YouTube:  https://youtube.com/channel/UCISdHdJIundC-OSVAEPzQIQ
      Facebook: https://www.facebook.com/curiosity.workshop42/
 *
 * 
 */

#include <arduino.h>
#include <XPLPro.h>              //  include file for the X-plane direct interface 

#define XPLSEQUENCER_MAXEVENTS     10                  //  Max amount of events to allow.  Default to 10 if not defined
#include <XPLSequencer.h>         // for making timed sequences

XPLPro XP(&Serial);      // create an instance of it


void seqStartupHandler(int inItem);    // This will be called at the requested times
XPLSequencer seqStartup(&seqStartupHandler);    // this will be the sequence for startup.  We can make additional ones as needed.

int drefMasterBattery;
int drefBeacon;         // this stores a handle to the beacon light dataref
int drefNavLight;       // this stores a handle to the nav light dataref
int drefThrottle;       // this stores a handle to the throttle position dataref

int cmdLdgLightToggle;  // this stores a handle to the landing light toggle command


void setup() 
{
 
  pinMode(LED_BUILTIN, OUTPUT);     // built in LED on arduino board will turn on and off with the status of the beacon light
  //pinMode(PIN_TRIGGER, INPUT_PULLUP);       
  
  Serial.begin(XPL_BAUDRATE); // start serial interface.  Baudrate is specified in the header, dont change   

 /*
    needed for initialization.  Parameters are:
        a texual identifier of your device 
        your function that will be called when xplane and the plugin are ready for dataref and command registrations
        your function that will be called when xplane either shuts down or unloads an aircraft model
        your function that will be called when we have requested sends of datarefs that have changed
 */
  XP.begin("XPLPro Sequence Example!", &xplRegister, &xplShutdown, &xplInboundHandler);               
  digitalWrite(LED_BUILTIN, LOW);


}


void loop() 
{
 
  unsigned long timeNow;
  unsigned long startTime;
  timeNow = millis();

  XP.xloop();  //  needs to run every cycle.  
  seqStartup.check(timeNow);


}

/*
 * This function is the callback function we specified that will be called any time our requested data is sent to us.
 * handle is the handle to the dataref.  The following variables within the plugin are how we receive the data:
 * 
 * inStruct *inData is a pointer to a structure that contains information about the incoming dataref. 
 * 
 *  int inData->handle          The handle of the incoming dataref
 *  int inData->element         If the dataref is an array style dataref, this is the element of the array
 *  long inData->inLong         long value for datarefs that are long values
 *  float inData->inFloat       float value for datarefs that are float values
 */
 void xplInboundHandler(inStruct *inData)
{
 
 if (inData->handle == drefMasterBattery)
  { 
    if (inData->inLong == 1)  seqStartup.trigger();     // The plugin keeps track of the data and only updates it when it changes.   
  
    
  }     
   
}

/*
 *  this is the function we set as a callback for when the plugin is ready to receive dataref and command bindings requests
 *  It could be called multiple times if for instance xplane is restarted or loads a different aircraft
 */
void xplRegister()         
{

  drefMasterBattery = XP.registerDataRef(F("sim/cockpit/electrical/battery_on") );
  XP.requestUpdates(drefMasterBattery, 100, 1);       //  We need to subscribe for updates to the dataref if wanting to read them.  When they change the inbound handler will be called
  

  drefBeacon = XP.registerDataRef(F("sim/cockpit2/switches/beacon_on") );    
  drefNavLight = XP.registerDataRef(F("sim/cockpit/electrical/nav_lights_on") );    
  drefThrottle = XP.registerDataRef(F("sim/cockpit2/engine/actuators/throttle_ratio") );
  cmdLdgLightToggle = XP.registerCommand(F("sim/lights/landing_lights_toggle") );

  seqStartup.clear();
  seqStartup.addEvent(2000);       // first event will occur 2000ms after trigger.
  seqStartup.addEvent(2000);      // second event will occur 2 seconds after first event
  seqStartup.addEvent(1000);      // third event will occur 1 second after the previous one
  seqStartup.addEvent(2000);      // ...and so on.
  seqStartup.addEvent(1000);

 
}

void xplShutdown()
{
  // if you need to do things when xplane shuts down or unloads an aircraft, do it here.
  
}


// This will be called for each scheduled event.  inItem is which event we are on in the sequence requested.
void seqStartupHandler(int inItem)
{
  
  switch(inItem)
  {
    case 0 :  XP.datarefWrite(drefBeacon, 1);       break;  // turn on the beacon light
    case 1 :  XP.datarefWrite(drefNavLight, 1);     break;  // turn on the nav light
    case 2 :  XP.commandTrigger(cmdLdgLightToggle); break;  // toggle the landing light on/off
    case 3 :  XP.datarefWrite(drefThrottle, .2F);   break;  // had to specify the type of .2 with "F" so it knows it is a float.  You could also do (float).2
    case 4 :  XP.sendSpeakMessage("Sequence Complete"); break;  
  }

}

