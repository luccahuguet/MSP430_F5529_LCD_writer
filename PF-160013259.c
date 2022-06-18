#include <msp430.h>
#include <stdint.h>
#include "libraries/lcd.h"
#include "libraries/i2c.h"
#include "libraries/mpu6050.h"

void config_adc_timer();
void start_adc_timer();
void config_ADC();
void config_Red_LED();
void config_Green_LED();

#define iters 40

#define RED_LED_ON (P1OUT |= BIT0)
#define RED_LED_OFF (P1OUT &= ~BIT0)
#define RED_LED_TOGGLE (P1OUT ^= BIT0)

#define GREEN_LED_ON (P4OUT |= BIT7)
#define GREEN_LED_OFF (P4OUT &= ~BIT7)
#define GREEN_LED_TOGGLE (P4OUT ^= BIT7)

// Comentarios sao evitados se a variavel for autoexplicativa

// var para armazenar os dados do acelerometro
int16_t accel[3];

// variavel para chave sw do joystick
unsigned long sw; // 6.5

// acumulador para chave sw
unsigned long sw_acc;

// program state
unsigned int state = 1;

unsigned long temp;
int adc12_conversion_ready;

char upper_default[] = "hello world";
char bottom_default[] = "\nstr de 15 chars";

// opcoes para o seletor de caracteres
char alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char alphabet_lower[26] = "abcdefghijklmnopqrstuvwxyz";
char numbers[10] = "0123456789";

// string com char a ser escrito
char mystr[1];

// indices para o seletor de caracteres
int alphabet_index = 0;
int alphabet_lower_index = 0;
int numbers_index = 0;

// posicao do cursor
int cursor_x = 15; // ranges from 0 to 15
int cursor_y = 1;  // ranges from 0 to 1

// proxima posicao do cursor (com base no acelerometro)
int x = 0; // -1 vai para a esquerda, 1 para a direita, 0 nao move
int y = 0; // -1 vai para baixo, 1 para cima, 0 nao move

// qual selecao de caracteres esta sendo usada (0 = alphabet, 1 = alphabet_lower, 2 = numbers)
int char_collection_index = 0;

// counters
unsigned int i, j;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    config_i2c_mpu(); // Configurar USCI_B1
    config_mpu();

    //   Configuracoes adc
    config_adc_timer();
    config_ADC();
    adc12_conversion_ready = 0;

    // Configuracoes relacionadas ao LCD
    config_i2c();
    lcdInit();

    // Configuracoes relacionadas ao LED
    config_Red_LED();
    config_Green_LED();

    __enable_interrupt();

    start_adc_timer();

    lcdPrint(upper_default);
    lcdPrint(bottom_default);

    while (1)
    {

        if (adc12_conversion_ready)
        {
            get_mpu_accel(accel);

            // Se detecta sw pressionado, muda estado (obs: delay cycles faz o debouncing)
            if (!sw)
            {
                // Led vermelho aceso, verde desligado, state = 0, modo de movimentacao do cursor
                // Led verde aceso, vermelho desligado, state = 1, modo de selecao de caracter

                __delay_cycles(1000);
                RED_LED_TOGGLE;
                GREEN_LED_TOGGLE;
                state = state % 2 + 1;
            }

            // calculo da proxima posicao do cursor
            if (6000 < accel[0]) // move para esquerda
            {
                x = -1;
            }
            else if (accel[0] < -2000) // move para direita
            {
                x = 1;
            }
            else
            {
                x = 0; // nao move
            }

            if (accel[1] > 4000) // move para baixo
            {
                y = -1;
            }
            else if (-4000 > accel[1]) // move para cima
            {
                y = 1;
            }
            else
            {
                y = 0; // nao move
            }

            if (state == 1)
            {

                if ((x == -1) && (cursor_x - 1 >= 0)) // first third
                {
                    lcdWriteByte(0b00010000, INSTRUCTION);
                    cursor_x = cursor_x + x;
                }
                else if ((x == 1) && (cursor_x + 1 <= 15)) // last third
                {

                    // move cursor to the right
                    lcdWriteByte(0b00010100, INSTRUCTION);
                    cursor_x = cursor_x + x;
                }
                else // middle third
                {
                }

                if (((y == -1) && cursor_y) || ((y == 1) && !cursor_y)) // first third
                {
                    lcdPrint("\n");
                    cursor_y = (cursor_y + 1) % 2;
                    cursor_x = 0;
                }
            }
            else // state = 0
            {

                if (y == 1) // select the char collection above
                {
                    char_collection_index = (char_collection_index + 1) % 3;
                }
                else if (y == -1) // select the char collection below
                {
                    if (char_collection_index == 0)
                    {
                        char_collection_index = 2;
                    }
                    else
                    {
                        char_collection_index = (char_collection_index - 1) % 3;
                    }
                }
                else // middle third: move on the x axis
                {

                    if (char_collection_index == 0) // alphabet
                    {
                        if ((alphabet_index + x) < 0)
                        {
                            alphabet_index = 25;
                        }
                        else
                        {
                            alphabet_index = (alphabet_index + x) % 26;
                        }
                        mystr[0] = alphabet[alphabet_index];
                    }
                    else if (char_collection_index == 1) // alphabet_lower
                    {
                        if ((alphabet_lower_index + x) < 0)
                        {
                            alphabet_lower_index = 25;
                        }
                        else
                        {
                            alphabet_lower_index = (alphabet_lower_index + x) % 26;
                        }
                        mystr[0] = alphabet_lower[alphabet_lower_index];
                    }
                    else // numbers
                    {
                        if ((numbers_index + x) < 0)
                        {
                            numbers_index = 9;
                        }
                        else
                        {
                            numbers_index = (numbers_index + x) % 10;
                        }
                        mystr[0] = numbers[numbers_index];
                    }
                }
                lcdPrint(mystr);

                // move cursor to the left (because it just printed a one-char string)
                lcdWriteByte(0b00010000, INSTRUCTION);
            }

            adc12_conversion_ready = 0;
        }
    }
    return 0;
}

void config_Red_LED()
{
    P1SEL &= ~BIT0;
    P1DIR |= BIT0;
    RED_LED_ON;
}

void config_Green_LED()
{
    P4SEL &= ~BIT7;
    P4DIR |= BIT7;
    GREEN_LED_OFF;
}

#pragma vector = ADC12_VECTOR
__interrupt void
ADC12_interrupt(void)
{
    switch (_even_in_range(ADC12IV, 0x24))
    {
    case ADC12IV_NONE:
        break;
    case ADC12IV_ADC12OVIFG: // MEMx overflow
        break;
    case ADC12IV_ADC12TOVIFG: // Conversion Time overflow
        break;
    case ADC12IV_ADC12IFG0: // MEM0 Ready
        break;
    case ADC12IV_ADC12IFG1: // MEM1 Ready
        break;
    case ADC12IV_ADC12IFG2: // MEM2 Ready
        break;
    case ADC12IV_ADC12IFG3: // MEM3 Ready
        break;
    case ADC12IV_ADC12IFG4: // MEM4 Ready
        break;
    case ADC12IV_ADC12IFG5: // MEM5 Ready

        temp = ADC12MEM5;

        // calculamos o nr de vezes que sw_cc viu valores maiores que zero
        if (temp > 0)
        {
            sw_acc += 1;
        }

        j++;

        if (j == (iters + 1))
        {
            sw = 1;

            // Se o nr de vezes que sw_acc foi maior que zero, for maior que 2, dizemos que o botao foi pressionado
            // esse valor iters/4 pode ser considerado a sensibilidade em relacao a sw (iters/2 seria muito sensivel, iters/8 seria muito pouco sensivel)
            if (sw_acc < iters / 4)
            {
                sw = 0;
            }
            sw_acc = 0;

            adc12_conversion_ready = 1;

            // No ultimo canal, o contador j vai para 0
            j = 0;
        }
        break;
    case ADC12IV_ADC12IFG6: // MEM6 Ready
        break;
    case ADC12IV_ADC12IFG7: // MEM7 Ready
        break;
    case ADC12IV_ADC12IFG8: // MEM8 Ready
        break;
    case ADC12IV_ADC12IFG9: // MEM9 Ready
        break;
    case ADC12IV_ADC12IFG10: // MEM10 Ready
        break;
    case ADC12IV_ADC12IFG11: // MEM11 Ready
        break;
    case ADC12IV_ADC12IFG12: // MEM12 Ready
        break;
    case ADC12IV_ADC12IFG13: // MEM13 Ready
        break;
    case ADC12IV_ADC12IFG14: // MEM14 Ready
        break;
    case ADC12IV_ADC12IFG15: // MEM15 Ready
        break;
    }
}

void config_ADC()
{
    // Configuro o P6.5 (A5) do ADC.
    P6SEL |= BIT5;

    // Desliga o modulo
    ADC12CTL0 &= ~ADC12ENC;

    ADC12CTL0 = ADC12SHT0_7 |
                ADC12MSC | // Um trigger dispara todas as conversoes.
                ADC12ON;   // Liga o ADC

    ADC12CTL1 = ADC12CSTARTADD_5 | // Start address: 5
                ADC12SHS_1 |       // Conversao via TimerA0.1
                ADC12SHP |         // Sample and Hold Pulse mode: input
                ADC12DIV_0 |       // Divide o clock por 1
                ADC12SSEL_1 |      // Escolhe o clock aCLK (que é mais devagar)
                ADC12CONSEQ_2;     // Modo: single channel / REPEAT conversion

    ADC12CTL2 = ADC12TCOFF | // Desliga o sensor de temperatura
                ADC12RES_2;  // 12 bits resolution

    // Configuracoes dos canais

    ADC12MCTL5 = ADC12SREF_0 | ADC12INCH_5; // Amostra o pino A5

    ADC12IE = ADC12IE5; // Liga a interrupcao do canal 5.

    // Liga o ADC.
    ADC12CTL0 |= ADC12ENC;
}

void config_adc_timer()
{
    TA0CTL = TASSEL__ACLK | // Usa o ACLK: 32768
             MC__STOP;      // Timer parado

    TA0CCTL1 = OUTMOD_2;

    TA0CCR0 = 32768;
    TA0CCR1 = 16384;
}

void start_adc_timer()
{
    TA0CTL |= (MC__UP | TACLR); // Start timer.
}
