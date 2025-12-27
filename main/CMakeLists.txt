# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
        SRCS 
        "main.c"
        "../Src/Hal/Gpio/gpio.c" # list the source files of this component
        "../Src/Hal/ADC/adc.c"      # list the source files of this component
        "../Src/Hal/I2C/i2c.c"    # list the source files of this component
        "../Src/Hal/UART/uart.c"    # list the source files of this component
        "../Src/Hal/WiFi/wifi.c"       # list the source files of this component
        "../Src/App/ULTRASONIC/ultrasonic.c"       # list the source files of this component
    INCLUDE_DIRS 
        "."
        "../Src/Hal/GPIO" # list the public include directories of this component
        "../Src/Hal/ADC"  
        "../Src/Hal/I2C"    
        "../Src/Hal/UART"
        "../Src/Hal/WiFi"
        "../Src/App/ULTRASONIC"
                # optional, add here public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
