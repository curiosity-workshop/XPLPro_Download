//   XPLPro.h - Library for serial interface to Xplane SDK.
//   Created by Curiosity Workshop, Michael Gerlicher,  2020-2023
//   See readme.txt file for information on updates.
//   To report problems, download updates and examples, suggest enhancements or get technical support, please visit:
//      discord:  https://discord.gg/gzXetjEST4
//      patreon:  www.patreon.com/curiosityworkshop

#ifndef XPLPro_h
#define XPLPro_h

#include <Arduino.h>


//////////////////////////////////////////////////////////////
// Parameters that can be overwritten by command line defines
//////////////////////////////////////////////////////////////

// Decimals of precision for floating point datarefs. More increases dataflow (default 4)
#ifndef XPL_FLOATPRECISION
#define XPL_FLOATPRECISION 4
#endif

// Timeout after sending a registration request, how long will we wait for the response.
// This is giant because sometimes xplane says the plane is loaded then does other stuff for a while. (default 90000 ms)
#ifndef XPL_RESPONSE_TIMEOUT
#define XPL_RESPONSE_TIMEOUT 90000
#endif

// For boards with limited memory that can use PROGMEM to store strings.
// You will need to wrap your dataref names with F() macro ie:
// Xinterface.registerDataref(F("laminar/B738/annunciator/drive2"), XPL_READ, 100, 0, &drive2);
// Disable for boards that have issues compiling: errors with strncmp_PF for instance.
#ifndef XPL_USE_PROGMEM
#ifdef __AVR_ARCH__
// flash strings are default on on AVR architecture
#define XPL_USE_PROGMEM 1
#else
// and off otherwise
#define XPL_USE_PROGMEM 0
//#include <avr/dtostrf.h>                // this is needed for non-AVR boards to include the dtostrf function

#endif
#endif

// Package buffer size for send and receive buffer each.
// If you need a few extra bytes of RAM it could be reduced, but it needs to
// be as long as the longest dataref name + 10.  If you are using datarefs
// that transfer strings it needs to be big enough for those too.  (default 200, should be less than 256)
#ifndef XPLMAX_PACKETSIZE_TRANSMIT
#define XPLMAX_PACKETSIZE_TRANSMIT 200
#endif

#ifndef XPLMAX_PACKETSIZE_RECEIVE
#define XPLMAX_PACKETSIZE_RECEIVE 200
#endif

//////////////////////////////////////////////////////////////
// All other defines in this header must not be modified
//////////////////////////////////////////////////////////////

// define whether flash strings will be used
#if XPL_USE_PROGMEM
// use Flash for strings, requires F() macro for strings in all registration calls
typedef const __FlashStringHelper XPString_t;

#else
typedef const char XPString_t;
#endif

typedef int  dref_handle;
typedef int  cmd_handle;
typedef int XPLMCommandKeyID;
typedef int XPLMCommandButtonID;

// Parameters around the interface
#define XPL_BAUDRATE 115200   // Baudrate needed to match plugin
#define XPL_RX_TIMEOUT 500    // Timeout for reception of one frame
#define XPL_PACKETHEADER '['  // Frame start character
#define XPL_PACKETTRAILER ']' // Frame end character
#define XPL_HANDLE_INVALID -1 // invalid handle

// Items in caps generally come from XPlane. Items in lower case are generally sent from the arduino.
#define XPLCMD_SENDNAME 'N'                // plugin request name from arduino
#define XPLRESPONSE_NAME 'n'               // Arduino responds with device name as initialized in the "begin" function
#define XPLRESPONSE_VERSION 'v'             // Arduino responds with build date and time (when sketch was compiled)
#define XPLCMD_SENDREQUEST 'Q'             // plugin sends this when it is ready to register bindings
#define XPLCMD_DATAFLOWPAUSE	'p'		// pause dataflow from plugin
#define XPLCMD_DATAFLOWRESUME  	'q'		// 
#define XPLCMD_SETDATAFLOWSPEED		'f'	// 07/09/2024:  Set maximum number of bytes to send per second.
#define XPLREQUEST_REGISTERDATAREF 'b'     // Register a dataref
#define XPLREQUEST_REGISTERCOMMAND 'm'     // Register a command
#define XPLRESPONSE_DATAREF 'D'            // Plugin responds with handle to dataref or - value if not found.  dataref handle, dataref name
#define XPLRESPONSE_COMMAND 'C'            // Plugin responds with handle to command or - value if not found.  command handle, command name
#define XPLCMD_PRINTDEBUG 'g'              // Plugin logs string sent from arduino
#define XPLCMD_SPEAK 's'                   // plugin speaks string through xplane speech
#define XPLREQUEST_DATAREFTOUCH 'd'         // 05/16/2024 update: force an update to specified dataref.  EXPERIMENTAL and possibly redundant
#define XPLREQUEST_UPDATES 'r'             // arduino is asking the plugin to update the specified dataref with rate and divider parameters
#define XPLREQUEST_UPDATESARRAY 't'        // arduino is asking the plugin to update the specified array dataref with rate and divider parameters
#define XPLREQUEST_UPDATES_TYPE 'y'       // 3/25/2024 update:  some datarefs (looking at you Zibo...) return multiple data types, We can force which one to receive here.
#define XPLREQUEST_UPDATES_TYPE_ARRAY 'w'
#define XPLCMD_SPECIAL '$'                  // 06/02/2024:  Special commands that are useful but xplane considers deprecated.  Turns out they really are deprecated!
#define XPLCMD_SPECIAL_SIMKEYPRESS      1   //              Simulate key press, parameters are  inKeyType, inKey as defined with xplane
#define XPLCMD_SPECIAL_CMDKEYSTROKE     2   //              Command Keystroke, parameter is  XPLMCommandKeyID     inKey
#define XPLCMD_SPECIAL_CMDBUTTONPRESS   3   //              Command Button Press, parameter isXPLMCommandButtonID  inButton
#define XPLCMD_SPECIAL_CMDBUTTONRELEASE 4   //              Command Button Release, parameter isXPLMCommandButtonID  inButton
                                            //                  Be sure to close all button presses with button releases!


// these are the data types for the above requests that we can send.  These values come directly from the Xplane SDK.  The Dataref needs to support the type of data
//          that we are requesting here, refer to the documentation for the dataref.  The XPLDirectError.log also reports the type of data each registered dataref
//          returns, if that is helpful.
    #define xplmType_Unknown	0	//  Data of a type the current XPLM doesn't do.
    #define xplmType_Int	    1	//  A single 4 - byte integer, native endian.
    #define xplmType_Float	    2	//  A single 4 - //byte float, native endian.
    #define xplmType_Double	    4	//  A single 8 - byte double, native endian.
    #define xplmType_FloatArray	8	//  An array of 4 - byte floats, native endian.
    #define xplmType_IntArray	16  //	An array of 4 - byte integers, native endian.
    #define xplmType_Data	    32  //	A variable block of data.

#define XPLREQUEST_SCALING 'u'             // arduino requests the plugin apply scaling to the dataref values
//#define XPLREQUEST_DATAREFVALUE 'e'        // one off request for a dataref value.  Avoid doing this every loop, better to use REQUEST_UPDATES.  Either way, value will be sent via the inbound callback
#define XPLCMD_RESET 'z'                   // Request a reset and reregistration from the plugin
#define XPLCMD_DATAREFUPDATEINT '1'        // Int DataRef update
#define XPLCMD_DATAREFUPDATEFLOAT '2'      // Float DataRef update
#define XPLCMD_DATAREFUPDATEINTARRAY '3'   // Int array DataRef update
#define XPLCMD_DATAREFUPDATEFLOATARRAY '4' // Float array DataRef Update
#define XPLCMD_DATAREFUPDATESTRING '9'     // String DataRef update
#define XPLCMD_COMMANDTRIGGER 'k'          // Trigger command n times
#define XPLCMD_COMMANDSTART 'i'            // Begin command (Button pressed)
#define XPLCMD_COMMANDEND 'j'              // End command (Button released)
#define XPL_EXITING 'X'                    // XPlane sends this to the arduino device during normal shutdown of XPlane. It may not happen if xplane crashes.

struct inStruct // potentially 'class'
{
    dref_handle handle;
    int type;
    int element;
    long inLong;
    float inFloat;
    int strLength;      // if string data, length of string data
    char* inStr;
};

/// @brief Core class for the XPLPro Arduino library
class XPLPro
{
public:
    /// @brief Constructor
    /// @param device Device to use (should be &Serial)
    XPLPro(Stream *device);

    /// @brief Register device and set callback functions
    /// @param devicename Device name
    /// @param initFunction Callback for DataRef and Command registration
    /// @param stopFunction Callback for XPlane shutdown or plane change
    /// @param inboundHandler Callback for incoming DataRefs
    void begin(const char *devicename, void (*initFunction)(void), void (*stopFunction)(void), void (*inboundHandler)(inStruct *));

    /// @brief Return connection status
    /// @return True if connection to XPlane established
    int connectionStatus();

    /// @brief Trigger a command once
    /// @param commandHandle of the command to trigger
    /// @return 0: OK, -1: command was not registered
    int commandTrigger(cmd_handle commandHandle) { return commandTrigger(commandHandle, 1); };

    /// @brief Trigger a command multiple times
    /// @param commandHandle Handle of the command to trigger
    /// @param triggerCount Number of times to trigger the command
    /// @return 0: OK, -1: command was not registered
    int commandTrigger(cmd_handle commandHandle, int triggerCount);

    /// @brief Start a command. All commandStart must be balanced with a commandEnd
    /// @param commandHandle Handle of the command to start
    /// @return 0: OK, -1: command was not registered
    int commandStart(cmd_handle commandHandle);

    /// @brief End a command. All commandStart must be balanced with a commandEnd
    /// @param commandHandle Handle of the command to start
    /// @return 0: OK, -1: command was not registered
    int commandEnd(cmd_handle commandHandle);

    void simulateKeyPress(int inKeyType, int inKey);
    void commandKeyStroke(XPLMCommandKeyID inKey);
    void commandButtonPress(XPLMCommandButtonID  inButton);
    void commandButtonRelease(XPLMCommandButtonID  inButton);

    /// @brief Write an integer DataRef.
    /// @param handle Handle of the DataRef to write
    /// @param value Value to write to the DataRef
    void datarefWrite(dref_handle handle, long value);

    /// @brief Write an integer DataRef. Maps to long DataRefs.
    /// @param handle Handle of the DataRef to write
    /// @param value Value to write to the DataRef
    void datarefWrite(dref_handle handle, int value);

    /// @brief Write a Integer DataRef to an array element.
    /// @param handle Handle of the DataRef to write
    /// @param value Value to write to the DataRef
    /// @param arrayElement Array element to write to
    void datarefWrite(dref_handle handle, long value, int arrayElement);

    /// @brief Write a Integer DataRef to an array element. Maps to long DataRefs.
    /// @param handle Handle of the DataRef to write
    /// @param value Value to write to the DataRef
    /// @param arrayElement Array element to write to
    void datarefWrite(dref_handle handle, int value, int arrayElement);

    /// @brief Write a float DataRef.
    /// @param handle Handle of the DataRef to write
    /// @param value Value to write to the DataRef
    void datarefWrite(dref_handle handle, float value);

    /// @brief Write a float DataRef to an array element.
    /// @param handle Handle of the DataRef to write
    /// @param value Value to write to the DataRef
    /// @param arrayElement Array element to write to
    void datarefWrite(dref_handle handle, float value, int arrayElement);
   
    /// @brief Force plugin to update dataref value.  Experimental and probably redundant, use sparingly!
    /// @param handle Handle of the DataRef to write
    void datarefTouch(dref_handle handle);

    /// @brief Request DataRef updates from the plugin
    /// @param handle Handle of the DataRef to subscribe to
    /// @param rate Maximum rate for updates to reduce traffic
    /// @param precision Floating point precision
    void requestUpdates(dref_handle handle, int rate, float precision);

    /// @brief Request DataRef updates from the plugin for an array DataRef
    /// @param handle Handle of the DataRef to subscribe to
    /// @param rate Maximum rate for updates to reduce traffic
    /// @param precision Floating point precision
    /// @param arrayElement Array element to subscribe to
    void requestUpdates(dref_handle handle, int rate, float precision, int arrayElement);

    /// @brief Request DataRef updates from the plugin
    /// @param handle Handle of the DataRef to subscribe to
    /// @param type Specific type of data to request, see header file
    /// @param rate Maximum rate for updates to reduce traffic
    /// @param precision Floating point precision
    void requestUpdatesType(dref_handle handle, int type, int rate, float precision);

    /// @brief Request DataRef updates from the plugin for an array DataRef
    /// @param handle Handle of the DataRef to subscribe to
    /// @param type Specific type of data to request, see header file
    /// @param rate Maximum rate for updates to reduce traffic
    /// @param precision Floating point precision
    /// @param arrayElement Array element to subscribe to
    void requestUpdatesType(dref_handle handle, int type, int rate, float precision, int arrayElement);

    /// @brief set scaling factor for a DataRef (offload mapping to the plugin)
    void setScaling(dref_handle handle, long int inLow, long int inHigh, long int outLow, long int outHigh);

    /// @brief Register a DataRef and obtain a handle
    /// @param datarefName Name of the DataRef (or abbreviation)
    /// @return Assigned handle for the DataRef, -1 if DataRef was not found
    int registerDataRef(XPString_t *datarefName);

    /// @brief Register a Command and obtain a handle
    /// @param commandName Name of the Command (or abbreviation)
    /// @return Assigned handle for the Command, -1 if Command was not found
    int registerCommand(XPString_t *commandName);

    
    /// @brief Send a debug message to the plugin
    /// @param msg Message to show as debug string
    /// @return
    int sendDebugMessage(const char *msg);

    /// @brief Send a speech message to the plugin
    /// @param msg Message to speak
    /// @return
    int sendSpeakMessage(const char *msg);

    /// @brief Request a reset from the plugin
    void sendResetRequest(void);

    void dataFlowPause(void);           // Tell plugin to pause flow of data to device
    void dataFlowResume(void);          // resume
    void setDataFlowSpeed(unsigned long);       // request speed throttling on data in bytes per second.  Understand that a full packet will always be sent but the throttling occurs between packets.
    int getBufferStatus(void);          // returns number of bytes in receive buffer

    /// @brief Cyclic loop handler, must be called in idle task
    /// @return Connection status
    int xloop();

    
    
private:
    void _processSerial();
    int _receiveNSerial(int inSize);
    void _processPacket();
    void _transmitPacket();
    void _sendname();
    void _sendVersion();
    void _sendPacketVoid(int command, int handle);              // just a command with a handle
    void _sendPacketVoid(int command);                          // Update 2024 May 23:  just a command 
    void _sendPacketString(int command, const char *str);       // send a string
    int _parseInt(int *outTarget, char *inBuffer, int parameter);
    int _parseInt(long *outTarget, char *inBuffer, int parameter);
    int _parseFloat(float *outTarget, char *inBuffer, int parameter);
    int _parseString(char *outBuffer, char *inBuffer, int parameter, int maxSize);
    int Xdtostrf(double val, signed char width, unsigned char prec, char* sout);

    Stream *_streamPtr;
    const char *_deviceName;
    bool _registerFlag;
    bool _connectionStatus;
    inStruct _inData;

    char _sendBuffer[XPLMAX_PACKETSIZE_TRANSMIT];
    char _receiveBuffer[XPLMAX_PACKETSIZE_RECEIVE];
    uint8_t _receiveBufferBytesReceived;

    void (*_xplInitFunction)(void);  // this function will be called when the plugin is ready to receive binding requests
    void (*_xplStopFunction)(void);  // this function will be called with the plugin receives message or detects xplane flight model inactive
    void (*_xplInboundHandler)(inStruct *); // this function will be called when the plugin sends dataref values

    dref_handle _handleAssignment;
 
};

#endif