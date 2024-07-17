//   XPLMux9685Servos.h - Library for PCA9685 PWM multiplexer 
//   Created by Curiosity Workshop, Michael Gerlicher,  2024
//   
//   To report problems, download updates and examples, suggest enhancements or get technical support, please visit:
//      discord:  https://discord.gg/gzXetjEST4
//      patreon:  www.patreon.com/curiosityworkshop


#ifndef XPLMux9685Servos_h
#define XPLMux9685Servos_h


#include <Wire.h>                       // This extension relies on the Adafruit PWMServo Driver and Wire libraries so they will need to be installed
#include <Adafruit_PWMServoDriver.h>

// Parameters around the interface

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

#ifndef XPLMUX9685_MAXSERVOS 
#define XPLMUX9685_MAXSERVOS     16                  //Default to 16.  Define this in your main sketch to override
#endif

#define XPLMUX9685_SERVOMIN     150 // This is the 'minimum' pulse length count (out of 4096)
#define XPLMUX9685_SERVOMAX     600 // This is the 'maximum' pulse length count (out of 4096)
#define XPLMUX9685_USMIN        600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define XPLMUX9685_USMAX        2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define XPLMUX9685_SERVO_FREQ   50 // Analog servos run at ~50 Hz updates

/// @brief Core class for the XPLPro Arduino library
class XPLMux9685Servos
{
public:
    /// @brief Constructor
    /// @param inPinSig Pin connection for reading
    /// @param inPinS0, Pin connection for s0
    /// @param inPinS1, Pin connection for s1
    /// @param inPinS2, Pin connection for s2
    /// @param inPinS3, Pin connection for s3
    /// @param muxHandler, function called when pin activity is detected, or NULL
    XPLMux9685Servos(void);

    void begin(XPLPro* xplpro, uint8_t = 0x40);     // 0x40 is the default configuration for the 9685 boards
    void check(inStruct* inData);
    int8_t addServo(dref_handle inHandle, int inElement, uint8_t inNumber, uint16_t inInitialVal);
    void writeServo(uint8_t inServo, uint16_t inValue);



    /// @brief Scan mux pins and call handler if any changes are detected.  Run regularly
    void check(void);

    void clear(void);

private:
    uint8_t _findServo(dref_handle inHandle, int inElement);

    XPLPro* _XP;

    unsigned int _servoCount;

    Adafruit_PWMServoDriver _pwm;

    struct XPLServo
    {
        //   uint8_t muxPin;                     // connected pin

        dref_handle handle;
        int element;
        uint8_t number;
        uint32_t initialVal;

        unsigned long prevTime;            //  time of last change

    };

    struct XPLServo _servos[XPLMUX9685_MAXSERVOS];

};


XPLMux9685Servos::XPLMux9685Servos(void)
{

 
 
}

void XPLMux9685Servos::begin(XPLPro* xplpro, uint8_t inAddress)
{

    // called this way, it uses the default address 0x40
    _pwm = Adafruit_PWMServoDriver(inAddress);
    // you can also call it with a different address you want
    //Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
    // you can also call it with a different address and I2C interface
    //Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

    _pwm.begin();
    /*
     * In theory the internal oscillator (clock) is 25MHz but it really isn't
     * that precise. You can 'calibrate' this by tweaking this number until
     * you get the PWM update frequency you're expecting!
     * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
     * is used for calculating things like writeMicroseconds()
     * Analog servos run at ~50 Hz updates, It is importaint to use an
     * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
     * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
     *    the I2C PCA9685 chip you are setting the value for.
     * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
     *    expected value (50Hz for most ESCs)
     * Setting the value here is specific to each individual I2C PCA9685 chip and
     * affects the calculations for the PWM update frequency.
     * Failure to correctly set the int.osc value will cause unexpected PWM results
     */
    _pwm.setOscillatorFrequency(27000000);
    _pwm.setPWMFreq(XPLMUX9685_SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    delay(10);
    _XP = xplpro;
    clear();

}

void XPLMux9685Servos::clear(void)           // call this prior to adding pins if not the first run
{
    _servoCount = 0;

}

void XPLMux9685Servos::check(inStruct *inData)
{
    int servoNum = _findServo(inData->handle, inData->element);
    if (servoNum >= 0)    
        switch (inData->type)
        {
        case xplmType_Int:
        case xplmType_IntArray:
            writeServo(servoNum, inData->inLong);
            break;

        case xplmType_Float:
        case xplmType_FloatArray:
            writeServo(servoNum, inData->inFloat);
            break; 
        }




}

int8_t XPLMux9685Servos::addServo(dref_handle inHandle, int inElement, uint8_t inNumber, uint16_t inInitialVal)
{
    if (_servoCount >= XPLMUX9685_MAXSERVOS) return -1;

    _servos[_servoCount].handle = inHandle;
    _servos[_servoCount].element = inElement;
    _servos[_servoCount].number = inNumber;
    _servos[_servoCount].initialVal = inInitialVal;
     
    writeServo(inNumber, inInitialVal);

 
    return _servoCount++;

}

void XPLMux9685Servos::writeServo(uint8_t inServo, uint16_t inMicroSeconds)   
{
    _pwm.writeMicroseconds(inServo, inMicroSeconds);
    //_pwm.setPWM(inServo, 0, map(inValue, 0, 180, XPLMUX9685_SERVOMIN, XPLMUX9685_SERVOMAX));
    

}

uint8_t XPLMux9685Servos::_findServo(dref_handle inHandle, int inElement)
{
    int i = 0;
    while (i < _servoCount)
    {
        if (_servos[i].handle == inHandle && _servos[i].element == inElement) return i;
        i++;
    }

    return -1;

}

#endif

