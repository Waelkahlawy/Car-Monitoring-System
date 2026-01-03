#include "dht11.h"

#if DHT11_ENABLED == STD_ON

#include "../../Hal/GPIO/gpio.h"

#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Debug TAG
static const char *TAG = TAG_DHT11;

static Gpio_ConfigType g_Dht11_PinConfig;
// Microsecond delay function
static void Dht11_DelayUs(uint32_t us)
{
    esp_rom_delay_us(us);
}

void Dht11_Init(void)
{
    // Configure GPIO as output initially
    g_Dht11_PinConfig.pin_num = DHT11_GPIO_PIN;
    g_Dht11_PinConfig.pin_mode = GPIO_MODE_OUTPUT;
    g_Dht11_PinConfig.pin_value = HIGH;           // Initial high
    Gpio_InitPin(&g_Dht11_PinConfig);

#if DHT11_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "✅ DHT11 initialized on GPIO%d", DHT11_GPIO_PIN);
#endif
}

int Dht11_Main(Dht11_DataType *data)
{

    uint8_t raw_data[5] = {0};
    int retry = DHT11_MAX_RETRIES;

    while (retry--) {
        // Send start signal to DHT11
            g_Dht11_PinConfig.pin_mode = GPIO_MODE_OUTPUT;
        Gpio_InitPin(&g_Dht11_PinConfig);           // Set as output

        g_Dht11_PinConfig.pin_value = LOW;
        Gpio_WritePinValue(&g_Dht11_PinConfig);     // Low
        vTaskDelay(pdMS_TO_TICKS(20));              

        g_Dht11_PinConfig.pin_value = HIGH;
        Gpio_WritePinValue(&g_Dht11_PinConfig);     // High
        Dht11_DelayUs(50);                         // 30-50µs high

        g_Dht11_PinConfig.pin_mode = GPIO_MODE_INPUT;
        Gpio_InitPin(&g_Dht11_PinConfig);           // Switch to input

        
        // Wait for DHT11 response
        int timeout = 0;
        while (Gpio_ReadPinValue(DHT11_GPIO_PIN)  == 1 && timeout++ < 100) {
            Dht11_DelayUs(1);
        }
        
        timeout = 0;
        while (Gpio_ReadPinValue(DHT11_GPIO_PIN)  == 0 && timeout++ < 100) {
            Dht11_DelayUs(1);
        }
        
        timeout = 0;
        while (Gpio_ReadPinValue(DHT11_GPIO_PIN)  == 1 && timeout++ < 100) {
            Dht11_DelayUs(1);
        }

        // Read 40 bits of data
        for (int i = 0; i < 40; i++) {
            timeout = 0;
            while (Gpio_ReadPinValue(DHT11_GPIO_PIN)  == 0 && timeout++ < 200) {
                Dht11_DelayUs(1);
            }
            
            uint64_t start = esp_timer_get_time();
            
            timeout = 0;
            while (Gpio_ReadPinValue(DHT11_GPIO_PIN)  == 1 && timeout++ < 200) {
                Dht11_DelayUs(1);
            }
            
            uint32_t duration = esp_timer_get_time() - start;

            // If pulse > 40µs, bit = 1, else bit = 0
            raw_data[i / 8] <<= 1;
            if (duration > 40) {
                raw_data[i / 8] |= 1;
            }
        }

        // Verify checksum
        uint8_t checksum = (raw_data[0] + raw_data[1] + raw_data[2] + raw_data[3]) & 0xFF;
        
        if (raw_data[4] == checksum) {
            // Success - store data
            data->humidity = raw_data[0];
            data->temperature = raw_data[2];

#if DHT11_DEBUG_ENABLED == STD_ON
            ESP_LOGI(TAG, "Temperature: %d°C, Humidity: %d%%", 
                     data->temperature, data->humidity);
#endif
            return 0;  // Success
        }

#if DHT11_DEBUG_ENABLED == STD_ON
        ESP_LOGW(TAG, "Checksum error - Retry %d", DHT11_MAX_RETRIES - retry);
#endif

        // Wait before retry
        vTaskDelay(pdMS_TO_TICKS(100));
    }

#if DHT11_DEBUG_ENABLED == STD_ON
    ESP_LOGE(TAG, " DHT11 Read Failed - Checksum error after %d retries", DHT11_MAX_RETRIES);
#endif

    return -1;  // Failed

}
    


#else


// Stub functions when DHT11 is disabled
void Dht11_Init(void)
{
    // Do nothing
}

int Dht11_Main(Dht11_DataType *data)
{
    return -1;
}

#endif // DHT11_ENABLED













