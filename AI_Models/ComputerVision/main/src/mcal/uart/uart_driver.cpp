#include "uart_driver.h"

void UARTDriver::init() {
  
    if (!Serial) {
        Serial.begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    }

}

bool UARTDriver::checkConnection() {
  
    while(Serial.available()) Serial.read();

 
    Serial.write(CMD_PING);

  
    unsigned long start = millis();
    while (millis() - start < 100) {
        if (Serial.available()) {
            uint8_t response = Serial.read();
            if (response == CMD_PONG) {
                return true;
            }
        }
    }
    return false; 
}

bool UARTDriver::sendResult(int labelIndex, float confidence) {
 
    Serial.print(">");
    Serial.print(labelIndex);
    Serial.print(",");
    Serial.println(confidence, 2);

    return true;
}