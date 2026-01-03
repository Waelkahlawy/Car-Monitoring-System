
#ifndef UART_H
#define UART_H

#include "../../Cfg.h" // Project configuration header

#if UART_ENABLED == STD_ON
#include "esp_log.h" // ESP32 logging
#include "driver/uart.h" // UART driver header
#include <stdint.h>    // Standard integer types

// UART Configuration Structure
typedef struct {
    uart_port_t port;           // UART port number (UART_NUM_0, UART_NUM_1, UART_NUM_2)
    int tx_pin;                 // TX pin
    int rx_pin;                 // RX pin
    int baud_rate;              // Baud rate (e.g., 9600 for GPS)
} Uart_ConfigType;

// Function prototypes
void Uart_Init(Uart_ConfigType *configurations); // Initialize UART with given configurations
int Uart_Write(const char *data, size_t size);   // Write data to UART
int Uart_Read(uint8_t *data, size_t size, uint32_t timeout_ms); // Read data from UART with timeout
#else 
 ESP_LOGI(g_TAG, " UART feature is disabled "); 
return -1;

#endif // UART_ENABLED

#endif // UART_H