#include "dht11.h"

#if DHT11_ENABLED == STD_ON

#include "../../Hal/GPIO/gpio.h"
#include "esp_log.h"
#include "esp_rom_delay.h"   
#include "esp_timer.h"       

// Debug TAG
static const char *g_TAG = TAG_DHT11;

// Global GPIO config
static Gpio_ConfigType g_Dht11_PinConfig;

static void dht11_delay_us(uint32_t us)
{
    esp_rom_delay_us(us);
}


void Dht11_Init(void)
{
    g_Dht11_PinConfig.pin_num = DHT11_GPIO_PIN;
    g_Dht11_PinConfig.pin_mode = GPIO_MODE_OUTPUT;
    g_Dht11_PinConfig.pin_value = HIGH;           // Initial high
    g_Dht11_PinConfig.pin_pull = GPIO_PULLUP_ENABLE;

    Gpio_InitPin(&g_Dht11_PinConfig);

#if DHT11_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "DHT11 Sensor Initialized on GPIO %d", DHT11_GPIO_PIN);
#endif
}

esp_err_t Dht11_Read(uint8_t* humidity, uint8_t* temperature)
{
    if (humidity == NULL || temperature == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data[5] = {0};
    int retries = 2;  // 

    while (retries--) {
        
        g_Dht11_PinConfig.pin_mode = GPIO_MODE_OUTPUT;
        Gpio_InitPin(&g_Dht11_PinConfig);           // Set as output

        g_Dht11_PinConfig.pin_value = LOW;
        Gpio_WritePinValue(&g_Dht11_PinConfig);     // Low
        vTaskDelay(pdMS_TO_TICKS(20));              

        g_Dht11_PinConfig.pin_value = HIGH;
        Gpio_WritePinValue(&g_Dht11_PinConfig);     // High
        dht11_delay_us(40);                         // 30-50µs high

        g_Dht11_PinConfig.pin_mode = GPIO_MODE_INPUT;
        g_Dht11_PinConfig.pin_pull = GPIO_PULLUP_ONLY;  //
        Gpio_InitPin(&g_Dht11_PinConfig);           // Switch to input

        
        int timeout = 0;
        while (Gpio_ReadPinValue(DHT11_GPIO_PIN) == HIGH && timeout++ < 200) dht11_delay_us(1);
        if (timeout >= 200) goto retry;

        timeout = 0;
        while (Gpio_ReadPinValue(DHT11_GPIO_PIN) == LOW && timeout++ < 200) dht11_delay_us(1);
        if (timeout >= 200) goto retry;

        timeout = 0;
        while (Gpio_ReadPinValue(DHT11_GPIO_PIN) == HIGH && timeout++ < 200) dht11_delay_us(1);
        if (timeout >= 200) goto retry;

        
        for (int i = 0; i < 40; i++) {
            timeout = 0;
            while (Gpio_ReadPinValue(DHT11_GPIO_PIN) == LOW && timeout++ < 200) dht11_delay_us(1);
            if (timeout >= 200) goto retry;

            uint64_t t_start = esp_timer_get_time();

            timeout = 0;
            while (Gpio_ReadPinValue(DHT11_GPIO_PIN) == HIGH && timeout++ < 200) dht11_delay_us(1);
            if (timeout >= 200) goto retry;

            uint64_t duration = esp_timer_get_time() - t_start;

            data[i / 8] <<= 1;
            if (duration > 40) {  
                data[i / 8] |= 1;
            }
        }

        
        if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
            *humidity = data[0];
            *temperature = data[2];

#if DHT11_DEBUG_ENABLED == STD_ON
            ESP_LOGI(g_TAG, "DHT11 Success - Humidity: %d%%, Temperature: %d°C", *humidity, *temperature);
#endif
            // Return pin to safe state
            g_Dht11_PinConfig.pin_mode = GPIO_MODE_OUTPUT;
            g_Dht11_PinConfig.pin_value = HIGH;
            Gpio_InitPin(&g_Dht11_PinConfig);
            Gpio_WritePinValue(&g_Dht11_PinConfig);

            return ESP_OK;
        }

    retry:
        if (retries > 0) {
            vTaskDelay(pdMS_TO_TICKS(100));   
        }
    }

#if DHT11_DEBUG_ENABLED == STD_ON
    ESP_LOGE(g_TAG, "DHT11 Read Failed - Checksum error or timeout after retries");
#endif

    // Return pin to high if failed
    g_Dht11_PinConfig.pin_mode = GPIO_MODE_OUTPUT;
    g_Dht11_PinConfig.pin_value = HIGH;
    Gpio_InitPin(&g_Dht11_PinConfig);
    Gpio_WritePinValue(&g_Dht11_PinConfig);

    return ESP_FAIL;
}



#else

// Stub functions when disabled
void Dht11_Init(void)
{
    // Do nothing
}

esp_err_t Dht11_Read(uint8_t* humidity, uint8_t* temperature)
{
    if (humidity) *humidity = 0;
    if (temperature) *temperature = 0;
    return ESP_FAIL;
}

#endif // DHT11_ENABLED