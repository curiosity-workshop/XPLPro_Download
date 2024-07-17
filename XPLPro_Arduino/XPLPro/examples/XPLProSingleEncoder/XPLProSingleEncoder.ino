

/*
 * 
 * XPLProSingleEncoder
 * 
 * This example shows how to connect a single rotary encoder and trigger xplane commands with it.
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

#include <Encoder.h>                // Available in the library manager.    For the rotary encoder

#include <XPLPro.h>              //  include file for the X-plane direct interface 
XPLPro XP(&Serial);      // create an instance of it

// Encoder Pins

#define PIN_ENCODER0A       8         // Pins for first encoder.  
#define PIN_ENCODER0B       9

Encoder encoder1(PIN_ENCODER0A, PIN_ENCODER0B);     // create an instance for the encoder
int enc0;                                           // this will be used to store and accumulate the current value of the encoder 
unsigned cmdEncoder0Up;                             // these hold the command handles after registering the xplane commands.
unsigned cmdEncoder0Dn;

void setup() 
{
 
  pinMode(LED_BUILTIN, OUTPUT);     // built in LED on arduino board will turn on and off with the status of the beacon light
 
  Serial.begin(XPL_BAUDRATE); // start serial interface.  Baudrate is specified in the header, dont change   

 /*
    needed for initialization.  Parameters are:
        a texual identifier of your device 
        your function that will be called when xplane and the plugin are ready for dataref and command registrations
        your function that will be called when xplane either shuts down or unloads an aircraft model
        your function that will be called when we have requested sends of datarefs that have changed
 */
  XP.begin("XPLPro Single Encoder Demo!", &xplRegister, &xplShutdown, &xplInboundHandler);               
  digitalWrite(LED_BUILTIN, LOW);


}


void loop() 
{
 
  XP.xloop();  //  needs to run every cycle.  
  EncodersCheck();    //  This needs to run as often as possible as well unless you are using interrupt pins


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
 
   
}

/*
 *  this is the function we set as a callback for when the plugin is ready to receive dataref and command bindings requests
 *  It could be called multiple times if for instance xplane is restarted or loads a different aircraft
 */
void xplRegister()         
{
/*
 * This example registers two commands.  These can be anything that makes sense to control with a rotary encoder.  
 * As a universal example I am using the heading select which is available in most aircraft models.
 */

  cmdEncoder0Up     = XP.registerCommand(F("sim/autopilot/heading_down"));          
  cmdEncoder0Dn     = XP.registerCommand(F("sim/autopilot/heading_up"));         

  
}

void xplShutdown()
{
  // if you need to do things when xplane shuts down or unloads an aircraft, do it here.
  
}


void EncodersCheck(void)
{
  enc0 += encoder1.readAndReset();
//  enc1 += encoder1.readAndReset();          // to add encoders follow this format
//  enc2 += encoder2.readAndReset();
//  enc3 += encoder3.readAndReset();
//  enc4 += encoder4.readAndReset();
//  enc5 += encoder5.readAndReset();

  if (enc0 < -3)  { XP.commandTrigger(cmdEncoder0Up);    enc0 = 0; }      // The quadrature encoders I am using create 4 pulses per detent.  This waits until 4 are received before triggering the command.
  if (enc0 > 3)   { XP.commandTrigger(cmdEncoder0Dn);    enc0 = 0; }

//  if (enc1 < -3)  { XP.commandTrigger(cmdAirspeedUp);       enc1 = 0; }
//  if (enc1 > 3)   { XP.commandTrigger(cmdAirspeedDn);       enc1 = 0; }

//  if (enc2 < -3)  { XP.commandTrigger(cmdHeadingUp);        enc2 = 0; }
//  if (enc2 > 3)   { XP.commandTrigger(cmdHeadingDn);        enc2 = 0; }

//  if (enc3 < -3)  { XP.commandTrigger(cmdAltitudeDn);       enc3 = 0; }
//  if (enc3 > 3)   { XP.commandTrigger(cmdAltitudeUp);       enc3 = 0; }

//  if (enc4 < -3)  { XP.commandTrigger(cmdVerticalSpeedUp);  enc4 = 0; }
//  if (enc4 > 3)   { XP.commandTrigger(cmdVerticalSpeedDn);  enc4 = 0; }

//  if (enc5 < -3)  { XP.commandTrigger(cmdCourseCopilotUp);  enc5 = 0; }
//  if (enc5 > 3)   { XP.commandTrigger(cmdCourseCopilotDn);  enc5 = 0; }

}
