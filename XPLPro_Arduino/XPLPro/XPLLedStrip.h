//   XPLLedStrip.h - Library for LED strips 
//   Created by Curiosity Workshop, Michael Gerlicher,  2024
//   
//   To report problems, download updates and examples, suggest enhancements or get technical support, please visit:
//      discord:  https://discord.gg/gzXetjEST4
//      patreon:  www.patreon.com/curiosityworkshop


#ifndef XPLLedStrip_h
#define XPLLedStrip_h


#include <Adafruit_NeoPixel.h>



// Parameters around the interface


#ifndef XPLLEDSTRIP_MAXPIXELS 
#define XPLLEDSTRIP_MAXPIXELS     16                  //Default to 16.  Define this in your main sketch to override
#endif


/// @brief Core class for the XPLPro Arduino library
class XPLLedStrip
{
public:
   
    XPLLedStrip(void);

    void begin(XPLPro* xplpro, uint16_t inPin);     // pin number
    void check(inStruct* inData);
    void test(void);
    int16_t addPixel(dref_handle inHandle, int inElement, uint16_t inNumber, uint8_t inR, uint8_t inG, uint8_t inB);
    void writePixel(uint16_t inPixel, float inBrightness);
    void check(void);

    void clear(void);

private:
    int16_t _findPixel(dref_handle inHandle, int inElement);
    
    XPLPro* _XP;

    int16_t _pixelCount;

    Adafruit_NeoPixel _strip;

    struct XPLPixel
    {
        //   uint8_t muxPin;                     // connected pin

        dref_handle handle;
        int element;
        uint16_t number;
        uint8_t cRed;
        uint8_t cGreen;
        uint8_t cBlue;


    };

    struct XPLPixel _pixels[XPLLEDSTRIP_MAXPIXELS];

};


XPLLedStrip::XPLLedStrip(void)
{

 
 
}

void XPLLedStrip::begin(XPLPro* xplpro, uint16_t inPin)
{


    // Parameter 1 = number of pixels in strip
    // Parameter 2 = pin number (most are valid)
    // Parameter 3 = pixel type flags, add together as needed:
    //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    _strip = Adafruit_NeoPixel(XPLLEDSTRIP_MAXPIXELS, inPin, NEO_GRB + NEO_KHZ800);
    _strip.begin();
    _XP = xplpro;
    clear();
   
}

void XPLLedStrip::clear(void)           // call this prior to adding pins if not the first run
{
    _pixelCount = 0;
    _strip.clear();
    _strip.fill(_strip.Color(0, 0, 0), 0, XPLLEDSTRIP_MAXPIXELS);
    _strip.show();

}

void XPLLedStrip::test(void)
{
    _strip.clear();

    _strip.fill(_strip.Color(200, 0, 0), 0, XPLLEDSTRIP_MAXPIXELS);
    _strip.show();
    delay(100);

    _strip.fill(_strip.Color(0, 200, 0), 0, XPLLEDSTRIP_MAXPIXELS);
    _strip.show();
    delay(100);

    _strip.fill(_strip.Color(0, 0, 200), 0, XPLLEDSTRIP_MAXPIXELS);
    _strip.show();
    delay(100);

    _strip.fill(_strip.Color(0, 0, 0), 0, XPLLEDSTRIP_MAXPIXELS);
    _strip.show();
    

}

void XPLLedStrip::check(inStruct *inData)
{
    int pixelNum = _findPixel(inData->handle, inData->element);
    if (pixelNum >= 0)    
        switch (inData->type)
        {
        case xplmType_Int:
        case xplmType_IntArray:
            if (inData->inLong) writePixel(pixelNum, 1);
            else
            {
                writePixel(pixelNum, 0);
            }
            
            break;

        case xplmType_Float:
        case xplmType_FloatArray:
            writePixel(pixelNum, inData->inFloat );
           
            break; 
        }




}

//strip1.addPixel(drefTemp, 0, 13, 0, 0, 250);
int16_t XPLLedStrip::addPixel(dref_handle inHandle, int inElement, uint16_t inNumber, uint8_t inR, uint8_t inG, uint8_t inB)
{
    if (_pixelCount >= XPLLEDSTRIP_MAXPIXELS) return -1;

    _pixels[_pixelCount].handle = inHandle;
    _pixels[_pixelCount].element = inElement;
    _pixels[_pixelCount].number = inNumber;
    _pixels[_pixelCount].cRed = inR;
    _pixels[_pixelCount].cGreen = inG;
    _pixels[_pixelCount].cBlue = inB;

    return _pixelCount++;

}

void XPLLedStrip::writePixel(uint16_t inPixel, float inBrightness)   // brightness is percentage*100 0-100
{
 
    _strip.setPixelColor(_pixels[inPixel].number,
            (inBrightness *  _pixels[inPixel].cRed) ,
            (inBrightness *  _pixels[inPixel].cGreen) ,
           (inBrightness *  _pixels[inPixel].cBlue)  );
   
    _strip.show();


}

int16_t XPLLedStrip::_findPixel(dref_handle inHandle, int inElement)
{
    int16_t i = 0;
    while (i < _pixelCount)
    {
        if (_pixels[i].handle == inHandle && _pixels[i].element == inElement) return i;
        i++;
    }

    return -1;

}

#endif

