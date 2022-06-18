#include <msp430.h>
#include <stdint.h>
#include "i2c.h"

void config_i2c()
{
  // Resets the interface
  UCB0CTL1 = UCSWRST;
  // Sets the mode to I2C
  UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
  // Sets the clock to 1MHz (SMCLK)
  UCB0CTL1 |= UCSSEL__SMCLK;
  // SCL => SMCLK /100 = 10 MHz;
  UCB0BRW = 100;

  // Uses P3.0 and P3.1 as SDA and SCL
  // OBS: physical pull-up resistors were used together with the internal pull-up resistors
  P3SEL |= BIT0 | BIT1;

  // Sets internal pull-up resistors
  P3DIR &= ~(BIT0 | BIT1);
  P3REN |= BIT0 | BIT1;
  P3OUT |= BIT0 | BIT1;

  UCB0CTL1 &= ~UCSWRST;
}

uint8_t write_i2c(uint8_t addr, uint8_t *data, uint8_t nBytes)
{
  // Resets the interface
  UCB0IFG = 0;

  // Sets the slave address
  UCB0I2CSA = addr;

  // Requests the start of a transmission
  UCB0CTL1 |= UCTXSTT | UCTR;

  // Awaits the line not to be busy
  while (!(UCB0IFG & UCTXIFG))
    ;

  // Trasmission buffer
  UCB0TXBUF = *data++;
  nBytes--;

  // Awaits the Ack/Nack cycle
  while (UCB0CTL1 & UCTXSTT)
    ;

  // nack case
  if (UCB0IFG & UCNACKIFG)
  {
    // Sends a stop
    UCB0CTL1 |= UCTXSTP;
    while (UCB0CTL1 & UCTXSTP)
      ;
    // Returns the nack error
    return 1;
  }

  while (nBytes--)
  {
    // Awaits the line not to be busy
    while (!(UCB0IFG & UCTXIFG))
      ;
    // When possible, writes in the transmission buffer
    UCB0TXBUF = *data++;
  }

  // Awaits the last byte
  while (!(UCB0IFG & UCTXIFG))
    ;

  // Asks for the stop
  UCB0CTL1 |= UCTXSTP; // Pe�o o STOP
  while (UCB0CTL1 & UCTXSTP)
    ; // Espero o Stop ser de fato transmitido

  return 0;
}

uint8_t write_byte_i2c(uint8_t addr, uint8_t byte)
{
  return write_i2c(addr, &byte, 1);
}