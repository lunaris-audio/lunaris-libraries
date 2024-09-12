 #ifndef LunarisDisplay_h
#define LunarisDisplay_h

#include "Arduino.h"

class LunarisDisplay{
  public:

    LunarisDisplay(int dataPin, int pixelCount);                                     // Display settings
    void setWavePointer(int pointerValue, float pointerWidth);          // Prints wave data to display
    void setWave();                                                     // Prints wave data to display
    void setupWave(int hueStart, int hueEnd, int saturation, float waveW, int waveT);   // Change wave settings
    void setupItemMenu(int items, float pointerWidth, int blinkTime);   // Setup item menu
    void setItemMenu(int selectedItem);                                 // Prints item menu to display
    void setItemPointer(int pointerValue);                              // Prints item value to display
    void setItemBoolSelector(bool pointerValue);                        // Prints item selector to display
    void blank();                                                       // Clears the display
    void waveMask(int startPixel, int endPixel, int dimWidth);          // Adds a mask to the wave display of hue offset by 180
    void waveMaskEnable(bool en);                                       // Enables wave mask
  private:

    float brightnessMulti;
        // Basic functions
    void setPixelRGB(int pixel, byte r, byte g, byte b);    // Saves pixel RGB data to array
    void setPixelHSV(int pixel, int h, int s, int v);       // Converts HSV to RGB and saves pixel data to array
    void sendOne();         // Sends 1 on D2
    void sendZero();        // Sends 0 on D2
    int pxCnt;              // Number of display pixels
    int pin;                // Data pin
    void update();          // Sends data to display
    void begin();           // Sets pixel values to black
    int timeLeftPercentage(long timeEnd, int time);         // Converts time left to values from 0 to 255
        // Wave display
    int waveSaturation; // Saturation
    int waveHueStart;   // Starting hue of wave
    int waveHueEnd;     // Ending hue of wave
    float waveWidth;    // Wave width in pixels
    int waveTime;       // Wave passing time in ms
    long waveTimeTemp;  // Temporary variable for keeping track of wave passing time, also used for item display

    int waveMaskStartPixel;
    int waveMaskEndPixel;

    bool waveMaskEn;
    int waveMaskDimWidth;

        // Item display
    int itemNumber;             // Number of items
    float itemMenuPointerWidth; // Item display item pointer width
    int itemMenuBlinkTime;      // Item blink time in ms

    byte pixelData[34][3];        // Array for keeping pixel RGB values
};

#endif
