# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
        SRCS 
        # List the source files of this component here  
        "main.c" 
        "../Src/Hal/Gpio/gpio.c" 
        "../Src/Hal/ADC/adc.c"      
        "../Src/Hal/I2C/i2c.c"    
        "../Src/Hal/UART/uart.c"   
        "../Src/Hal/WIFI/wifi.c"    
        "../Src/App/IMU/imu.c" 
        "../Src/App/MQTT/mqtt.c"       
        "../Src/App/LDR/ldr.c" 
        "../Src/App/Ultrasonic/ultrasonic.c"   
        "../Src/App/DHT11/dht11.c"  
        "../Src/App/GPS/gps.c"  
        "../Src/App/GSM/gsm.c"

        

    INCLUDE_DIRS 
        # List the include directories of this component here
        "."
        "../Src/Hal/GPIO" 
        "../Src/Hal/ADC"  
        "../Src/Hal/I2C"    
        "../Src/Hal/UART"
        "../Src/Hal/WIFI"
        "../Src/App/MQTT"
        "../Src/App/IMU/"
        "../Src/App/LDR"
        "../Src/App/Ultrasonic"
        "../Src/App/DHT11"
        "../Src/App/GPS"
        "../Src/App/GSM"
           
                        

                        # optional, add here public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
