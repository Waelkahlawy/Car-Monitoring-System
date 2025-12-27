#include "i2c.h" // Include I2C header
// Check if I2C feature is enabled
#if I2C_ENABLED == STD_ON

#include "esp_log.h" // ESP32 logging

// Debug TAG
static const char *g_TAG = TAG_I2C;

// Global I2C port
static i2c_port_t g_I2c_Port;
// Initialize I2C with given configurations
void I2c_Init(I2c_ConfigType *configurations)
{

    // Store I2C port
    g_I2c_Port = configurations->port;

    // I2C master configuration
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, // Master mode
        .sda_io_num = configurations->sda_pin, // SDA pin
        .scl_io_num = configurations->scl_pin,  // SCL pin
        .sda_pullup_en = GPIO_PULLUP_ENABLE,     // Enable internal pull-up for SDA
        .scl_pullup_en = GPIO_PULLUP_ENABLE,        // Enable internal pull-up for SCL
        .master.clk_speed = configurations->clk_speed   // Clock speed in Hz
    };

    // Configure I2C parameters
    i2c_param_config(g_I2c_Port, &conf);

    // Install I2C driver
    i2c_driver_install(g_I2c_Port, conf.mode, 0, 0, 0);

#if I2C_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, " I2C Initialized");
    ESP_LOGI(g_TAG, "Port: %d, SDA: %d, SCL: %d, Speed: %lu Hz",
             g_I2c_Port, configurations->sda_pin, 
             configurations->scl_pin, configurations->clk_speed);
#endif

}
// Write data to I2C device
int I2c_Write(uint8_t device_addr, uint8_t *data, size_t size)
{

    // Create I2C command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Start bit
    i2c_master_start(cmd);
    
    // Write device address + write bit
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    
    // Write data
    i2c_master_write(cmd, data, size, true);
    
    // Stop bit
    i2c_master_stop(cmd);
    
    // Execute command
    esp_err_t ret = i2c_master_cmd_begin(g_I2c_Port, cmd, pdMS_TO_TICKS(1000));
    
    // Delete command link
    i2c_cmd_link_delete(cmd);

#if I2C_DEBUG_ENABLED == STD_ON
    if (ret == ESP_OK) {
        ESP_LOGI(g_TAG, "Write to 0x%02X: %d bytes", device_addr, size);
    } else {
        ESP_LOGE(g_TAG, "Write to 0x%02X failed: %s", device_addr, esp_err_to_name(ret));
    }
#endif

    return (ret == ESP_OK) ? 0 : -1;

}
// Read data from I2C device
int I2c_Read(uint8_t device_addr, uint8_t *data, size_t size)
{

    if (size == 0) {
        return -1;
    }

    // Create I2C command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Start bit
    i2c_master_start(cmd);
    
    // Write device address + read bit
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    
    // Read data
    if (size > 1) {
        i2c_master_read(cmd, data, size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + size - 1, I2C_MASTER_NACK);
    
    // Stop bit
    i2c_master_stop(cmd);
    
    // Execute command
    esp_err_t ret = i2c_master_cmd_begin(g_I2c_Port, cmd, pdMS_TO_TICKS(1000));
    
    // Delete command link
    i2c_cmd_link_delete(cmd);

#if I2C_DEBUG_ENABLED == STD_ON
    if (ret == ESP_OK) {
        ESP_LOGI(g_TAG, "Read from 0x%02X: %d bytes", device_addr, size);
    } else {
        ESP_LOGE(g_TAG, "Read from 0x%02X failed: %s", device_addr, esp_err_to_name(ret));
    }
#endif

    return (ret == ESP_OK) ? 0 : -1;

}
// Write then read data from I2C device
int I2c_WriteRead(uint8_t device_addr, uint8_t *write_data, size_t write_size,
                  uint8_t *read_data, size_t read_size)
{

    // Create I2C command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Start bit
    i2c_master_start(cmd);
    
    // Write device address + write bit
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    
    // Write data (usually register address)
    i2c_master_write(cmd, write_data, write_size, true);
    
    // Repeated start
    i2c_master_start(cmd);
    
    // Write device address + read bit
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    
    // Read data
    if (read_size > 1) {
        i2c_master_read(cmd, read_data, read_size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, read_data + read_size - 1, I2C_MASTER_NACK);
    
    // Stop bit
    i2c_master_stop(cmd);
    
    // Execute command
    esp_err_t ret = i2c_master_cmd_begin(g_I2c_Port, cmd, pdMS_TO_TICKS(1000));
    
    // Delete command link
    i2c_cmd_link_delete(cmd);

#if I2C_DEBUG_ENABLED == STD_ON
    if (ret == ESP_OK) {
        ESP_LOGI(g_TAG, "WriteRead 0x%02X: W=%d, R=%d bytes", device_addr, write_size, read_size);
    } else {
        ESP_LOGE(g_TAG, "WriteRead 0x%02X failed: %s", device_addr, esp_err_to_name(ret));
    }
#endif

    return (ret == ESP_OK) ? 0 : -1;

}
#else 
 ESP_LOGI(g_TAG, " I2C feature is disabled "); 
return -1;

#endif // I2C_ENABLED