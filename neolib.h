#ifndef _NEOLIB_H_
#define _NEOLIB_H_

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__                 // Whaaadiiissiiiittt? == its only vor AVR microcontrollers ( the ATTINY is one )
#include <avr/power.h>
#endif

#include "neocolors.h"

// hardware description / pin connections
#define NEOPIXELPIN         0
#define SPEAKERPIN          1
#define POTI_RIGHT         A2
#define POTI_LEFT          A1
#define BUTTONS_ADC        A3

#define NUMPIXELS           8

#define BUTTON_NONE         0
#define BUTTON_LEFT         1
#define BUTTON_RIGHT        2


// hardware calibration

#define Vbutton_releaseLevel  450
#define Vbutton_left          380
#define Vbutton_right         300
#define Vbutton_both          224
#define Vbutton_pressedLevel  Vbutton_left 

#define Vcc                    37 // 3.7 V for LiPo
#define Vdiv                   26 // measure max Voltage on Analog In

extern Adafruit_NeoPixel pixels;

void     neobegin();
uint8_t  getButton();
uint8_t  wasButtonPressed();

uint16_t analogReadScaled(uint8_t channel);
uint16_t getPoti(uint8_t channel);

void     displayBinaryValue(uint16_t value, uint32_t color);
void     setColorAllPixel(uint32_t color);
void     rainbowCycle(uint8_t wait, uint8_t rounds, uint8_t rainbowPixels);
uint32_t Wheel(byte WheelPos); // depreciated
uint32_t colorWheel(uint8_t colorNumber); 

#endif // _NEOLIB_H_ 