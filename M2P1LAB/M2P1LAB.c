// Description: when the button 1.1 (left) is pressed the red led lights up

#include <msp430.h>

/**
 * main.c
 */

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Initializes input pins

    P1DIR &= ~BIT1; // P1.1 is input -> left button
    P1REN |= BIT1;  // Resistor pull up / pull down is enabled
    P1OUT |= BIT1;  // Resistor is a pull up resistor

    // Initializes output pins.

    P1DIR |= BIT0;  // P1.0 is output -> red led
    P1OUT &= ~BIT0; // initialized on off

    while(1)
    {
        if (P1IN & BIT1)
        {
            P1OUT &= ~BIT0; // Turns off the led if the button is not currently pressed;
        }
        else
        {
            P1OUT |= BIT0; // Turns on the led if the button is currently pressed;
        }
    }

    return 0;
}
