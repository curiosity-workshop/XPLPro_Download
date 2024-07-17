
/*
 * 
 * XPLProFlapStatus
 *
 * This example lights LEDs for each position of the flaps on the beechcraft Baron 58, including "In Transit".  This 
 * may or may not work on other aircraft models depending on the values sent at various flap positions, but it should
 * work as an example template.
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

#define PINLED_FLAP_TRAN   26
#define PINLED_FLAP_APP    27
#define PINLED_FLAP_DOWN   28

unsigned drefFlapDeploy;

void setup()
{
  

  pinMode(PINLED_FLAP_TRAN, OUTPUT);
  pinMode(PINLED_FLAP_APP, OUTPUT);
  pinMode(PINLED_FLAP_DOWN, OUTPUT);

  Serial.begin(XPL_BAUDRATE);  

  XP.begin("Flap LED Example", &xplRegister, &xplShutdown, &xplInboundHandler);
 
  digitalWrite(PINLED_FLAP_TRAN, LOW);
  digitalWrite(PINLED_FLAP_APP, LOW);
  digitalWrite(PINLED_FLAP_DOWN, LOW);
 }

void loop()
{
  XP.xloop();
 
}


void xplInboundHandler(inStruct *inData)
{

  if(inData->handle == drefFlapDeploy)
  {
    
    if (inData->inFloat == 0)           // Flaps are UP
    { setFlapLED(LOW, LOW, LOW);
      return;
    }

    if (inData->inFloat == 0.5)         // Flaps are at APPROACH
    {
      setFlapLED(LOW, HIGH, LOW);
      return;
    }

    if (inData->inFloat == 1)           // Flaps are DOWN
    { setFlapLED(LOW, LOW, HIGH);
      return;
    }
      
    setFlapLED(HIGH, LOW, LOW);       // In transit is the default
    
   
  }
 


}

void setFlapLED(int inTrans, int inApp, int inDown)
{
  digitalWrite(PINLED_FLAP_TRAN,  inTrans);
  digitalWrite(PINLED_FLAP_APP,   inApp);
  digitalWrite(PINLED_FLAP_DOWN,  inDown);
}


void xplRegister()
{
  drefFlapDeploy = XP.registerDataRef(F("sim/cockpit2/controls/flap_handle_deploy_ratio"));
  XP.requestUpdates(drefFlapDeploy, 100, .1);
 
}
 

void xplShutdown()
{
 
}

