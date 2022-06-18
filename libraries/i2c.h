#include <msp430.h>
#include <stdint.h>

void config_i2c();
uint8_t write_i2c(uint8_t addr, uint8_t *data, uint8_t nBytes);
uint8_t write_byte_i2c(uint8_t addr, uint8_t byte);
