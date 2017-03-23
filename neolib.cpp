/* 
=========================================================================================================
 _____  ______ _ _      ___  ____      _                       _   _  _____ _____ 
|  _  | | ___ (_) |     |  \/  (_)    | |                     | \ | ||  ___|  _  |
 \ V /  | |_/ /_| |_    | .  . |___  _| |_ __ _ _ __   ___    |  \| || |__ | | | |
 / _ \  | ___ \ | __|   | |\/| | \ \/ / __/ _` | '_ \ / _ \   | . ` ||  __|| | | |
| |_| | | |_/ / | |_    | |  | | |>  <| || (_| | |_) |  __/   | |\  || |___\ \_/ /
\_____/ \____/|_|\__|   \_|  |_/_/_/\_\\__\__,_| .__/ \___|   \_| \_/\____/ \___/ 
                                             | |                              
     https://8bitmixtape.github.io/          |_|                    
    .------------------------------.                              ATTINY85 Pins
    |↓↓\ 0  0  0  0  0  0  0  0 /↑↑|                              =============
    |   \___8Bit Mixtape NEO___/   |                                 _______
    |        __  ______  __        |                                |   U   |                                     
    |       /  \|\.....|/  \       |       (SYNC-OUT) <- D5/A0  PB5-|       |- VCC                                
    |       \__/|/_____|\__/       |   SD -> *SD-prog -> D3/A3  PB3-| ATTINY|- PB2  D2/A1 <- POTI_RIGHT
    |   ________________________   |   POTI_LEFT / CV -> D4/A2  PB4-|   85  |- PB1  D1    -> PWM SOUND
    |  |v2.0 hardware by dusjagr|  |                            GND-|       |- PB0  D0    -> NEOPIXELS
    '=============================='                                |_______|
  * based on TinyAudioBoot and hex2wav by Chris Haberer, Fredrik Olofsson, Budi Prakosa
    https://github.com/ChrisMicro/AttinySound
    
=========================================================================================================
The Center for Alternative Coconut Research presents:
-----------------------------------------------------
  Empty sketch to edit
  Description:
*********************************************************************************************************
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
********************************************* list of outhors *******************************************
  v0.x  01.01.1999 -H-A-B-E-R-E-R-  various routines to read the 8Bit Mixtape NEO
  v0.1  13.03.2017 -D-U-S-J-A-G-R-  adapted to new schematics 0.95. different resistor values
  v0.2  14.03.2017 -D-U-S-J-A-G-R-  geekint with ascii .·´¯`·.´¯`·.¸¸.·´¯`·.¸><(((º>
  v0.3  14.03.2017 -H-A-B-E-R-E-R-  made a library out of it
  v0.                               coming soon...
  
  
  It is mandatory to keep the list of authors in this code.
  Please add your name if you improve/extend something
     _          _             
   _| |_ _ ___ |_|___ ___ ___ 
  | . | | |_ -|| | .'| . |  _|
  |___|___|___|| |__,|_  |_|  
             |___|   |___|    
****************************************************************************************************** */
#include "neolib.h"


// fast pin access
#define AUDIOPIN (1<<SPEAKERPIN)
#define PINLOW (PORTB&=~AUDIOPIN)
#define PINHIGH (PORTB|=AUDIOPIN)

// variables
// Initialize the NEO pixel library
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the 8Bit Mixtape NEO
 ___ _____ _ _      _____ _     _                  _____ _____ _____ 
| . | __  |_| |_   |     |_|_ _| |_ ___ ___ ___   |   | |   __|     |
| . | __ -| |  _|  | | | | |_'_|  _| .'| . | -_|  | | | |   __|  |  |
|___|_____|_|_|    |_|_|_|_|_,_|_| |__,|  _|___|  |_|___|_____|_____|
                                       |_|                           
========================================================================================================================
   _________    
  | 8Bit()  |   uint8_t   getButton()                       -> 1 left, 2 right  3 both 
  |  o___o  |   uint16_t  analogReadScaled(uint8_t channel) -> scale the adc of the voltage divider to a return value: 0..1023     
  |__/___\__|   
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

uint8_t getButton()
{
  uint8_t  button = BUTTON_NONE;
  uint16_t pinVoltage;
  
  pinVoltage = analogRead( BUTTONS_ADC );
  
  if ( pinVoltage < Vbutton_left  ) button = BUTTON_LEFT;
  if ( pinVoltage < Vbutton_right ) button = BUTTON_RIGHT;
  if ( pinVoltage < Vbutton_both  ) button = BUTTON_LEFT + BUTTON_RIGHT;

  return button;
}


uint16_t analogReadScaled(uint8_t channel)
{
  uint16_t value = analogRead(channel);
  value = value * Vcc / Vdiv;
  if (value > 1023) value = 1023;
  return value;
}

uint16_t getPoti(uint8_t channel)
{
  analogReadScaled(channel);
}

/*
 uint8_t wasButtonPressed()
 
 returns the button which was pressed when the button is released.
 
 return values:
 
 BUTTON_LEFT
 BUTTON_RIGHT
 BUTTON_LEFT+BUTTON_RIGHT
 
 One of the most underestimated tasks is debouncing a button. 
 This routine works in the most cases. It could be useful to 
 add some time delay after the button is released to prevent
 bouncing. 
 
 v0.1  16.03.2017 -H-A-B-E-R-E-R-  first version 
 
*/

#define BUTTON_NOTPRESSED   0
#define BUTTON_PRESSED      1

uint8_t wasButtonPressed()
{
  static uint8_t buttonPressed    = false;
  static uint8_t buttonState      = 0;
  static uint8_t buttonValue      = BUTTON_NONE;
  static uint8_t buttonMaxValue   = 0;

  uint8_t        buttonReturnValue;  
  uint16_t       pinVoltage;
    
  pinVoltage = analogRead( BUTTONS_ADC );
  
  // hysteresis switch
  if( pinVoltage > Vbutton_releaseLevel ) buttonPressed = false;
  if( pinVoltage < Vbutton_pressedLevel ) buttonPressed = true;

  buttonReturnValue = BUTTON_NONE;
  
  switch( buttonState )
  {
    case BUTTON_NOTPRESSED:
    {
      buttonMaxValue = 0;      
      
      if( buttonPressed )
      {      
        buttonState = BUTTON_PRESSED;
      }
    };break;
    
    case BUTTON_PRESSED:
    {
      if( buttonPressed ) // find minimum volage level during button pressed period
      {
        buttonValue = BUTTON_NONE; 
        
             if ( pinVoltage < Vbutton_both  ) buttonValue = BUTTON_LEFT + BUTTON_RIGHT;
        else if ( pinVoltage < Vbutton_right ) buttonValue =               BUTTON_RIGHT;
        else if ( pinVoltage < Vbutton_left  ) buttonValue = BUTTON_LEFT               ;      
        
        if( buttonValue > buttonMaxValue ) buttonMaxValue = buttonValue;   
                                     
      }else
      {
        buttonState = BUTTON_NOTPRESSED;
	buttonReturnValue = buttonMaxValue; 
      }
      ;break;

    }
  }
  
  return buttonReturnValue; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the NEO-Pixel Library
 _____ _____ _____        _         _ 
|   | |   __|     |   ___|_|_ _ ___| |
| | | |   __|  |  |  | . | |_'_| -_| |
|_|___|_____|_____|  |  _|_|_,_|___|_|
                     |_|              
========================================================================================================================
   _________    
  | NEO()   |   void setColorAllPixel(uint32_t color)                   -> Sets all the pixels to the same color
  |  o___o  |   void displayBinrayValue(uint16_t value, uint32_t color) -> displays binary number
  |__/___\__|   uint32_t Wheel(byte WheelPos)                           -> Input a value 0 to 255 to get a color value.   
                                                                        The colours are a transition r - g - b - back to r. 
                void rainbowCycle(uint8_t wait, uint8_t rounds)         -> makes a Rainbow :-)  
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void displayBinaryValue(uint16_t value, uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    if (value & (1 << n)) pixels.setPixelColor(n, color);
    //else pixels.setPixelColor(n,0); // off
  }
}

void setColorAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, color); // off
  }
}

void rainbowCycle(uint8_t wait, uint8_t rounds, uint8_t rainbowPixels) {
  uint16_t i, j;

  for (j = 0; j < 256 * rounds; j++) { 
    for (i = 0; i < rainbowPixels; i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / rainbowPixels) + j) & 255));
    }
    //if (brightosiech >= 255) brightosiech = 255;
    pixels.show();
    delay(wait);
  }
}
/*

RGB_Color=Wheel(colorNumber)

converts a colorNumber on a color wheel from 0 to 255 into a RGB color.

https://color.adobe.com/de/create/color-wheel/?base=2&rule=Analogous
*/
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// remape wheel to a more significant name
uint32_t colorWheel(uint8_t colorNumber) 
{
   return Wheel(colorNumber);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* the setup routine runs once when you start the tape or press reset
========================================================================================================================
   _________            _                   
  | setup() |   ___ ___| |_ _ _ _ _ 
  |  o___o  |  |_ -| -_|  _| | | . |
  |__/___\__|  |___|___|_| |___|  _|
                               |_|    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void neobegin() 
{
  
#if defined (__AVR_ATtiny85__)                               // Whaaadiiissiiiittt?
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pinMode(SPEAKERPIN, OUTPUT);
  
  //analogReference( INTERNAL2V56 );                        //still discussing it...
  
  pixels.begin();                                           // This initializes the NeoPixel library.
  pixels.setBrightness(40);                                 // Woooowww!! They are sooo bright!

}