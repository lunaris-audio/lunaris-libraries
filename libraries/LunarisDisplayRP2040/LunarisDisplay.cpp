#include "Arduino.h"
#include "LunarisDisplay.h"

LunarisDisplay::LunarisDisplay(int pn, int pixelCount) {
	pxCnt = pixelCount;
    pin = pn;
	pinMode(pin, OUTPUT);
    begin();
    waveTimeTemp = 0;
    waveMaskStartPixel = pixelCount;
    waveMaskEndPixel = pixelCount;
    waveMaskEn = 0;
    brightnessMulti = 0.25;
}

void LunarisDisplay::begin(){
	for (int n = 0; n < pxCnt; n++) {
		setPixelRGB(n, 0, 0, 0);
	}
}
void LunarisDisplay::blank(){
    begin();
    cli();
    update();
    sei();
}

void LunarisDisplay::waveMaskEnable(bool en){
    waveMaskEn = en;
}

void LunarisDisplay::waveMask(int startPixel, int endPixel, int dimWidth){
    waveMaskStartPixel = startPixel;
    waveMaskEndPixel = endPixel;
    waveMaskDimWidth = dimWidth;
}
// Initial wave animation settings
void LunarisDisplay::setupWave(int hueStart, int hueEnd, int saturation, float width, int time) {
    waveSaturation = saturation;
    waveHueStart = hueStart;
    waveHueEnd = hueEnd;
    waveWidth = width;
    waveTime = time;
    waveTimeTemp = millis() + waveTime;
}

void LunarisDisplay::setPixelRGB(int pixel, byte r, byte g, byte b) {
    int red = r * brightnessMulti;
    int green  = g * brightnessMulti;
    int blue = b * brightnessMulti;
    if(red > 254) r = 254;
    if(green > 254) g = 254;
    if(blue > 254) b = 254;
	pixelData[pixel][0] = green;
	pixelData[pixel][1] = red;
	pixelData[pixel][2] = blue;
}

void LunarisDisplay::setPixelHSV(int pixel, int h, int s, int v) {
    h = h % 360;
    s = s % 256;
    v = v % 256;
    float hh, p, q, t, ff;
    long i;
    int r, g, b;

    if (s <= 0) {
        r = v;
        g = v;
        b = v;
        setPixelRGB(pixel, v, v, v);
        return;
    }

    hh = h / 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s / 255.0);
    q = v * (1.0 - (s / 255.0) * ff);
    t = v * (1.0 - (s / 255.0) * (1.0 - ff));

    switch (i) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    default:
        r = v;
        g = p;
        b = q;
        break;
    }

    // Apply gamma correction
    r = (int)(pow((float)r / 255.0, 2.2) * 255);
    g = (int)(pow((float)g / 255.0, 2.2) * 255);
    b = (int)(pow((float)b / 255.0, 2.2) * 255);

    setPixelRGB(pixel, r, g, b);
}

int LunarisDisplay::timeLeftPercentage(long timeEnd, int time) {
    timeEnd = timeEnd - millis();
    timeEnd = timeEnd * 255 / time;
    if (timeEnd > 0) return(timeEnd);
    else return(0);
}

void LunarisDisplay::setWavePointer(int pointerValue, float pointerWidth) {
    begin();
    if (timeLeftPercentage(waveTimeTemp, waveTime) == 0) waveTimeTemp = millis() + waveTime;
    int offset = timeLeftPercentage(waveTimeTemp, waveTime);
    int valStart = (pointerValue * pxCnt) - (pointerWidth * 127);
    int valEnd = (pointerValue * pxCnt) + (pointerWidth * 127);
    for (int n = 0; n < pxCnt; n++) {

        int startOverlap = max(valStart, n * 255);
        int endOverlap = min(valEnd, n * 255 + 255);
        int tempBright = 255;
        if (startOverlap < endOverlap) {
            tempBright = endOverlap - startOverlap;
            if (tempBright > 255) tempBright = 255;
        }
         else tempBright = 0;

        int wavePos = abs(offset + 255 * n / waveWidth);
        wavePos = wavePos % 255;
        if (wavePos < 127) {
            setPixelHSV(n, map(wavePos, 0, 127, waveHueStart, waveHueEnd), waveSaturation, tempBright);
        }
        else if (wavePos >= 127) {
            setPixelHSV(n, map(wavePos, 255, 127, waveHueStart, waveHueEnd), waveSaturation, tempBright);
        }
    }
    cli();
    update();
    sei();
}

void LunarisDisplay::setWave() {
    begin();
    if (timeLeftPercentage(waveTimeTemp, waveTime) == 0) waveTimeTemp = millis() + waveTime;
    int offset = timeLeftPercentage(waveTimeTemp, waveTime);
    for (int n = 0; n < pxCnt; n++) {
        int wavePos = abs(offset + 255 * n / waveWidth);
        wavePos = wavePos % 255;
        if (wavePos < 127) {
            setPixelHSV(n, map(wavePos, 0, 127, waveHueStart, waveHueEnd), waveSaturation, 255);
        }
        else if (wavePos >= 127) {
            setPixelHSV(n, map(wavePos, 255, 127, waveHueStart, waveHueEnd), waveSaturation, 255);
        }
    }
    if(waveMaskEn == 1){
        for(int n = waveMaskStartPixel - waveMaskDimWidth; n < waveMaskStartPixel; n++){
            setPixelRGB(n, 0, 0, 0);
        }
        for(int n = waveMaskStartPixel; n <= waveMaskEndPixel; n++){
            setPixelHSV(n, waveHueEnd + 180, 140,255);
        }
        for(int n = waveMaskEndPixel + waveMaskDimWidth; n > waveMaskEndPixel; n--){
            setPixelRGB(n, 0, 0, 0);
        }
    }
    cli();
    update();
    sei();
}

void LunarisDisplay::setupItemMenu(int items, float pointerWidth, int blinkTime) {
    itemNumber = items;
    itemMenuPointerWidth = pointerWidth;
    itemMenuBlinkTime = blinkTime;
}

void LunarisDisplay::setItemMenu(int selectedItem) {
    begin();
    if (waveTimeTemp - millis() < 0 || waveTimeTemp - millis() > itemMenuBlinkTime) waveTimeTemp = millis() + itemMenuBlinkTime;
    int itemBoundries[itemNumber * 2];
    for (int n = 0; n < itemNumber; n++) {
        int itemMiddle = (pxCnt * 255 - itemMenuPointerWidth * 255) / (itemNumber - 1) * n + (itemMenuPointerWidth * 127);
        itemBoundries[n * 2] = itemMiddle - itemMenuPointerWidth * 127;
        itemBoundries[n * 2 + 1] = itemMiddle + itemMenuPointerWidth * 127;
    }

    int tempItem = 0;

    for (int n = 0; n < pxCnt; n++) {
        int tempBright = 0;

        if (itemBoundries[tempItem * 2 + 1] > n * 255) {
            int startOverlap = max(itemBoundries[tempItem * 2], n * 255);
            int endOverlap = min(itemBoundries[tempItem * 2 + 1], n * 255 + 255);
            if (startOverlap < endOverlap) tempBright = endOverlap - startOverlap;
            if (tempItem == selectedItem && millis() < waveTimeTemp - itemMenuBlinkTime / 2) tempBright = 0;
            setPixelRGB(n, tempBright, tempBright, tempBright);
        }
        else {
            tempItem++;
            int startOverlap = max(itemBoundries[tempItem * 2], n * 255);
            int endOverlap = min(itemBoundries[tempItem * 2 + 1], n * 255 + 255);
            if (startOverlap < endOverlap) tempBright = endOverlap - startOverlap;
            setPixelRGB(n, tempBright, tempBright, tempBright);
        }
    }
    cli();
    update();
    sei();
}

void LunarisDisplay::setItemPointer(int pointerValue) {
    begin();
    int valStart = (pointerValue * pxCnt) - (itemMenuPointerWidth * 127);
    int valEnd = (pointerValue * pxCnt) + (itemMenuPointerWidth * 127);
    for (int n = 0; n < pxCnt; n++) {

        int startOverlap = max(valStart, n * 255);
        int endOverlap = min(valEnd, n * 255 + 255);
        int tempBright = 255;
        if (startOverlap < endOverlap) {
            tempBright = endOverlap - startOverlap;
            if (tempBright > 255) tempBright = 255;
        }
        else tempBright = 0;
        if(pointerValue > 125 && pointerValue < 129) setPixelRGB(n, tempBright, tempBright, 0);
        else setPixelRGB(n, tempBright / 2, tempBright, tempBright);
    }
    cli();
    update();
    sei();
}

void LunarisDisplay::setItemBoolSelector(bool pointerValue) {
    begin();
    if (waveTimeTemp - millis() < 0 || waveTimeTemp - millis() > itemMenuBlinkTime) waveTimeTemp = millis() + itemMenuBlinkTime;
    int valStart = 0;
    int valEnd = 255 * itemMenuPointerWidth;
    int valStart2 = 255 * pxCnt - 255 * itemMenuPointerWidth;
    int valEnd2 = 255 * pxCnt;
    for (int n = 0; n < pxCnt; n++) {

        int startOverlap = max(valStart, n * 255);
        int endOverlap = min(valEnd, n * 255 + 255);
        int startOverlap2 = max(valStart2, n * 255);
        int endOverlap2 = min(valEnd2, n * 255 + 255);
        int tempBright = 255;
        if (startOverlap < endOverlap) {
            tempBright = endOverlap - startOverlap;
            if (tempBright > 255) tempBright = 255;
            if (pointerValue == 0 && millis() < waveTimeTemp - itemMenuBlinkTime / 2) tempBright = 0;
            setPixelHSV(n, 350, 255, tempBright);
        }
        else if (startOverlap2 < endOverlap2) {
            tempBright = endOverlap2 - startOverlap2;
            if (tempBright > 255) tempBright = 255;
            if (pointerValue == 1 && millis() < waveTimeTemp - itemMenuBlinkTime / 2) tempBright = 0;
            setPixelHSV(n, 150, 255, tempBright);
        }
        else tempBright = 0;
    }
    cli();
    update();
    sei();
}

void LunarisDisplay::update() {
    float t = micros() + 200;
    while(micros() < t){
        digitalWriteFast(pin, 0);
    }
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    while(micros() < t){
        digitalWriteFast(pin, 0);
    }
	for (int n = 0; n < pxCnt; n++) {
        if (bitRead(pixelData[n][0], 1)) n = n;


		for (int m = 7; m > -1; m--) {
			if (bitRead(pixelData[n][0], m)) sendOne();
			else sendZero();
		}
		for (int m = 7; m > -1; m--) {
			if (bitRead(pixelData[n][1], m)) sendOne();
			else sendZero();
		}
		for (int m = 7; m > -1; m--) {
			if (bitRead(pixelData[n][2], m)) sendOne();
			else sendZero();
		}
	}
}

void LunarisDisplay::sendOne() {
    digitalWriteFast(pin, 1);
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay

    digitalWriteFast(pin, 0);
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay

}

void LunarisDisplay::sendZero() {
    digitalWriteFast(pin, 1);
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay

    digitalWriteFast(pin, 0);
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
    __asm__ ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //100 ns of delay
}