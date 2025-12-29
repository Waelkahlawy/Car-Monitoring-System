#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/_intsup.h>
#include <unistd.h>
#include "../../Hal/UART/uart.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "hal/uart_types.h"
#include "portmacro.h"
#include "esp_log.h"
#include "string.h"
#include "gps.h"

/* GPS RECIVED BUFFER SIZE Based On Full NMEA Sentences*/
#define BUFFER_SIZE     128

/* GPS RECIVED BUFFER */
static uint8_t GPS_BUFFER[BUFFER_SIZE];



static const uint8_t *TAG = "GPS_MODULE";


void GPS_Init (const Uart_ConfigType *UART_Config)
{
    /* THE LINE BELOW USED TO INTIALIZE UART PORT FOR GPS */
    Uart_Init(UART_Config);
}


void GPS_GetData (Gps_Parameters_t *GPS_Parametars)
{
    const uint8_t *start;
    uint32_t Validation;
    uint8_t Latitude[12] , Longitude[12] , Altitude[12] , Timestamp[12];
    float   Speed_Knots , CourseDirection , Status;

    /* raw_nmea(void) */
    memset(GPS_BUFFER , 0 , BUFFER_SIZE);                   /* Clears the global buffer to ensure no old data remains */
    Uart_Read(GPS_BUFFER , BUFFER_SIZE , portMAX_DELAY );   /* Read The Recived Data from GPS */
    ESP_LOGI(TAG , "%s" , GPS_BUFFER);

    
    /* This is where the raw text is turned into useful data. It looks for the $GPGGA sentence, which contains essential 3D fix data */
    start = strstr(GPS_BUFFER , "$GPGGA");  /* Searches the buffer for the string "$GPGGA". If it's not found, it skips the rest (meaning the buffer likely didn't capture a full sentence yet). */
    if (start != NULL)
    {
        sscanf(start , "$GPGGA,%*f,%11[^,],%*c,%11[^,],%*c,%*d,%*d,%*f,%11[^,],%f,%f" , Latitude , Longitude , Altitude);  /* sscanf logic: ( $GPGGA - Match header ) , ( %*f - Skip Time ) , ( %11[^,],%*c - Capture Lat, Skip 'N/S') , ( %11[^,],%*c - Capture Lon Skip 'E/W' ) , ( %*d - Skip Fix Quality ) , ( %*d - Skip Satellites ) , ( %*f - Skip HDOP ) , ( %11[^,] - Capture Altitude )*/ 
        sprintf((GPS_Parametars->Latitude ) , "%s" , Latitude );        /* Copies the Latitude  results into ( GPS_Parametars->Latitude  ) */
        sprintf((GPS_Parametars->Longitude) , "%s" , Longitude);        /* Copies the Longitude results into ( GPS_Parametars->Longitude ) */
        sprintf((GPS_Parametars->Altitude ) , "%s" , Altitude );        /* Copies the Altitude  results into ( GPS_Parametars->Altitude  ) */
    }

    start = strstr(GPS_BUFFER , "$GPRMC");
    if (start != NULL)
    {
        Validation = (start , "$GPRMC,%10[^,],%c,%*[^,],%*[^,],%*[^,],%*[^,],%f,%f" , Timestamp, &Status, &Speed_Knots, &CourseDirection);  /* sscanf logic: ( $GPGGA - Match header ) , ( %*f - Skip Time ) , ( %11[^,],%*c - Capture Lat, Skip 'N/S') , ( %11[^,],%*c - Capture Lon Skip 'E/W' ) , ( %*d - Skip Fix Quality ) , ( %*d - Skip Satellites ) , ( %*f - Skip HDOP ) , ( %11[^,] - Capture Altitude )*/ 
        switch(Validation)
        {
            case 'A':
                sprintf((GPS_Parametars->Timestamp ) , "%s" , Timestamp );        /* Copies the Timestamp results into ( GPS_Parametars->Timestamp) */
                (GPS_Parametars->Speed)  = Speed_Knots * 1.852;
                (GPS_Parametars->Course) = CourseDirection;
            case 'V': 
                ESP_LOGI(TAG , " INVALID DATA : GPS Module Hasn't Locked Onto Enough Satellites Yet ");

            default : 
                sprintf((GPS_Parametars->Timestamp ) , "%s" , Timestamp );        /* Copies the Timestamp results into ( GPS_Parametars->Timestamp) */
                (GPS_Parametars->Speed)  = Speed_Knots * 1.852;
                (GPS_Parametars->Course) = CourseDirection;
        }
        
    }

    ESP_LOGI(TAG, "Latitude  : [ %s ]" , (GPS_Parametars->Latitude ) ); 
    ESP_LOGI(TAG, "Longitude : [ %s ]" , (GPS_Parametars->Longitude) );
    ESP_LOGI(TAG, "Altitude  : [ %s ]" , (GPS_Parametars->Altitude ) );
    ESP_LOGI(TAG, "Timestamp : [ %s ]" , (GPS_Parametars->Timestamp) );
    ESP_LOGI(TAG, "Speed     : [ %.2f ] [ Km/h ]"   , (GPS_Parametars->Speed ) );
    ESP_LOGI(TAG, "Course    : [ %.2f ] [ Degree ]" , (GPS_Parametars->Course) );
}