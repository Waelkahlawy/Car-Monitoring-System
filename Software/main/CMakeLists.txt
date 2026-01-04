# ESP-IDF Component CMakeLists.txt
# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS 
        # Main application
        "main.c"
        
        # ================= HAL Layer =================
        "../Src/Hal/Gpio/gpio.c" 
        "../Src/Hal/ADC/adc.c"  
        "../Src/Hal/UART/uart.c"
        "../Src/Hal/I2C/i2c.c"
        "../Src/Hal/WIFI/wifi.c"
        
        # ================= Drivers =================
        "../Src/App/Modules_Driver/MQTT/mqtt.c"
        "../Src/App/Modules_Driver/DHT11/dht11.c"  
        "../Src/App/Modules_Driver/IMU/imu.c"                 
        "../Src/App/Modules_Driver/ULTRASONIC/ultrasonic.c"   
        "../Src/App/Modules_Driver/LDR/ldr.c"                 
        "../Src/App/Modules_Driver/GPS/gps.c"                 
        "../Src/App/Modules_Driver/GSM/gsm.c"
        "../Src/App/Modules_Driver/PREDICTIVE_MAINTENANCE/predictive_maintenance.cpp"
        "../Src/App/Modules_Driver/SENSOR_FUSION/sensor_fusion.c"
        
        
        # ================= Sensor Tasks =================
        "../Src/App/Services/SensorTask/DHT11_TASK/dht11_task.c"           
        "../Src/App/Services/SensorTask/IMU_TASK/imu_task.c"               
        "../Src/App/Services/SensorTask/ULTRASONIC_TASK/ultrasonic_task.c" 
        "../Src/App/Services/SensorTask/LDR_TASK/ldr_task.c"               
        "../Src/App/Services/SensorTask/GPS_TASK/gps_task.c"               
        "../Src/App/Services/SensorTask/GSM_TASK/gsm_task.c"
        "../Src/App/Services/SensorTask/PREDICTIVE_MAINTENANCE_TASK/predictive_maintenance_task.c"
        "../Src/App/Services/SensorTask/SENSOR_FUSION_TASK/sensor_fusion_task.c"
        
        # ================= Aggregate Task =================
        "../Src/App/Services/AggregateTask/aggregate_task.c"
        
        # ================= System =================
        "../Src/App/System/RTOS_MANAGER/rtos_manager.c"

    INCLUDE_DIRS
        # Main
        "."
        
        # HAL Layer
        "../Src/Hal/GPIO" 
        "../Src/Hal/ADC" 
        "../Src/Hal/UART"
        "../Src/Hal/I2C"
        "../Src/Hal/WIFI"
        
        # Drivers
        "../Src/App/Modules_Driver/MQTT"
        "../Src/App/Modules_Driver/DHT11"
        "../Src/App/Modules_Driver/IMU"
        "../Src/App/Modules_Driver/ULTRASONIC"
        "../Src/App/Modules_Driver/LDR"
        "../Src/App/Modules_Driver/GPS"
        "../Src/App/Modules_Driver/GSM"
        "../Src/App/Modules_Driver/PREDICTIVE_MAINTENANCE"
        "../Src/App/Modules_Driver/SENSOR_FUSION"
        
        # Sensor Tasks
        "../Src/App/Services/SensorTask/DHT11_TASK"
        "../Src/App/Services/SensorTask/IMU_TASK"
        "../Src/App/Services/SensorTask/ULTRASONIC_TASK"
        "../Src/App/Services/SensorTask/LDR_TASK"
        "../Src/App/Services/SensorTask/GPS_TASK"
        "../Src/App/Services/SensorTask/GSM_TASK"
        "../Src/App/Services/SensorTask/PREDICTIVE_MAINTENANCE_TASK"
        "../Src/App/Services/SensorTask/SENSOR_FUSION_TASK"
        
        # Aggregate Task
        "../Src/App/Services/AggregateTask"
        
        # System
        "../Src/App/System/RTOS_MANAGER"
     
    # PRIV_REQUIRES
    #     # TensorFlow Lite for Predictive Maintenance
    #     esp-tflite-micro
)