#ifndef I2C_H
#define I2C_H

#include "../../Cfg.h"

#if I2C_ENABLED == STD_ON
// Include necessary headers
#include "esp_log.h" // ESP32 logging
#include "driver/i2c.h"

// I2C Configuration Structure
typedef struct {
    i2c_port_t port;            // I2C port number (I2C_NUM_0 or I2C_NUM_1)
    gpio_num_t sda_pin;         // SDA pin
    gpio_num_t scl_pin;         // SCL pin
    uint32_t clk_speed;         // Clock speed in Hz
} I2c_ConfigType;

// Function prototypes
void I2c_Init(I2c_ConfigType *configurations); // Initialize I2C with given configurations
int I2c_Write(uint8_t device_addr, uint8_t *data, size_t size); // Write data to I2C device
int I2c_Read(uint8_t device_addr, uint8_t *data, size_t size);  // Read data from I2C device
int I2c_WriteRead(uint8_t device_addr, uint8_t *write_data, size_t write_size, uint8_t *read_data, size_t read_size);   // Write then read data from I2C device


#endif // I2C_ENABLED

#endif // I2C_H