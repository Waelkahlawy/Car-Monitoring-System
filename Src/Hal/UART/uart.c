#include "uart.h" // Include UART driver header

#if UART_ENABLED == STD_ON // Check if UART feature is enabled

#include "esp_log.h" // ESP32 logging

// Debug TAG
static const char *g_TAG = TAG_UART;
// Global UART port
static uart_port_t g_Uart_Port;

void Uart_Init(Uart_ConfigType *configurations)
{

    // Store UART port
    g_Uart_Port = configurations->port;

    // UART configuration (8N1 - standard for GPS)
    uart_config_t uart_config = {
        .baud_rate = configurations->baud_rate,// Set baud rate from configuration
        .data_bits = UART_FRAME_LENGTH,   // 8 data bits
        .parity = UART_PARITY_CHECK,  // No parity
        .stop_bits = UART_STOP_BIT,  // 1 stop bit
        .flow_ctrl = UART_FLOW_CTRL,  // No flow control
        .source_clk = UART_SOURCE_CLK,    // Default source clock
    };

    // Configure UART parameters
    uart_param_config(g_Uart_Port, &uart_config);

    // Set UART pins
    uart_set_pin(g_Uart_Port, 
                 configurations->tx_pin, 
                 configurations->rx_pin,
                 UART_PIN_NO_CHANGE,    // RTS
                 UART_PIN_NO_CHANGE);   // CTS

    // Install UART driver (RX buffer 2048, TX buffer 0 for GPS)
    uart_driver_install(g_Uart_Port, 2048, 0, 0, NULL, 0);
// Debug information
#if UART_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "UART Initialized for GPS");
    ESP_LOGI(g_TAG, "Port: %d, TX: %d, RX: %d, Baud: %d",
             g_Uart_Port, configurations->tx_pin,
             configurations->rx_pin, configurations->baud_rate);
#endif

}
// Write data to UART
int Uart_Write(const char *data, size_t size)
{

    // Write data to UART
    int bytes_written = uart_write_bytes(g_Uart_Port, data, size);
//  Debug information
#if UART_DEBUG_ENABLED == STD_ON
    if (bytes_written > 0) {
        ESP_LOGI(g_TAG, "Wrote %d bytes", bytes_written);
    } else {
        ESP_LOGE(g_TAG, "Write failed");
    }
#endif

    return bytes_written;

}
// Read data from UART with timeout
int Uart_Read(uint8_t *data, size_t size, uint32_t timeout_ms)
{

    // Read data from UART with timeout
    int bytes_read = uart_read_bytes(g_Uart_Port, data, size, pdMS_TO_TICKS(timeout_ms));

#if UART_DEBUG_ENABLED == STD_ON
    if (bytes_read > 0) {
        ESP_LOGI(g_TAG, "Read %d bytes", bytes_read);
    }
#endif

    return bytes_read; // Return number of bytes read

}

#else 
 ESP_LOGI(g_TAG, " UART feature is disabled "); 
return -1;

#endif // UART_ENABLED