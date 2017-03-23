// pusched button example
// 16.3.2017 ChrisMicro

#include "neolib.h"

void setup()
{
  neobegin();
}

uint8_t  LedPosition   = 0;
uint8_t  ColorPosition = 0;
uint32_t Colors[] = { COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_PINK};

void loop()
{
  pixels.setPixelColor( LedPosition, COLOR_BLACK );   // clear pixel
  
  uint8_t b = wasButtonPressed();
  
  if( b == BUTTON_LEFT                )   LedPosition--   ;
  if( b == BUTTON_RIGHT               )   LedPosition++   ;  
  if( b == BUTTON_RIGHT + BUTTON_LEFT )   ColorPosition++ ;

  LedPosition   &= 0b00000111; // prevent out of range
  ColorPosition &= 0b00000011; // prevent out of range
  
  pixels.setPixelColor( LedPosition, Colors[ColorPosition] );
  pixels.show();
}

 



