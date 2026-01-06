#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <Arduino.h>


#define UART_BAUD_RATE  115200
#define UART_TX_PIN     1   // U0TXD (Standard ESP32-CAM)
#define UART_RX_PIN     3   // U0RXD (Standard ESP32-CAM)


#define CMD_PING        0xAA
#define CMD_PONG        0x55
#define START_BYTE      0x7E 

class UARTDriver {
public:
    
    static void init();

   
    static bool checkConnection();

    
    static bool sendResult(int labelIndex, float confidence);
};

#endif