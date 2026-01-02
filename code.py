#!/usr/bin/env python3
"""
ESP32 Simulator - WiFi and MQTT Publisher
Simulates ESP32 board connecting to WiFi and MQTT broker (Mosquitto)
Publishes sensor data to multiple MQTT topics in JSON format
All data includes client ID, node ID, and sub-node IDs
"""

import time
import json
import socket
import paho.mqtt.client as mqtt
from typing import Optional
import random
import math

# ==================== Configuration (from Cfg.h) ====================
# WiFi Configuration
WIFI_SSID = "we#100"
WIFI_PASSWORD = "#50006000#"

# MQTT Configuration
# For local testing with Docker, use "localhost" or "127.0.0.1"
# For remote/EC2 deployment, use the actual IP address
MQTT_BROKER_URI = "18.197.149.97"  # Change to "192.168.1.101" or your MQTT broker IP
MQTT_BROKER_PORT = 1883
MQTT_CLIENT_ID = "esp32_client"
MQTT_USERNAME =  "asm_user" # Set to None for anonymous access, or provide username
MQTT_PASSWORD =  "543as" # Set to None for anonymous access, or provide password

# Node and Sub-Node IDs
ESP_CLIENT_ID = "esp32_client"
ESP_NODE_ID = "esp_main"
GPS_SUB_NODE_ID = "gps"
GSM_SUB_NODE_ID = "gsm"
DHT11_SUB_NODE_ID = "dht11_cabin"
LDR_SUB_NODE_ID = "ldr"
ULTRASONIC_SUB_NODE_ID = "ultrasonic"
IMU_SUB_NODE_ID = "imu"
TEMP_ENGINE_SUB_NODE_ID = "temp_engine"

# MQTT Topics Structure: {client_id}/{node_id}/{sub_node_id}
MQTT_TOPIC_GPS_LOCATION = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{GPS_SUB_NODE_ID}/location"
MQTT_TOPIC_GPS_SPEED = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{GPS_SUB_NODE_ID}/speed"
MQTT_TOPIC_GSM = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{GSM_SUB_NODE_ID}/message"
MQTT_TOPIC_DHT11_CABIN = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{DHT11_SUB_NODE_ID}/data"
MQTT_TOPIC_LDR = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{LDR_SUB_NODE_ID}/headlight"
MQTT_TOPIC_ULTRASONIC = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{ULTRASONIC_SUB_NODE_ID}/distance"
MQTT_TOPIC_IMU_VIBRATION = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{IMU_SUB_NODE_ID}/vibration_engine"
MQTT_TOPIC_IMU_RMS = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{IMU_SUB_NODE_ID}/rms"
MQTT_TOPIC_TEMP_ENGINE = f"{ESP_CLIENT_ID}/{ESP_NODE_ID}/{TEMP_ENGINE_SUB_NODE_ID}/temperature"

# Publishing interval (60 seconds)
PUBLISH_INTERVAL = 60

# Value gaps and ranges for realistic simulation
GPS_LAT_BASE = 30.0444
GPS_LON_BASE = 31.2357
GPS_LAT_GAP = 0.0005  # Gap between values
GPS_LON_GAP = 0.0005
GPS_SPEED_MIN = 40
GPS_SPEED_MAX = 120
GPS_SPEED_GAP = 5  # Speed increments

TEMP_CABIN_MIN = 20.0
TEMP_CABIN_MAX = 35.0
TEMP_CABIN_GAP = 0.5

HUMIDITY_MIN = 40.0
HUMIDITY_MAX = 70.0
HUMIDITY_GAP = 2.0

TEMP_ENGINE_MIN = 80.0
TEMP_ENGINE_MAX = 110.0
TEMP_ENGINE_GAP = 1.5

DISTANCE_MIN = 20
DISTANCE_MAX = 200
DISTANCE_GAP = 10

LDR_THRESHOLD = 500  # Threshold for headlight failure detection

IMU_RMS_MIN = 0.05
IMU_RMS_MAX = 0.15
IMU_RMS_GAP = 0.01

# ==================== Global Variables ====================
mqtt_client: Optional[mqtt.Client] = None
is_connected = False

# State variables for generating different values with gaps
gps_lat_counter = 0
gps_lon_counter = 0
gps_speed_counter = 0
temp_cabin_counter = 0
humidity_counter = 0
temp_engine_counter = 0
distance_counter = 0
imu_rms_counter = 0

# ==================== WiFi Simulation ====================
def check_wifi_connection() -> bool:
    """
    Simulates WiFi connection by checking network connectivity.
    In real ESP32, this would be handled by esp_wifi_start() and IP_EVENT_STA_GOT_IP.
    """
    try:
        # Try to connect to a well-known server (Google DNS) to check internet connectivity
        socket.create_connection(("8.8.8.8", 53), timeout=3)
        print("[WIFI] ✓ WiFi connected (network connectivity verified)")
        return True
    except OSError:
        print("[WIFI] ✗ WiFi connection failed (no network connectivity)")
        return False

def wifi_init_sta():
    """
    Simulates Wifi_Init_Sta() from wifi.c
    Initializes WiFi station mode and connects to network.
    """
    print("[WIFI] Initializing WiFi Station mode...")
    print(f"[WIFI] SSID: {WIFI_SSID}")
    
    # Simulate WiFi initialization delay
    time.sleep(1)
    
    # Check network connectivity (simulates WiFi connection)
    if check_wifi_connection():
        print("[WIFI] ✓ WiFi initialized successfully")
        return True
    else:
        print("[WIFI] ✗ WiFi initialization failed")
        return False

# ==================== MQTT Functions ====================
def on_connect(client, userdata, flags, rc):
    """
    Callback when MQTT client connects to broker.
    """
    global is_connected
    if rc == 0:
        is_connected = True
        print(f"[MQTT] ✓ Connected to MQTT broker ({MQTT_BROKER_URI}:{MQTT_BROKER_PORT})")
        print(f"[MQTT] Client ID: {MQTT_CLIENT_ID}")
    else:
        is_connected = False
        print(f"[MQTT] ✗ Connection failed with code {rc}")

def on_disconnect(client, userdata, rc):
    """
    Callback when MQTT client disconnects from broker.
    """
    global is_connected
    is_connected = False
    print(f"[MQTT] ✗ Disconnected from MQTT broker (rc={rc})")

def on_publish(client, userdata, mid):
    """
    Callback when message is published.
    """
    pass

def mqtt_init():
    """
    Simulates Mqtt_Init() from mqtt.c
    Initializes MQTT client and connects to broker.
    """
    global mqtt_client
    
    print("[MQTT] Initializing MQTT client...")
    print(f"[MQTT] Broker: {MQTT_BROKER_URI}:{MQTT_BROKER_PORT}")
    print(f"[MQTT] Client ID: {MQTT_CLIENT_ID}")
    print(f"[MQTT] Username: {MQTT_USERNAME}")
    
    # Create MQTT client
    mqtt_client = mqtt.Client(client_id=MQTT_CLIENT_ID)
    
    # Set callbacks
    mqtt_client.on_connect = on_connect
    mqtt_client.on_disconnect = on_disconnect
    mqtt_client.on_publish = on_publish
    
    # Set username and password (if provided)
    if MQTT_USERNAME and MQTT_PASSWORD:
        mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    
    try:
        # Connect to broker
        mqtt_client.connect(MQTT_BROKER_URI, MQTT_BROKER_PORT, keepalive=60)
        
        # Start network loop (non-blocking)
        mqtt_client.loop_start()
        
        # Wait for connection
        timeout = 5
        start_time = time.time()
        while not is_connected and (time.time() - start_time) < timeout:
            time.sleep(0.1)
        
        if is_connected:
            print("[MQTT] ✓ MQTT initialized successfully")
            return True
        else:
            print("[MQTT] ✗ MQTT connection timeout")
            return False
            
    except Exception as e:
        print(f"[MQTT] ✗ MQTT initialization failed: {e}")
        return False

def mqtt_publish(topic: str, data: str, qos: int = 1, retain: int = 0):
    """
    Simulates Mqtt_Publish() from mqtt.c
    Publishes data to MQTT topic.
    """
    global mqtt_client, is_connected
    
    if not mqtt_client or not is_connected:
        print(f"[MQTT] ✗ Cannot publish: MQTT client not connected")
        return False
    
    try:
        result = mqtt_client.publish(topic, data, qos=qos, retain=retain)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            return True
        else:
            print(f"[MQTT] ✗ Failed to publish to {topic}")
            return False
    except Exception as e:
        print(f"[MQTT] ✗ Error publishing to {topic}: {e}")
        return False

# ==================== Sensor Data Generation Functions ====================
def generate_gps_location():
    """Generate GPS location with gaps between values"""
    global gps_lat_counter, gps_lon_counter
    
    lat = GPS_LAT_BASE + (gps_lat_counter * GPS_LAT_GAP)
    lon = GPS_LON_BASE + (gps_lon_counter * GPS_LON_GAP)
    
    # Reset when reaching limits
    if lat > 30.05:
        gps_lat_counter = 0
        lat = GPS_LAT_BASE
    else:
        gps_lat_counter += 1
    
    if lon > 31.24:
        gps_lon_counter = 0
        lon = GPS_LON_BASE
    else:
        gps_lon_counter += 1
    
    return round(lat, 6), round(lon, 6)

def generate_gps_speed():
    """Generate GPS speed with gaps between values"""
    global gps_speed_counter
    
    speed = GPS_SPEED_MIN + (gps_speed_counter * GPS_SPEED_GAP)
    
    if speed > GPS_SPEED_MAX:
        gps_speed_counter = 0
        speed = GPS_SPEED_MIN
    else:
        gps_speed_counter += 1
    
    return speed

def generate_cabin_temp_humidity():
    """Generate cabin temperature and humidity with gaps"""
    global temp_cabin_counter, humidity_counter
    
    temp = TEMP_CABIN_MIN + (temp_cabin_counter * TEMP_CABIN_GAP)
    if temp > TEMP_CABIN_MAX:
        temp_cabin_counter = 0
        temp = TEMP_CABIN_MIN
    else:
        temp_cabin_counter += 1
    
    humidity = HUMIDITY_MIN + (humidity_counter * HUMIDITY_GAP)
    if humidity > HUMIDITY_MAX:
        humidity_counter = 0
        humidity = HUMIDITY_MIN
    else:
        humidity_counter += 1
    
    return round(temp, 1), round(humidity, 1)

def generate_engine_temp():
    """Generate engine temperature with gaps"""
    global temp_engine_counter
    
    temp = TEMP_ENGINE_MIN + (temp_engine_counter * TEMP_ENGINE_GAP)
    if temp > TEMP_ENGINE_MAX:
        temp_engine_counter = 0
        temp = TEMP_ENGINE_MIN
    else:
        temp_engine_counter += 1
    
    return round(temp, 1)

def generate_distance():
    """Generate ultrasonic distance with gaps"""
    global distance_counter
    
    distance = DISTANCE_MIN + (distance_counter * DISTANCE_GAP)
    if distance > DISTANCE_MAX:
        distance_counter = 0
        distance = DISTANCE_MIN
    else:
        distance_counter += 1
    
    return distance

def generate_ldr_status():
    """Generate LDR status (high/low) for headlight failure detection"""
    # Simulate random light conditions
    light_value = random.randint(100, 1000)
    status = "high" if light_value > LDR_THRESHOLD else "low"
    return status, light_value

def generate_imu_vibration():
    """Generate IMU vibration engine status (true/false)"""
    # Simulate engine vibration detection
    vibration_detected = random.random() > 0.7  # 30% chance of vibration
    return vibration_detected

def generate_imu_rms():
    """Generate IMU RMS value with gaps"""
    global imu_rms_counter
    
    rms = IMU_RMS_MIN + (imu_rms_counter * IMU_RMS_GAP)
    if rms > IMU_RMS_MAX:
        imu_rms_counter = 0
        rms = IMU_RMS_MIN
    else:
        imu_rms_counter += 1
    
    return round(rms, 3)

def generate_gsm_message(iter_count, temp_cabin, temp_engine, vibration, ldr_status):
    """Generate GSM message in JSON format with car status"""
    car_status = "normal"
    message_type = "status"
    message_text = "All systems operational"
    
    # Determine car status based on sensor readings
    if temp_engine > 105.0:
        car_status = "critical"
        message_type = "alert"
        message_text = "Engine temperature critical! Immediate attention required."
    elif temp_engine > 100.0:
        car_status = "warning"
        message_type = "warning"
        message_text = "Engine temperature high. Monitor closely."
    elif temp_cabin > 32.0:
        car_status = "warning"
        message_type = "alert"
        message_text = "Cabin temperature high. Check climate control."
    elif vibration:
        car_status = "warning"
        message_type = "alert"
        message_text = "Engine vibration detected. Service recommended."
    elif ldr_status == "low":
        car_status = "warning"
        message_type = "warning"
        message_text = "Headlight failure detected. Check lighting system."
    elif iter_count % 10 == 0:
        car_status = "normal"
        message_type = "info"
        message_text = "Driver status check: All systems normal."
    
    gsm_data = {
        "client_id": ESP_CLIENT_ID,
        "node_id": ESP_NODE_ID,
        "sub_node_id": GSM_SUB_NODE_ID,
        "message_type": message_type,
        "car_status": car_status,
        "message": message_text,
        "timestamp": int(time.time())
    }
    
    return json.dumps(gsm_data)

# ==================== Environment Task (from main.c) ====================
def environment_task():
    """
    Simulates Environment_Task() from main.c
    Publishes sensor data to all MQTT topics in JSON format every 10 seconds.
    All data includes client ID, node ID, and sub-node IDs.
    """
    iter_count = 0
    
    print("\n" + "="*60)
    print("Starting NOVA-CAR ESP32 - Multi-Sensor MQTT Publisher v3.0")
    print("="*60)
    print(f"Client ID: {ESP_CLIENT_ID}")
    print(f"Node ID: {ESP_NODE_ID}")
    print("Sub-Nodes: GPS, GSM, DHT11, LDR, Ultrasonic, IMU, Temp_Engine")
    print("All data published in JSON format with IDs")
    print("="*60 + "\n")
    
    while True:
        print(f"\n[MAIN] === Publishing Sensor Data - Iteration {iter_count} ===")
        
        if is_connected:
            # 1. GPS Location Topic (separate from speed)
            lat, lon = generate_gps_location()
            gps_location_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": GPS_SUB_NODE_ID,
                "latitude": lat,
                "longitude": lon,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_GPS_LOCATION, json.dumps(gps_location_data), 1, 0)
            print(f"[GPS] Location: lat={lat}, lon={lon}")
            
            # 2. GPS Speed Topic (separate topic)
            speed = generate_gps_speed()
            gps_speed_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": GPS_SUB_NODE_ID,
                "speed_kmh": speed,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_GPS_SPEED, json.dumps(gps_speed_data), 1, 0)
            print(f"[GPS] Speed: {speed} km/h")
            
            # 3. DHT11 Cabin Temperature and Humidity (combined in one topic)
            temp_cabin, humidity = generate_cabin_temp_humidity()
            dht11_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": DHT11_SUB_NODE_ID,
                "temperature_c": temp_cabin,
                "humidity_percent": humidity,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_DHT11_CABIN, json.dumps(dht11_data), 1, 0)
            print(f"[DHT11] Cabin: temp={temp_cabin}°C, humidity={humidity}%")
            
            # 4. Engine Temperature (separate topic, JSON format)
            temp_engine = generate_engine_temp()
            temp_engine_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": TEMP_ENGINE_SUB_NODE_ID,
                "temperature_c": temp_engine,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_TEMP_ENGINE, json.dumps(temp_engine_data), 1, 0)
            print(f"[TEMP_ENGINE] Temperature: {temp_engine}°C")
            
            # 5. LDR Headlight Status (high/low for failure detection)
            ldr_status, ldr_value = generate_ldr_status()
            ldr_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": LDR_SUB_NODE_ID,
                "headlight_status": ldr_status,
                "light_value": ldr_value,
                "failure_detected": ldr_status == "low",
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_LDR, json.dumps(ldr_data), 1, 0)
            print(f"[LDR] Headlight: {ldr_status} (value={ldr_value})")
            
            # 6. Ultrasonic Distance (JSON format)
            distance = generate_distance()
            ultrasonic_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": ULTRASONIC_SUB_NODE_ID,
                "distance_cm": distance,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_ULTRASONIC, json.dumps(ultrasonic_data), 1, 0)
            print(f"[ULTRASONIC] Distance: {distance} cm")
            
            # 7. IMU Vibration Engine (true/false)
            vibration = generate_imu_vibration()
            imu_vibration_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": IMU_SUB_NODE_ID,
                "vibration_engine": vibration,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_IMU_VIBRATION, json.dumps(imu_vibration_data), 1, 0)
            print(f"[IMU] Vibration Engine: {vibration}")
            
            # 8. IMU RMS Value (JSON format)
            rms_value = generate_imu_rms()
            imu_rms_data = {
                "client_id": ESP_CLIENT_ID,
                "node_id": ESP_NODE_ID,
                "sub_node_id": IMU_SUB_NODE_ID,
                "rms_value": rms_value,
                "timestamp": int(time.time())
            }
            mqtt_publish(MQTT_TOPIC_IMU_RMS, json.dumps(imu_rms_data), 1, 0)
            print(f"[IMU] RMS: {rms_value}")
            
            # 9. GSM Message (JSON format with car status)
            gsm_message = generate_gsm_message(iter_count, temp_cabin, temp_engine, vibration, ldr_status)
            mqtt_publish(MQTT_TOPIC_GSM, gsm_message, 1, 0)
            gsm_parsed = json.loads(gsm_message)
            print(f"[GSM] Message: {gsm_parsed['message_type']} - {gsm_parsed['message']}")
            
            print("[MAIN] ✓ All sensor data published successfully in JSON format!")
        else:
            print("[MAIN] ✗ MQTT disabled — no publish (not connected)")
        
        iter_count += 1
        time.sleep(PUBLISH_INTERVAL)  # Wait 10 seconds (same as main.c)

# ==================== Main Function (simulates app_main) ====================
def main():
    """
    Simulates app_main() from main.c
    Initializes WiFi and MQTT, then starts the environment task.
    """
    print("\n" + "="*60)
    print("ESP32 Simulator - WiFi & MQTT Publisher")
    print("="*60 + "\n")
    
    # Initialize WiFi (simulates Wifi_Init_Sta())
    if not wifi_init_sta():
        print("\n[ERROR] WiFi initialization failed. Exiting...")
        return
    
    # Wait a bit for WiFi to stabilize
    time.sleep(2)
    
    # Initialize MQTT (simulates Mqtt_Init())
    if not mqtt_init():
        print("\n[ERROR] MQTT initialization failed. Exiting...")
        return
    
    # Wait a bit for MQTT connection to stabilize
    time.sleep(1)
    
    # Start environment task (simulates xTaskCreate)
    print("\n[MAIN] Starting environment task...")
    try:
        environment_task()
    except KeyboardInterrupt:
        print("\n\n[MAIN] Shutting down...")
        if mqtt_client:
            mqtt_client.loop_stop()
            mqtt_client.disconnect()
        print("[MAIN] Goodbye!")

if __name__ == "__main__":
    main()
