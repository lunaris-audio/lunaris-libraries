#include "Arduino.h"
#include "LunarisRotary.h"

LunarisRotary::LunarisRotary(int dataPin, int clockPin, int buttonPin, int longClickTime, int doubleClickTime) {
    dPin = dataPin;
    clPin = clockPin;
    btPin = buttonPin;

    // Setting pin modes

    pinMode(dPin, INPUT);
    pinMode(clPin, INPUT);
    pinMode(btPin, INPUT);

    oldPosD = 0;
    oldPosCl = 0;
    tempValue = 0;

    lClTime = longClickTime;
    dClTime = doubleClickTime;
    buttonState = 0;
    lastClickTime = 0;
}

void LunarisRotary::service(){
    cli();
    
    // Rotary encoder

    bool tempPosD = digitalRead(dPin);
    bool tempPosCl = digitalRead(clPin);
    if(oldPosD == 1 && tempPosD == 0){
        oldPosD = 0;
        if(oldPosCl == 1){
            tempValue--;
        }
        else{
            tempValue++;
        }
     }
    if(oldPosCl == 1 && tempPosCl == 0){
        oldPosCl = 0;
        if(oldPosD == 1){
            tempValue++;
        }
        else{
            tempValue--;
        }
     }
     if(tempPosD == 1 && tempPosCl == 1){
        oldPosCl = 1;
        oldPosD = 1;
     }

     // Button
     // 0 - not pressed yet
     // 1 - pressed but not yet released
     // 2 - pressed and released
     // 3 - activated

     if(buttonState == 0 && digitalRead(btPin) == 0){
        lastClickTime = millis();
        buttonState = 1;
     }
     if(buttonState == 1){
        if(lastClickTime + lClTime > millis() && digitalRead(btPin) == 1) buttonState = 2;
        else if(lastClickTime + lClTime < millis()){
            buttonState = 3;
            buttonStatus = 3;
        }
     }
     if(buttonState == 2){
        if(lastClickTime + dClTime < millis()){
            buttonState = 3;
            buttonStatus = 1;
        }
        else if(digitalRead(btPin) == 0){
            buttonState = 3;
            buttonStatus = 2;
        }
     }
     if(buttonState == 3 && digitalRead(btPin) == 1){
        buttonState = 0;
     }
     sei();
}

int LunarisRotary::change(){
    int tempChange = tempValue;
    tempValue = 0;
    return(tempChange);
}
int LunarisRotary::buttonStatusRead (){
    int tempStatus = buttonStatus;
    buttonStatus = 0;
    return(tempStatus);
}