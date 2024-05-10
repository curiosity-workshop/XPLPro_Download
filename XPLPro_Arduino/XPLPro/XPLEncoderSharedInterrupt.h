//   XPLEncoderSharedInterrupt.h - XPLPro Add-on Header for rotary encoders sharing an interrupt pin via schottky diode isolation 
// // This is NOT FUNCTIONAL AT THIS TIME
//   Created by Curiosity Workshop, Michael Gerlicher,  2024
//   
//   To report problems, download updates and examples, suggest enhancements or get technical support, please visit:
//      discord:  https://discord.gg/gzXetjEST4
//      patreon:  www.patreon.com/curiosityworkshop


// 3 April 2024 -- Initial release


// I didn't encapsulate this into a class as the use of interrupt calls makes it complex.

#ifndef XPLEncoderSharedInterrupt_h
#define XPLEncoderSharedInterrupt_h

// Parameters around the interface

#ifndef XPLESI_MAXENCODERS 
    #define XPLESI_MAXENCODERS 8                       //Default to 10.  
#endif



    //XPLEncoderSharedInterrupt(int interruptPin, void (*esiHandler)(int inEncoder, int inValue));

int XPLESIbegin(int inPin, void(*esiHandler)(int esiID, int esiValue));
int XPLESIaddEncoder(int inPinA, int inPinB);   
void _XPLESIcheck();  
void XPEESIwrite(int inEncoder, int inValue);
int  XPLESIread(int inEncoder);
void XPLESIclear(void);
  
int _XPLESIcount;             // how many are registered
int _XPLESIinterruptPin;         // which is the shared interrupt pin

void (*_XPLESIhandler)(int esiID, int esiValue) = NULL;  // this function will be called when activity is detected on the pot, if not NULL
   
  
struct XPLESIencoder
{
      int pinA;                // connected pin
      int pinB;                 // connected pin

      byte prevStatusPinA;
      byte prevStatusPinB;
      byte currentStatusPinA;
      byte currentStatusPinB;
           
      int currentValue;

   
};

struct XPLESIencoder _XPLesis[XPLESI_MAXENCODERS];


int XPLESIbegin(int interruptPin, void (*esiHandler)(int esiID, int esiValue))
{
    _XPLESIhandler = esiHandler;
    pinMode(interruptPin, INPUT_PULLUP);
    // attach interrupt handler for when the interrupt pin is triggered
    attachInterrupt(digitalPinToInterrupt(interruptPin), _XPLESIcheck, CHANGE);
  
    
};



void XPLESIclear(void)           // call this prior to adding encoders if not the first run
{
    _XPLESIcount = 0;

}



int XPLESIaddEncoder(int inPinA, int inPinB)
{
    if (_XPLESIcount >= XPLESI_MAXENCODERS) return -1;

    pinMode(inPinA, INPUT_PULLUP);
    pinMode(inPinB, INPUT_PULLUP);

    _XPLesis[_XPLESIcount].pinA = inPinA;
    _XPLesis[_XPLESIcount].pinB = inPinB;

    _XPLesis[_XPLESIcount].prevStatusPinA = 0;
    _XPLesis[_XPLESIcount].prevStatusPinB = 0;

    _XPLesis[_XPLESIcount].currentValue = 0;

    return _XPLESIcount++;


}


void _XPLESIcheck()
{
 
    for (int i = 0; i < _XPLESIcount; i++)             // Im doing this first to get the best possible read of the pins
    {
        _XPLesis[i].currentStatusPinA = digitalRead(_XPLesis[i].pinA);
        _XPLesis[i].currentStatusPinB = digitalRead(_XPLesis[i].pinB);
    }

    // Now figure out which one moved.
    for (int i = 0; i < _XPLESIcount; i++)             
    {
     // If the previous and the current state are different, that means a Pulse has occured
      if (_XPLesis[i].currentStatusPinA != _XPLesis[i].prevStatusPinA || _XPLesis[i].currentStatusPinB != _XPLesis[i].prevStatusPinB)
      {
          // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
          if (_XPLesis[i].currentStatusPinA != _XPLesis[i].currentStatusPinB)   _XPLesis[i].currentValue++;
          else                                                                  _XPLesis[i].currentValue--;
          _XPLesis[i].prevStatusPinA = _XPLesis[i].currentStatusPinA;  
          _XPLesis[i].prevStatusPinB = _XPLesis[i].currentStatusPinB;
          _XPLESIhandler(i, _XPLesis[i].currentValue);
      }
      
   }
    
  
}

void XPLESIwrite(int inEncoder, int inValue)          
{
    _XPLesis[inEncoder].currentValue = inValue;

}
int XPLESIread(int inEncoder)        
{
    return _XPLesis[inEncoder].currentValue;

}
#endif