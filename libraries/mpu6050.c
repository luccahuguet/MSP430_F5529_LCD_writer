// Funcoes para interface com o MPU-6050 (GY-521)

#include <msp430.h>
#include "mpu6050.h"

// A funcao abaixo coleta dados do acelerometro e grava em um vetor
void get_mpu_accel(int *accel)
{
    char array[16];
    read_array_mpu(ACCEL_XOUT_H, array, 14);

    accel[0] = array[0];
    accel[0] = (accel[0] << 8) + array[1];
    accel[1] = array[2];
    accel[1] = (accel[1] << 8) + array[3];
    accel[2] = array[4];
    accel[2] = (accel[2] << 8) + array[5];
}

// Configura o MPU
void config_mpu(void)
{
    UCB1I2CSA = MPU_ADR;
    write_mpu(PWR_MGMT_1, 1);   // Acodar e Rel�gio=PLL gx
    delays(100);                // Esperar acordar
    write_mpu(CONFIG, 6);       // Taxa = 1 kHz, Banda=5Hz
    write_mpu(SMPLRT_DIV, 9);   // Taxa de amostr. = 100 Hz
    write_mpu(GYRO_CONFIG, 0);  // +/- 250 graus/seg
    write_mpu(ACCEL_CONFIG, 0); // +/- 2g
}

// Faz a leitura de dados do MPU em sequencia
void read_array_mpu(char my_register, char *arr, char qtty)
{
    int i;

    // Requests the start of a transmission, as tx master
    UCB1CTL1 |= UCTR | UCTXSTT;

    while (!(UCB1IFG & UCTXIFG))
        ; // Awaits the line not to be busy

    // which register to read
    UCB1TXBUF = my_register;

    // Awaits the Ack/Nack cycle
    while (UCB1CTL1 & UCTXSTT)
        ;

    // nack case
    if (UCB1IFG & UCNACKIFG)
    {
        while (1)
            ;
    }

    // requests start of a transmission, as master rx

    UCB1CTL1 &= ~UCTR;
    UCB1CTL1 |= UCTXSTT;

    // Awaits the Ack/Nack cycle
    while (UCB1CTL1 & UCTXSTT)
        ;

    // reads some bites except the last
    for (i = 0; i < qtty - 1; i++)
    {
        while (!(UCB1IFG & UCRXIFG))
            ; // Awaits the line not to be busy

        arr[i] = UCB1RXBUF;
    }

    // generates stop
    UCB1CTL1 |= UCTXSTP;

    while (!(UCB1IFG & UCRXIFG))
        ; // Awaits the line not to be busy

    arr[i] = UCB1RXBUF;

    // Awaits the stop
    while (UCB1CTL1 & UCTXSTP)
        ;
}

// writes to a mpu register
void write_mpu(char my_register, char dado)
{
    // Requests the start of a transmission, as tx master
    UCB1CTL1 |= UCTR | UCTXSTT;

    while (!(UCB1IFG & UCTXIFG))
        ; // Awaits the line not to be busy

    // picks a register to write
    UCB1TXBUF = my_register;

    // Awaits the Ack/Nack cycle
    while (UCB1CTL1 & UCTXSTT)
        ;

    // nack case
    if (UCB1IFG & UCNACKIFG)
    {
        while (1)
            ;
    }

    while (!(UCB1IFG & UCTXIFG))
        ; // Awaits the line not to be busy

    UCB1TXBUF = dado;

    while (!(UCB1IFG & UCTXIFG))
        ; // Awaits the line not to be busy

    // generates stop
    UCB1CTL1 |= UCTXSTP;

    // Awaits the stop
    while (UCB1CTL1 & UCTXSTP)
        ;
}

// configures UCB1 as I2C master
void config_i2c_mpu()
{
    // Resets the interface
    UCB1CTL1 = UCSWRST;
    // Sets the mode to I2C
    UCB1CTL0 = UCMST | UCMODE_3 | UCSYNC;
    // Sets the clock to 1MHz (SMCLK)
    UCB1CTL1 |= UCSSEL__SMCLK;
    // 100KHz;
    UCB1BRW = BR100K;

    // Uses P4.1 and P4.2 as SDA and SCL
    // OBS: physical pull-up resistors were used together with the internal pull-up resistors
    P4SEL |= BIT1 | BIT2;

    UCB1CTL1 &= ~UCSWRST;
}

// generates delays
void delays(int x)
{
    volatile int j;
    for (j = 0; j < x; j++)
        ;
}
