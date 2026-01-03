
#include "gps.h"

#if GPS_ENABLED == STD_ON
#include "../../Hal/GPIO/gpio.h"
#include "../../Hal/UART/uart.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>


// Debug TAG
static const char *TAG = TAG_GPS;

// Global UART config for GPS
static Uart_ConfigType g_Gps_UartConfig;

// GPS buffer
static uint8_t g_Gps_Buffer[GPS_BUFFER_SIZE];

// Parse GPGGA sentence (Global Positioning System Fix Data)
static bool Gps_ParseGPGGA(const char *sentence, Gps_DataType *data)
{
    char buffer[GPS_BUFFER_SIZE];
    strncpy(buffer, sentence, GPS_BUFFER_SIZE - 1);
    buffer[GPS_BUFFER_SIZE - 1] = '\0';

    // Tokenize: $GPGGA,time,lat,N/S,lon,E/W,fix,sats,hdop,alt,M,...
    char *token = strtok(buffer, ",");
    int field = 0;
    
    float lat_deg = 0, lon_deg = 0;
    char lat_dir = 'N', lon_dir = 'E';
    int fix_quality = 0;

    while (token != NULL) {
        switch (field) {
            case 1:  // UTC time (hhmmss.sss)
                if (strlen(token) >= 6) {
                    data->hour = (token[0] - '0') * 10 + (token[1] - '0');
                    data->minute = (token[2] - '0') * 10 + (token[3] - '0');
                    data->second = (token[4] - '0') * 10 + (token[5] - '0');
                }
                break;
            case 2:  // Latitude (ddmm.mmmm)
                lat_deg = atof(token);
                break;
            case 3:  // N/S
                lat_dir = token[0];
                break;
            case 4:  // Longitude (dddmm.mmmm)
                lon_deg = atof(token);
                break;
            case 5:  // E/W
                lon_dir = token[0];
                break;
            case 6:  // Fix quality (0=invalid, 1=GPS, 2=DGPS)
                fix_quality = atoi(token);
                break;
            case 7:  // Number of satellites
                data->satellites = atoi(token);
                break;
            case 9:  // Altitude
                data->altitude = atof(token);
                break;
        }
        token = strtok(NULL, ",");
        field++;
    }

    // Convert NMEA format (ddmm.mmmm) to decimal degrees
    if (lat_deg > 0) {
        int lat_d = (int)(lat_deg / 100);
        float lat_m = lat_deg - (lat_d * 100);
        data->latitude = lat_d + (lat_m / 60.0);
        if (lat_dir == 'S') data->latitude = -data->latitude;
    }

    if (lon_deg > 0) {
        int lon_d = (int)(lon_deg / 100);
        float lon_m = lon_deg - (lon_d * 100);
        data->longitude = lon_d + (lon_m / 60.0);
        if (lon_dir == 'W') data->longitude = -data->longitude;
    }

    data->valid = (fix_quality > 0);
    return data->valid;
}

void Gps_Init(void)
{
    // Configure UART for GPS
    g_Gps_UartConfig.port = GPS_UART_PORT;
    g_Gps_UartConfig.tx_pin = GPS_TX_PIN;
    g_Gps_UartConfig.rx_pin = GPS_RX_PIN;
    g_Gps_UartConfig.baud_rate = GPS_BAUD_RATE;

    Uart_Init(&g_Gps_UartConfig);

#if GPS_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, " GPS initialized on UART%d (TX:%d, RX:%d, Baud:%d)",
             GPS_UART_PORT, GPS_TX_PIN, GPS_RX_PIN, GPS_BAUD_RATE);
#endif
}

void Gps_PowerOn(void)
{
     // Enable GPS power
    Uart_Write("AT+CGPSPWR=1\r\n", strlen("AT+CGPSPWR=1\r\n"));
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Configure NMEA output baud rate
    // This is optional, but recommended if the baud rate was changed before
    Uart_Write("AT+CGPSIPR=9600\r\n", strlen("AT+CGPSIPR=9600\r\n"));
    vTaskDelay(pdMS_TO_TICKS(500));

        // Perform a cold start reset
    // Cold start clears previous GPS data and forces a full satellite reacquisition
    Uart_Write("AT+CGPSRST=0\r\n", strlen("AT+CGPSRST=0\r\n"));  // 0 = Cold start
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Enable output of all NMEA sentences (GGA, RMC, GSA, GSV, etc.)
    Uart_Write("AT+CGPSOUT=255\r\n", strlen("AT+CGPSOUT=255\r\n"));  // 255 = كل الـ sentences
    vTaskDelay(pdMS_TO_TICKS(1000));

#if GPS_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "GPS powered ON with FULL NMEA stream (incl. GSA for PDOP)");
#endif
}


// Debug TAG
static const char *TAG = TAG_GPS;

// Global UART config for GPS
static Uart_ConfigType g_Gps_UartConfig;

// GPS buffer
static uint8_t g_Gps_Buffer[GPS_BUFFER_SIZE];

// Parse GPGGA sentence (Global Positioning System Fix Data)
static bool Gps_ParseGPGGA(const char *sentence, Gps_DataType *data)
{
    char buffer[GPS_BUFFER_SIZE];
    strncpy(buffer, sentence, GPS_BUFFER_SIZE - 1);
    buffer[GPS_BUFFER_SIZE - 1] = '\0';

    // Tokenize: $GPGGA,time,lat,N/S,lon,E/W,fix,sats,hdop,alt,M,...
    char *token = strtok(buffer, ",");
    int field = 0;
    
    float lat_deg = 0, lon_deg = 0;
    char lat_dir = 'N', lon_dir = 'E';
    int fix_quality = 0;

    while (token != NULL) {
        switch (field) {
            case 1:  // UTC time (hhmmss.sss)
                if (strlen(token) >= 6) {
                    data->hour = (token[0] - '0') * 10 + (token[1] - '0');
                    data->minute = (token[2] - '0') * 10 + (token[3] - '0');
                    data->second = (token[4] - '0') * 10 + (token[5] - '0');
                }
                break;
            case 2:  // Latitude (ddmm.mmmm)
                lat_deg = atof(token);
                break;
            case 3:  // N/S
                lat_dir = token[0];
                break;
            case 4:  // Longitude (dddmm.mmmm)
                lon_deg = atof(token);
                break;
            case 5:  // E/W
                lon_dir = token[0];
                break;
            case 6:  // Fix quality (0=invalid, 1=GPS, 2=DGPS)
                fix_quality = atoi(token);
                break;
            case 7:  // Number of satellites
                data->satellites = atoi(token);
                break;
            case 9:  // Altitude
                data->altitude = atof(token);
                break;
        }
        token = strtok(NULL, ",");
        field++;
    }

    // Convert NMEA format (ddmm.mmmm) to decimal degrees
    if (lat_deg > 0) {
        int lat_d = (int)(lat_deg / 100);
        float lat_m = lat_deg - (lat_d * 100);
        data->latitude = lat_d + (lat_m / 60.0);
        if (lat_dir == 'S') data->latitude = -data->latitude;
    }

    if (lon_deg > 0) {
        int lon_d = (int)(lon_deg / 100);
        float lon_m = lon_deg - (lon_d * 100);
        data->longitude = lon_d + (lon_m / 60.0);
        if (lon_dir == 'W') data->longitude = -data->longitude;
    }

    data->valid = (fix_quality > 0);
    return data->valid;
}

void Gps_Init(void)
{
    // Configure UART for GPS
    g_Gps_UartConfig.port = GPS_UART_PORT;
    g_Gps_UartConfig.tx_pin = GPS_TX_PIN;
    g_Gps_UartConfig.rx_pin = GPS_RX_PIN;
    g_Gps_UartConfig.baud_rate = GPS_BAUD_RATE;

    Uart_Init(&g_Gps_UartConfig);

#if GPS_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "✅ GPS initialized on UART%d (TX:%d, RX:%d, Baud:%d)",
             GPS_UART_PORT, GPS_TX_PIN, GPS_RX_PIN, GPS_BAUD_RATE);
#endif
}

void Gps_PowerOn(void)
{
    // تفعيل GPS
    Uart_Write("AT+CGPSPWR=1\r\n", strlen("AT+CGPSPWR=1\r\n"));
    vTaskDelay(pdMS_TO_TICKS(1000));

    // تأكيد baud rate للـ NMEA output (اختياري، بس كويس لو غيرته قبل كده)
    Uart_Write("AT+CGPSIPR=9600\r\n", strlen("AT+CGPSIPR=9600\r\n"));
    vTaskDelay(pdMS_TO_TICKS(500));

    // Cold start reset (أسرع fix في أول مرة، يمسح البيانات القديمة ويبدأ من الصفر)
    Uart_Write("AT+CGPSRST=0\r\n", strlen("AT+CGPSRST=0\r\n"));  // 0 = Cold start
    vTaskDelay(pdMS_TO_TICKS(1000));

    // تفعيل إخراج كل جمل NMEA (GGA + RMC + GSA + GSV + إلخ)
    Uart_Write("AT+CGPSOUT=255\r\n", strlen("AT+CGPSOUT=255\r\n"));  // 255 = كل الـ sentences
    vTaskDelay(pdMS_TO_TICKS(1000));

#if GPS_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "GPS powered ON with FULL NMEA stream (incl. GSA for PDOP)");
#endif
}

void Gps_PowerOff(void)
{
    // Send AT command to turn off GPS
    const char *cmd = "AT+CGPSPWR=0\r\n";
    Uart_Write(cmd, strlen(cmd));

#if GPS_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "GPS Power OFF");
#endif
}


int Gps_Main(Gps_DataType *data)
{
    // Clear buffer
    memset(g_Gps_Buffer, 0, GPS_BUFFER_SIZE);

    // Read GPS data from UART
    int bytes_read = Uart_Read(g_Gps_Buffer, GPS_BUFFER_SIZE - 1, 5000);

    if (bytes_read <= 0) {
#if GPS_DEBUG_ENABLED == STD_ON
        ESP_LOGW(TAG, "No GPS data received");
#endif
        data->valid = false;
        return -1;
    }

    // Null-terminate
    g_Gps_Buffer[bytes_read] = '\0';



int Gps_Main(Gps_DataType *data)
{
    // Clear buffer
    memset(g_Gps_Buffer, 0, GPS_BUFFER_SIZE);

    // Read GPS data from UART
    int bytes_read = Uart_Read(g_Gps_Buffer, GPS_BUFFER_SIZE - 1, 5000);

    if (bytes_read <= 0) {
#if GPS_DEBUG_ENABLED == STD_ON
        ESP_LOGW(TAG, "No GPS data received");
#endif
        data->valid = false;
        return -1;
    }

    // Null-terminate
    g_Gps_Buffer[bytes_read] = '\0';

    // Look for GPGGA sentence
    char *gpgga = strstr((char *)g_Gps_Buffer, "$GPGGA");
    
    if (gpgga != NULL) {
        // Find end of sentence
        char *end = strstr(gpgga, "\r\n");
        if (end != NULL) {
            *end = '\0';  // Terminate sentence
            
            // Parse GPGGA
            if (Gps_ParseGPGGA(gpgga, data)) {
#if GPS_DEBUG_ENABLED == STD_ON

                ESP_LOGI(TAG, " Lat: %.6f, Lon: %.6f, Alt: %.1fm, Sats: %d",
                ESP_LOGI(TAG, "📍 Lat: %.6f, Lon: %.6f, Alt: %.1fm, Sats: %d",

                         data->latitude, data->longitude, data->altitude, data->satellites);
#endif
                return 0;  // Success
            }
        }
    }

#if GPS_DEBUG_ENABLED == STD_ON
    ESP_LOGW(TAG, "No valid GPS fix");
#endif
    data->valid = false;
    return -1;
}

#else

// Stub functions when GPS is disabled
void Gps_Init(void)
{
    // Do nothing
}

int Gps_Main(Gps_DataType *data)
{
    return -1;
}

void Gps_PowerOn(void)
{
    // Do nothing
}


}

#else

// Stub functions when GPS is disabled
void Gps_Init(void)
{
    // Do nothing
}

int Gps_Main(Gps_DataType *data)
{
    return -1;
}

void Gps_PowerOn(void)
{
    // Do nothing
}

void Gps_PowerOff(void)
{
    // Do nothing
}

#endif // GPS_ENABLED