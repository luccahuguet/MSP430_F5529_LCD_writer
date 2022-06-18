#include <msp430.h>
#include <stdint.h>

#define CHARACTER 1
#define INSTRUCTION 0

#define LCD 0x27
#define BT BIT3
#define EN BIT2
#define RW BIT1
#define RS BIT0

void lcdPrint(uint8_t *str);
void lcdInit();
void lcdWriteByte(uint8_t byte, uint8_t isChar);
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
