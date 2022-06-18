#include <msp430.h>
#include <stdint.h>
#include "i2c.h"
#include "lcd.h"

void lcdWriteNibble(uint8_t nibble, uint8_t isCharacter)
{
  // Facamos o Enable (EN) pulsar 1 vez

  nibble <<= 4;
  //                         BT | EN | RW | RS
  write_byte_i2c(LCD, nibble | BT | 0 | 0 | isCharacter);
  write_byte_i2c(LCD, nibble | BT | EN | 0 | isCharacter);
  write_byte_i2c(LCD, nibble | BT | 0 | 0 | isCharacter);
}

void lcdWriteByte(uint8_t byte, uint8_t isCharacter)
{
  // Writes the most significant nibble
  lcdWriteNibble(byte >> 4, isCharacter);
  // Writes the least significant nibble
  lcdWriteNibble(byte & 0x0F, isCharacter);
}

uint8_t line = 0x00;
// O progama so consegue pular linha se o cursor estiver na upper line
// se nao, o cursor deve ser movido para com o comando apropriado

void lcdPrint(uint8_t *str)
{

  while (*str)
  {
    if (*str == '\n')
    {
      line ^= BIT6;
      lcdWriteByte(BIT7 | line, INSTRUCTION);
    }
    else
    {
      lcdWriteByte(*str, CHARACTER);
    }
    str++;
  }
}

void lcdInit()
{
  // tries to enter 8bit mode 3 times
  lcdWriteNibble(0x3, 0);
  lcdWriteNibble(0x3, 0);
  lcdWriteNibble(0x3, 0);

  // 0 0 0 0 0 1, S / C,  R / L, -, - Set cursor moving and display shift control bit, and the direction, without changing of DDRAM data.

  // Enters 4-bit mode
  lcdWriteNibble(0x2, 0);

  // Configures the LCD
  lcdWriteByte(0x06, 0);

  // Turns on the display and the cursor
  lcdWriteByte(0x0F, 0);

  // Cursor moves to the right and configures the shift
  lcdWriteByte(0x14, 0);

  // 4-bit mode with 2 lines
  lcdWriteByte(0x28, 0);

  //  Clears the display
  lcdWriteByte(0x01, 0);
}
