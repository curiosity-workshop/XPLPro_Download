

/*
 * 
 * XPLProDualRPMGauge
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
#include <Servo.h>                // servo library by Michael Margolis

#include <XPLPro.h>              //  include file for the X-plane direct interface 
XPLPro XP(&Serial);      // create an instance of it

Servo servoRPM1;
Servo servoRPM2;

long int startTime;


#define PIN_SERVO_RPM1    8          // RPM Servo Engine 1
#define PIN_SERVO_RPM2    9          // RPM Servo Engine 2



unsigned drefRPM;         // since this dataref is an array we only need it once for both engines, we can ask for the elements separately

void setup() 
{
  
  servoRPM1.attach(PIN_SERVO_RPM1);  // engine 1 rpm servo
  servoRPM2.attach(PIN_SERVO_RPM2);   // engine 2 rpm servo
     
  Serial.begin(XPL_BAUDRATE); // start serial interface.  Baudrate is specified in the header, dont change   

 /*
    needed for initialization.  Parameters are:
        a texual identifier of your device 
        your function that will be called when xplane and the plugin are ready for dataref and command registrations
        your function that will be called when xplane either shuts down or unloads an aircraft model
        your function that will be called when we have requested sends of datarefs that have changed
 */
  XP.begin("XPLPro Dual RPM Servo Gauge", &xplRegister, &xplShutdown, &xplInboundHandler);               
  digitalWrite(LED_BUILTIN, LOW);


}


void loop() 
{

  XP.xloop();  //  needs to run every cycle.  
  
}

/*
 * This function is the callback function we specified that will be called any time our requested data is sent to us.
 * handle is the handle to the dataref.  The following values are transfered to the callback through inStruct:
 * 
 *  int inData->handle          The handle of the incoming dataref
 *  int inData->element         If the dataref is an array style dataref, this is the element of the array
 *  long inData->inLong         long value for datarefs that are long values
 *  float inData->inFloat       float value for datarefs that are float values
 */
void xplInboundHandler(inStruct *inData)
{
  if (inData->handle == drefRPM)
  {   
   if (inData->element == 0) servoRPM1.write((int)inData->inFloat);    // set position of engine 1 rpm servo
   if (inData->element == 1) servoRPM2.write((int)inData->inFloat);    // set position of engine 2 rpm servo 
     
  }


  
}

void xplRegister()          // this is the function we set as a callback for when the plugin is ready to receive dataref and command bindings requests
{

 
  drefRPM = XP.registerDataRef(F("sim/cockpit2/engine/indicators/engine_speed_rpm") );            // this will be updated from xplane to tell us the rpm
  XP.requestUpdates(drefRPM, 200, 20, 0);          // Tell xplane to send us updates when the RPM changes by 20 and no more than every 200milliseconds (5 times per second)
  XP.requestUpdates(drefRPM, 200, 20, 1);          //    and for engine 2 (array element 1)
  
  XP.setScaling(drefRPM, 0,3500, 180,0);            // Have the plugin scale the data to match our servo scale.  RPM on the baron is 0 - 3500, servo takes 0 - 180 full scale.  Adjust as needed

}

void xplShutdown()
{
  // if you need to do things when xplane shuts down or unloads an aircraft, do it here.
  
}
