//   XPLSequencer.h - XPLPro Add-on Library for event sequencing
//   Created by Curiosity Workshop, Michael Gerlicher,  2024
//   
//   To report problems, download updates and examples, suggest enhancements or get technical support, please visit:
//      discord:  https://discord.gg/gzXetjEST4
//      patreon:  www.patreon.com/curiosityworkshop
//      facebook: https://www.facebook.com/curiosity.workshop42

// 23 March 2024 -- Fixed bug where the first event would occur immediately rather than at its scheduled time
// 22 March 2024 -- Initial release


#ifndef XPLSequencer_h
#define XPLSequencer_h


#ifndef XPLSEQUENCER_MAXEVENTS 
    #define XPLSEQUENCER_MAXEVENTS     10                  //  Max amount of events to allow.  Default to 10.  
#endif


/// @brief Core class for the XPLPro Potentiometers Addon
class XPLSequencer
{
public:
    /// @brief Constructor
    /// @param potHandler, Function called when pot activity detected, or NULL if not needed
    XPLSequencer(void (*eventHandler)(int id));

    /// <summary>
    /// @brief begin
    /// </summary>
    /// <param name="xplpro"></param>
    void begin(void);

    int addEvent(unsigned long inTime);
   
    /// @brief Scan pins and call handler if any changes are detected.  Run regularly
    void check(unsigned long inTimeNow);  
    void trigger(void);

    void clear(void);
    
private:
 
     
  int _eventCount;             // how many are registered
  int _sequenceCounter;         // where are we in the sequence or -1 if not active
  unsigned long _previousEventTime;

  
    void (*_eventHandler)(int inItem) = NULL;  // this function will be called when events are triggered.  
   
  
  struct XPLEvents
  {
          
      unsigned long timeSchedule;   //  when this event will occur after the previous event in the sequence (ms)
     
  };

  struct XPLEvents _events[XPLSEQUENCER_MAXEVENTS];

};


XPLSequencer::XPLSequencer(void (*eventHandler)(int id))
{

   _eventHandler = eventHandler;
   _sequenceCounter = -1;       // start off inactive

};


void XPLSequencer::clear(void)    
{
    _eventCount = 0;

}


int XPLSequencer::addEvent(unsigned long inTime)
{

    if (_eventCount >= XPLSEQUENCER_MAXEVENTS) return -1;

    _events[_eventCount].timeSchedule = inTime;
    return _eventCount++;
}


void XPLSequencer::check(unsigned long inTimeNow)
{
 
    if (_sequenceCounter == -1) return;
 
    if (inTimeNow - _previousEventTime < _events[_sequenceCounter].timeSchedule) return;        // not time for the next event yet

  
    _previousEventTime = inTimeNow;
    
    if (_eventHandler != NULL) _eventHandler(_sequenceCounter);

    if (++_sequenceCounter > _eventCount) _sequenceCounter = -1;                // end of sequence reached




}

void XPLSequencer::trigger()
{
    _sequenceCounter = 0;           // start the sequence
    _previousEventTime = millis();
}

#endif