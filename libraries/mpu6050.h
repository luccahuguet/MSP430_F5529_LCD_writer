#include <msp430.h>

//(SMCLK) 1.048.576/11  ~= 100kHz

#define BR100K 11

#define MPU_ADR 0x68 // mpu address
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define PWR_MGMT_1 0x6B

void config_mpu(void);
void read_array_mpu(char my_register, char *arr, char qtty);
void write_mpu(char my_register, char dado);
void config_i2c_mpu(void);
void delays(int x);
void get_mpu_accel(int *accel);
