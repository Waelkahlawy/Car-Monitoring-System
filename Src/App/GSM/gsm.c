#include "gsm.h"

#if GSM_ENABLED == STD_ON // Check if GSM feature is enabled
#include <stdint.h>


static uint8_t g_gsm_rx_buffer[GSM_BUF_SIZE];

 static Gsm_Status_t GSM_SendAT(const char *cmd, const char *expected,uint32_t timeout_ms)
{
    memset(g_gsm_rx_buffer, 0, sizeof(g_gsm_rx_buffer));

    Uart_Write(cmd, strlen(cmd));
    Uart_Write("\r\n", 2);

    int len = Uart_Read(g_gsm_rx_buffer,
                        GSM_BUF_SIZE - 1,
                        timeout_ms);

    if (len > 0) {
        if (strstr((char *)g_gsm_rx_buffer, expected))
            return GSM_OK;
        return GSM_ERROR;
    }

    return GSM_TIMEOUT;
}

void GSM_Init(void)
{
    
    GSM_SendAT("AT", "OK", 1000);
    GSM_SendAT("ATE0", "OK", 1000);        // Echo off
    GSM_SendAT("AT+CMGF=1", "OK", 1000);   // SMS text mode

}

Gsm_Status_t GSM_SendSMS(const char *number, const char *text)
{
    char cmd[64];

    if (GSM_SendAT("AT+CREG?", "+CREG: 0,1", 5000) != GSM_OK)
        return GSM_NO_NETWORK;

    GSM_SendAT("AT+CMGF=1", "OK", 1000); // SMS MODE
    GSM_SendAT("AT+CSCS=\"GSM\"", "OK", 1000); // CHARSET

    sprintf(cmd, "AT+CMGS=\"%s\"", number);
    Uart_Write(cmd, strlen(cmd));
    Uart_Write("\r\n", 2);

    vTaskDelay(pdMS_TO_TICKS(1000)); // important delay

    Uart_Write(text, strlen(text));
    Uart_Write("\r\n", 2);           // some modems need this
    vTaskDelay(pdMS_TO_TICKS(500));  // let modem process

    Uart_Write("\x1A", 1);           // CTRL+Z

    int len = Uart_Read(g_gsm_rx_buffer, GSM_BUF_SIZE - 1, 20000); // <-- 20 sec

    if (len > 0 && strstr((char *)g_gsm_rx_buffer, "+CMGS"))
        return GSM_OK;

    return GSM_ERROR;
}


Gsm_Status_t GSM_MakeCall(const char *number)
{
    char cmd[32];
    sprintf(cmd, "ATD%s;", number);
    return GSM_SendAT(cmd, "OK", 2000);
}

Gsm_Status_t GSM_GetSignal(uint8_t *rssi)
{
    if (GSM_SendAT("AT+CSQ", "+CSQ", 1000) != GSM_OK)
        return GSM_ERROR;

    /* +CSQ: <rssi>,<ber> */
    sscanf((char *)g_gsm_rx_buffer, "+CSQ: %hhu", rssi); 
    return GSM_OK;
}


#endif