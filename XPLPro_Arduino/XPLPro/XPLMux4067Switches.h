//   XPLMux4067Switches.h - Library for 4067 multiplexer 
//   Created by Curiosity Workshop, Michael Gerlicher,  2024
//   
//   To report problems, download updates and examples, suggest enhancements or get technical support, please visit:
//      discord:  https://discord.gg/gzXetjEST4
//      patreon:  www.patreon.com/curiosityworkshop

// 2024 May 11 update: standardizing type definitions
// 2024 May 7 update:  Removed bitmap vector to simplify code.  Thanks GioCC!

#ifndef XPLMux4067Switches_h
#define XPLMux4067Switches_h

// Parameters around the interface
#define XPLMUX4067_DEBOUNCETIME 50
#define XPLMUX4067_PRESSED      0
#define XPLMUX4067_RELEASED     1

#define XPLMUX4067_SENDTOHANDLER   0                   // Default is to send switch events to the supplied handler.  This always occurs regardless.
#define XPLMUX4067_DATAREFWRITE    1                   // Update dataref with switch status 
#define XPLMUX4067_COMMANDTRIGGER  2                   // Trigger command with pressed
#define XPLMUX4067_COMMANDSTARTEND 3                   // Start command when pressed, end command when released
#define XPLMUX4067_DATAREFWRITE_INVERT 4               // same as datarefwrite but invert the signal

/// @brief Core class for the XPLPro Arduino library
class XPLMux4067Switches
{
public:
    /// @brief Constructor
    /// @param inPinSig Pin connection for reading
    /// @param inPinS0, Pin connection for s0
    /// @param inPinS1, Pin connection for s1
    /// @param inPinS2, Pin connection for s2
    /// @param inPinS3, Pin connection for s3
    /// @param muxHandler, function called when pin activity is detected, or NULL
    XPLMux4067Switches(uint8_t inPinSig, uint8_t inPinS0, uint8_t inPinS1, uint8_t inPinS2, uint8_t inPinS3, void (*muxHandler)(uint8_t muxChannel, uint8_t muxValue));

    void begin(XPLPro* xplpro);

    int8_t addPin(uint8_t inPin, uint8_t inMode, unsigned int inHandle);

    int8_t getHandle(uint8_t inPin);

  
    /// @brief Scan mux pins and call handler if any changes are detected.  Run regularly
    void check(void);  

    void clear(void);
    
private:

  XPLPro* _XP;          
  unsigned int _maxSwitches;
  unsigned int _switchCount;

  void (*_muxHandler)(uint8_t muxChannel, uint8_t muxValue) = NULL;  // this function will be called when activity is detected on the mux
   
  uint8_t _pinS0;
  uint8_t _pinS1;
  uint8_t _pinS2;
  uint8_t _pinS3;
  uint8_t _pinSig;
   
  struct XPLSwitch
  {
      uint8_t muxPin;                     // connected pin
      uint8_t prevStatus;              //  last known status
      uint8_t mode;
      unsigned int  handle;
      unsigned long prevTime;            //  time of last change

  };

  struct XPLSwitch _switches[16];
 // struct XPLSwitch _switches[];
};


XPLMux4067Switches::XPLMux4067Switches(uint8_t inPinSig, uint8_t inPinS0, uint8_t inPinS1, uint8_t inPinS2, uint8_t inPinS3, void (*muxHandler)(uint8_t inChannel, uint8_t inValue))
{

  _pinSig = inPinSig;         pinMode(_pinSig, INPUT_PULLUP); 
  _pinS0 = inPinS0;           pinMode(_pinS0, OUTPUT);            digitalWrite(_pinS0, LOW);
  _pinS1 = inPinS1;           pinMode(_pinS1, OUTPUT);            digitalWrite(_pinS1, LOW);
  _pinS2 = inPinS2;           pinMode(_pinS2, OUTPUT);            digitalWrite(_pinS2, LOW);
  _pinS3 = inPinS3;           pinMode(_pinS3, OUTPUT);            digitalWrite(_pinS3, LOW);

  _muxHandler = muxHandler;
 // _maxSwitches = inMaxSwitches;
 // if (_maxSwitches > 16) _maxSwitches = 16;                 // max for 4067 mux
  _maxSwitches = 16;                // my intention is to make this dynamic but it is problematic
 //switches = new struct XPLSwitch[ _maxSwitches];
 
};

void XPLMux4067Switches::begin(XPLPro* xplpro)
{
    _XP = xplpro;
    clear();

}

void XPLMux4067Switches::clear(void)           // call this prior to adding pins if not the first run
{
    _switchCount = 0;

}

int8_t XPLMux4067Switches::addPin(uint8_t inPin, uint8_t inMode, unsigned int inHandle)
{
    if (_switchCount >= _maxSwitches) return -1;

    _switches[_switchCount].muxPin = inPin;
    _switches[_switchCount].mode = inMode;
    _switches[_switchCount].handle = inHandle;
    _switches[_switchCount].prevStatus = -1;                // this will force it to update to the plugin.
  
    return _switchCount++;

}

int8_t XPLMux4067Switches::getHandle(uint8_t inPin)
{
    for (uint8_t i = 0; i < _maxSwitches; i++) if (_switches[i].muxPin == inPin) return _switches[i].handle;
    return -1;

}


void XPLMux4067Switches::check(void)
{
 
  unsigned long timeNow = millis();
  for (uint8_t i = 0; i < _switchCount; i++)
  { 
       
    const byte ch = _switches[i].muxPin;
    digitalWrite(_pinS0, ch & 0x01);
    digitalWrite(_pinS1, ch & 0x02);
    digitalWrite(_pinS2, ch & 0x04);
    digitalWrite(_pinS3, ch & 0x08);
    
    uint8_t pinValue = digitalRead(_pinSig);

    if (pinValue != _switches[i].prevStatus && timeNow - _switches[i].prevTime >= XPLMUX4067_DEBOUNCETIME)
    {
      _switches[i].prevStatus = pinValue;
      _switches[i].prevTime   = timeNow;

      switch (_switches[i].mode)
      {

      case XPLMUX4067_DATAREFWRITE:
          _XP->datarefWrite(_switches[i].handle, pinValue);
          break;

      case XPLMUX4067_DATAREFWRITE_INVERT:
          _XP->datarefWrite(_switches[i].handle, !pinValue);
          break;

      case XPLMUX4067_COMMANDTRIGGER:
          if (pinValue == XPLMUX4067_PRESSED) _XP->commandTrigger(_switches[i].handle);
          break;

      case XPLMUX4067_COMMANDSTARTEND:
          if (pinValue == XPLMUX4067_PRESSED)     _XP->commandStart(_switches[i].handle);
          if (pinValue == XPLMUX4067_RELEASED)    _XP->commandEnd(_switches[i].handle);
          break;


      }


      if (_muxHandler != NULL) _muxHandler(_switches[i].muxPin, pinValue);
    }

  }
 
}

#endif