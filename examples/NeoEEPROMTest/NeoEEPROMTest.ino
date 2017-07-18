// neo pixel color test
// 15.3.2017 ChrisMicro

#include <EEPROM.h>
#include "neolib.h"

byte value;


void eeprom_write(unsigned short address, unsigned char data)
{
   while(EECR & (1<<EEPE));

   EECR = (0<<EEPM1) | (0<<EEPM0);

   if (address < 512)
   {
      EEAR = address;
   }else{
      EEAR = 511;
   }

   EEDR = data;

   EECR |= (1<<EEMPE);
   EECR |= (1<<EEPE);  
}

void setup() 
{
  neobegin();

  //eeprom_write(0, 0b00001111);
  uint8_t bin = EEPROM.read(0);

  for (uint8_t i = 0 ; i < 8; i++)
  {
    if ( bin & (1<<i) ) {
      pixels.setPixelColor(i,COLOR_RED);    
    }
  }

    pixels.show();                      // This sends the updated pixel color to the hardware.

}

void loop() 
{
  
  delay(5000);                        
                     
}

