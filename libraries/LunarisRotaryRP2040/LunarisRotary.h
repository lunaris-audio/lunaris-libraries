#ifndef LunarisRotary_h
#include "Arduino.h"

class LunarisRotary{
  public:

  LunarisRotary(int dataPin, int clockPin, int buttonPin, int longClickTime, int doubleClickTime);    // Rotary encoder settings
  void service();                                             // Interrupt service. Needs to run as often as possible (at least once every 1ms is recommended)
  int change();                                               // Returns the change from the last time this was called
  int buttonStatusRead();                                     // Returns button status: 0 - no action; 1 - regular click; 2 - double-click; 3 - long press

  private:

  int dPin;               // Rotary encoder data pin
  int clPin;              // Rotary encoder clock pin
  int btPin;              // Rotary encoder button pin

  bool oldPosD;           // Previous data value
  bool oldPosCl;          // Previous clock value

  int tempValue;          // Holds change value until change() is called

  int lClTime;            // Holds long-press time in ms
  int dClTime;            // Holds double-clock time in ms

  int buttonState;        // Variable for button press state
  long buttonHoldTime;    // Variable for keeping track of how long the button is held for
  long lastClickTime;     // Last time the button was pressed
  int buttonStatus;       // Holds button status until buttonStatusRead() is called

};

#endif
