#ifndef I2C_H
#define I2C_H

#include "config_file.h"

#if I2C_ENABLED == STD_ON

#include "driver/i2c.h"

// I2C Configuration Structure
typedef struct {
    i2c_port_t port;            // I2C port number (I2C_NUM_0 or I2C_NUM_1)
    gpio_num_t sda_pin;         // SDA pin
    gpio_num_t scl_pin;         // SCL pin
    uint32_t clk_speed;         // Clock speed in Hz
} I2c_ConfigType;

// Function prototypes
void I2c_Init(I2c_ConfigType *configurations);
int I2c_Write(uint8_t device_addr, uint8_t *data, size_t size);
int I2c_Read(uint8_t device_addr, uint8_t *data, size_t size);
int I2c_WriteRead(uint8_t device_addr, uint8_t *write_data, size_t write_size,
                  uint8_t *read_data, size_t read_size);

#endif // I2C_ENABLED

#endif // I2C_DRIVER_H