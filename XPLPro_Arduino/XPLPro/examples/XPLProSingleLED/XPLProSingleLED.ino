

/*
 * 
 * XPLProSingleLED
 * 
 * This example shows how to connect a single LED to an xplane dataref.
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

#include <XPLPro.h>              //  include file for the X-plane direct interface 
XPLPro XP(&Serial);      // create an instance of it

#define PIN_LED0          10         // Pin for LED Anode (positive).  The Cathode (negative) will need to be connected
                                      // to ground, and somewhere in the line should be a resistor ~220 ohms for the 5v signal
                                      // coming from the Arduino Mega.  

unsigned datarefLED0;           // This will store a handle to access information from the requested dataref.

void setup() 
{
 
  pinMode(PIN_LED0, OUTPUT);     // This tells the arduino board that we are using the pin for output
 
  Serial.begin(XPL_BAUDRATE); // start serial interface.  Baudrate is specified in the header, dont change   

 /*
    needed for initialization.  Parameters are:
        a texual identifier of your device 
        your function that will be called when xplane and the plugin are ready for dataref and command registrations
        your function that will be called when xplane either shuts down or unloads an aircraft model
        your function that will be called when we have requested sends of datarefs that have changed
 */
  XP.begin("XPLPro Single LED Demo!", &xplRegister, &xplShutdown, &xplInboundHandler);               
  digitalWrite(PIN_LED0, LOW);        // turn off the LED for now


}


void loop() 
{
 
  XP.xloop();  //  needs to run every cycle.  
 
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
 *  int inData->strLength       If the dataref is binary data/string, length of the data
 *  char inData->inStr          if binary data/string, a pointer to it.
 */
void xplInboundHandler(inStruct *inData)
{
 
 if (inData->handle == datarefLED0)       // Which handle is supplying new data... maybe ours?
 {
    digitalWrite(PIN_LED0, inData->inLong);   //  Xplane talks in long integers
 }
   
}

/*
 *  this is the function we set as a callback for when the plugin is ready to receive dataref and command bindings requests
 *  It could be called multiple times if for instance xplane is restarted or loads a different aircraft
 */
void xplRegister()         
{
/*
 * This example registers one dataref.  
 * As a universal example I am using the nav light switch which is available in most aircraft models.
 */

  datarefLED0     = XP.registerDataRef(F("sim/cockpit2/switches/navigation_lights_on"));          
  XP.requestUpdates(datarefLED0, 100, 0);       // request that new values get sent to us.
  
}

void xplShutdown()
{
  // if you need to do things when xplane shuts down or unloads an aircraft, do it here.
  
}

