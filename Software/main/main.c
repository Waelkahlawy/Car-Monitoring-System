#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../Src/Cfg.h"
#include "../Src/Hal/GPIO/gpio.h"
#include "../Src/Hal/ADC/adc.h" 
#include "../Src/Hal/UART/uart.h"
#include "../Src/Hal/I2C/i2c.h"
#include "../Src/Hal/WIFI/wifi.h"
#include "../Src/App/Modules_Driver/MQTT/mqtt.h"
#include "../Src/App/Modules_Driver/LDR/ldr.h"
#include "../Src/App//Modules_Driver/ULTRASONIC/ultrasonic.h" 
#include "../Src/App//Modules_Driver/IMU/imu.h"
#include "../Src/App//Modules_Driver/DHT11/dht11.h"
#include "../Src/App/Modules_Driver/PREDICTIVE_MAINTENANCE/predictive_maintenance.h"


#include "../Src/App/System/RTOS_MANAGER/rtos_manager.h"


// static const char *TAG = "APP_MAIN";



void app_main(void)
{
    printf("Hello from  RTOS!\n");
	RTOS_Manager_Init();

}

