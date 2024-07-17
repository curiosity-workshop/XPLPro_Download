Curiosity Workshop XPLPro Arduino Library - Library for serial interface to Xplane SDK.
  
  Created by Michael Gerlicher, Curiosity Workshop,  September 2020 - 2023.

  To report problems, download updates and examples, suggest enhancements or get technical support, please visit one of the following:

     discord:  	https://discord.gg/gzXetjEST4
     patreon: 	www.patreon.com/curiosityworkshop
     youtube:	https://www.youtube.com/channel/UCISdHdJIundC-OSVAEPzQIQ
     facebook:  https://www.facebook.com/curiosity.workshop42

  Please support this project in one of the following ways:

        Free:  like and subscribe to our youtube channel!
        Support us at patreon.com/curiosityworkshop
        
Known issues:
    
    Arduino Uno with I2c display seems to interfere with serial dialog between the arduino and Xplane.
    Incompatibility with some bluetooth serial port installations, try disabling them if you have issues.

    Remember to close your serial monitor if it is open, it will tie up the serial port and your device won't be found.

Wish List

    -- additional scaling points for data (some gauges are not linear) and individual scales for array element datarefs
    -- exclusion of com ports, some com port devices cause issues when opened/closed/opened


Updates:

-- XP.setDataFlowSpeed(1000);  // max bytes per second plugin will send.  



-- Library now sends data type in the inStruct.  It follows the native xplane format:

    #define xplmType_Unknown	0	//  Data of a type the current XPLM doesn't do.
    #define xplmType_Int	    1	//  A single 4 - byte integer, native endian.
    #define xplmType_Float	    2	//  A single 4 - byte float, native endian.
    #define xplmType_Double	    4	//  A single 8 - byte double, native endian.
    #define xplmType_FloatArray	8	//  An array of 4 - byte floats, native endian.
    #define xplmType_IntArray	16  //	An array of 4 - byte integers, native endian.
    #define xplmType_Data	    32  //	A variable block of data.

-- fixed plugin bug with data types for comparing
-- in Serial log unprintable characters now are represented by their hex value (0xXX)
    void dataFlowPause(void);           // Mostly for internal use but can be useful if your device is busy updating a display screen or whatever
    void dataFlowResume(void);
    int getBufferStatus(void);          // returns number of bytes in receive buffer
    void setDataFlowSpeed(unsigned long int);       // Set max bytes to send per second.  The plugin will always send a complete packet then wait until 
            `                                       // the time catches up (in terms of flight loops) so take that into consideration.

    -- when requesting updates to data datarefs, specify the maximum digits to return in the precision specifier.

    16 May 2024

    -- added datarefTouch(dataref) method to ask the plugin to update specified dataref.  This is experimental, probably redundant and should be used sparingly lest
        much dataflow be induced.  Only use this if you know why you might want to use it.


    10 May 2024

    -- Data scaling now enabled for dataref values sent from the plugin to the arduino.

    -- Improvement made to send only amount of digits requested with "precision" parameter on float datarefs, reducing dataflow from the plugin to the arduino.

    -- Improvements made within the library to reduce creation of buffers for data transfer.  Thanks GioCC!

    -- Logic put in to force the transmission of a subscribed dataref after "requestUpdates" has been called.  This improves synchronization issues.

    -- Some datarefs return that they support multiple data types and may not update them all consistently (looking at you Zibo...).  We can now
        specify the data type that we want when we request updates with the new functions:

            /// @brief Request DataRef updates from the plugin
            /// @param handle Handle of the DataRef to subscribe to
            /// @param type Specific type of data to request, see header file
            /// @param rate Maximum rate for updates to reduce traffic
            /// @param precision Floating point precision
                void requestUpdatesType(int handle, int type, int rate, float precision);

            /// @brief Request DataRef updates from the plugin for an array DataRef
            /// @param handle Handle of the DataRef to subscribe to
            /// @param type Specific type of data to request, see header file
            /// @param rate Maximum rate for updates to reduce traffic
            /// @param precision Floating point precision
            /// @param arrayElement Array element to subscribe to
                void requestUpdatesType(int handle, int type, int rate, float precision, int arrayElement);

         type can be one of the following xplane data types, as defined in the Xplane SDK:

                #define xplmType_Int	    1	//  A single 4 - byte integer, native endian.
                #define xplmType_Float	    2	//  A single 4 - //byte float, native endian.
                #define xplmType_Double	    4	//  A single 8 - byte double, native endian.
                #define xplmType_FloatArray	8	//  An array of 4 - byte floats, native endian.
                #define xplmType_IntArray	16  //	An array of 4 - byte integers, native endian.
                #define xplmType_Data	    32  //	A variable block of data.

        You can continue to use the original requestUpdates functions, it will default to whatever type the dataref reports when registered.

   -- Rewrite of the request updates functions to reduce iterations on each cycle, improving performance and simplicity of code
    -- added debug messages (sent from XPLPro device) to the status window.  They also appear in the XPLProError.log file.
  
	-- minor bug fixes in examples
	-- added XPLSequencer for making timed macro style events.  Airplane startup?
        -- added XPLProArraysExample 
        -- added array capability to XPLPotentiometers.h and XPLSwitches.h.  See the example for information
    -- Added other examples


    06 Mar 2024:

        -- Examples are now contained within the xplpro library folder to ease distribution and so they will show up in the arduino IDE examples under xplpro.

        -- Added XPLProMax72XX example sketch for connecting 72xx series LED displays.  

        -- Arduino library now sends build version, this is derived automatically from the date and time of compilation.  It will be utilized in future updates.
        
        -- Added XPLSwitches.h to the library, as well as an example.  This takes care of common uses of switches used to control xplane and extensively simplifies coding.

        -- Added XPLMuxIn4067.h to the library, as well as an example.  This handles connection to a 4067 multiplexer, use 5 arduino pins to receive 16 inputs.  
           Use another arduino pin to add another 16 inputs.
           

    03 Mar 2024:
        -- String functionality implemented
        -- Several functionality bugs squashed

    16 May 2023:  
        --  When sending data to the plugin long ints were converted to ints in some cases.  This has been resolved.
        
        -- Additional examples added.

 



        ToDo:
        
        -- Add scaling to outbound (from the plugin) data.  Inbound data currently supported.
        -- minor floating point data transfer issues
	    -- Some serial devices that aren't programmed with XPLPro can cause crashes on subsequent loads.
        -- Bug fix, datarefs are updated after subsequent device registrations (disengage/reengage)
       