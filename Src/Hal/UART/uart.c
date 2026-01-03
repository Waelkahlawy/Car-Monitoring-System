#include "uart.h"

#if UART_ENABLED == STD_ON

#include "esp_log.h"

static const char *g_TAG = TAG_UART;
static uart_port_t g_Uart_Port;

void Uart_Init(Uart_ConfigType *configurations)
{
    g_Uart_Port = configurations->port;

    uart_config_t uart_config = {
        .baud_rate = configurations->baud_rate,
        .data_bits = UART_FRAME_LENGTH,
        .parity    = UART_PARITY_CHECK,
        .stop_bits = UART_STOP_BIT,
        .flow_ctrl = UART_FLOW_CTRL,
        .source_clk = UART_SOURCE_CLK,
    };

    uart_param_config(g_Uart_Port, &uart_config);

    uart_set_pin(g_Uart_Port,
                 configurations->tx_pin,
                 configurations->rx_pin,
                 UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE);

    uart_driver_install(g_Uart_Port, 2048, 0, 0, NULL, 0);

#if UART_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "UART Initialized for GPS");
    ESP_LOGI(g_TAG, "Port: %d, TX: %d, RX: %d, Baud: %d",
             g_Uart_Port,
             configurations->tx_pin,
             configurations->rx_pin,
             configurations->baud_rate);
#endif
}

int Uart_Write(const char *data, size_t size)
{
    int bytes_written = uart_write_bytes(g_Uart_Port, data, size);

#if UART_DEBUG_ENABLED == STD_ON
    if (bytes_written > 0)
        ESP_LOGI(g_TAG, "Wrote %d bytes", bytes_written);
    else
        ESP_LOGE(g_TAG, "Write failed");
#endif

    return bytes_written;
}

int Uart_Read(uint8_t *data, size_t size, uint32_t timeout_ms)
{
    int bytes_read = uart_read_bytes(g_Uart_Port, data, size, pdMS_TO_TICKS(timeout_ms));

#if UART_DEBUG_ENABLED == STD_ON
    if (bytes_read > 0)
        ESP_LOGI(g_TAG, "Read %d bytes", bytes_read);
#endif

    return bytes_read;
}

#else
#warning "UART feature is disabled"
#endif
