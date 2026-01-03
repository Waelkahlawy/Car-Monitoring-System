#!/usr/bin/env python3
# """
# ESP32 Simulator - WiFi and MQTT Publisher
# Simulates ESP32 board connecting to WiFi and MQTT broker (Mosquitto)
# Publishes sensor data to multiple MQTT topics, same as main.c
# """

import time
import json
import socket
import paho.mqtt.client as mqtt
from typing import Optional

# ==================== Configuration (from Cfg.h) ====================
# WiFi Configuration
WIFI_SSID = "we#100"
WIFI_PASSWORD = "#50006000#"

# # MQTT Configuration
MQTT_BROKER_URI = "192.168.1.101"  # Extract from "mqtt://192.168.1.101:1883"
MQTT_BROKER_PORT = 1883
MQTT_CLIENT_ID = "esp32_client"
MQTT_USERNAME = "asm_user"
MQTT_PASSWORD = "543as"

# MQTT Topics (from main.c)
MQTT_TOPIC_GPS = "esp32_client/esp_main/gps"
MQTT_TOPIC_GSM = "esp32_client/esp_main/gsm"
MQTT_TOPIC_IMU_TRA = "esp32_client/esp_main/imu_tracking"
MQTT_TOPIC_TEMP_CABIN_ENGINE = "esp32_client/esp_main/temperature_cabin_engine"
MQTT_TOPIC_TEMP_ENGINE1 = "esp32_client/esp_main/temperature_engine1"
MQTT_TOPIC_HUMIDITY = "esp32_client/esp_main/humidity"
MQTT_TOPIC_ENV = "esp32_client/esp_main/environment"
MQTT_TOPIC_ULTRASONIC = "esp32_client/esp_main/ultrasonic"
MQTT_TOPIC_RFID = "esp32_client/esp_main/rfid"
MQTT_TOPIC_LIGHT = "esp32_client/esp_main/light"
MQTT_TOPIC_IMU_VIB = "esp32_client/esp_main/imu_vibration"

# Publishing interval (10 seconds, same as main.c)
PUBLISH_INTERVAL = 10

# ==================== Global Variables ====================
mqtt_client: Optional[mqtt.Client] = None
is_connected = False

# ==================== WiFi Simulation ====================
def check_wifi_connection() -> bool:
    # """
    # Simulates WiFi connection by checking network connectivity.
    # In real ESP32, this would be handled by esp_wifi_start() and IP_EVENT_STA_GOT_IP.
    # """
    try:
        # Try to connect to a well-known server (Google DNS) to check internet connectivity
        socket.create_connection(("8.8.8.8", 53), timeout=3)
        print("[WIFI] ✓ WiFi connected (network connectivity verified)")
        return True
    except OSError:
        print("[WIFI] ✗ WiFi connection failed (no network connectivity)")
        return False

def wifi_init_sta():
    # """
    # Simulates Wifi_Init_Sta() from wifi.c
    # Initializes WiFi station mode and connects to network.
    # """
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
    # """
    # Callback when MQTT client connects to broker.
    # """
    global is_connected
    if rc == 0:
        is_connected = True
        print(f"[MQTT] ✓ Connected to MQTT broker ({MQTT_BROKER_URI}:{MQTT_BROKER_PORT})")
        print(f"[MQTT] Client ID: {MQTT_CLIENT_ID}")
    else:
        is_connected = False
        print(f"[MQTT] ✗ Connection failed with code {rc}")

def on_disconnect(client, userdata, rc):
    # """
    # Callback when MQTT client disconnects from broker.
    # """
    global is_connected
    is_connected = False
    print(f"[MQTT] ✗ Disconnected from MQTT broker (rc={rc})")

def on_publish(client, userdata, mid):
    # """
    # Callback when message is published.
    # """
    # Optional: Uncomment for detailed publish confirmation
    # print(f"[MQTT] Message published (mid={mid})")
    pass

def mqtt_init():
    # """
    # Simulates Mqtt_Init() from mqtt.c
    # Initializes MQTT client and connects to broker.
    # """
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
    
    # Set username and password
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
    # """
    # Simulates Mqtt_Publish() from mqtt.c
    # Publishes data to MQTT topic.
    # """
    global mqtt_client, is_connected
    
    if not mqtt_client or not is_connected:
        print(f"[MQTT] ✗ Cannot publish: MQTT client not connected")
        return False
    
    try:
        result = mqtt_client.publish(topic, data, qos=qos, retain=retain)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            # Uncomment for detailed logging
            # print(f"[MQTT] Published to {topic}: {data}")
            return True
        else:
            print(f"[MQTT] ✗ Failed to publish to {topic}")
            return False
    except Exception as e:
        print(f"[MQTT] ✗ Error publishing to {topic}: {e}")
        return False

# ==================== Environment Task (from main.c) ====================
def environment_task():
    # """
    # Simulates Environment_Task() from main.c
    # Publishes dummy sensor data to all MQTT topics every 10 seconds.
    # """
    iter_count = 0
    fake_lat = 30.0444
    fake_lon = 31.2357
    fake_temp = 25.0
    fake_hum = 55.0
    fake_speed = 80
    
    fake_distance = 50
    fake_light = 500
    
    print("\n" + "="*60)
    print("Starting NOVA-CAR ESP32 - Multi-Sensor MQTT Test v2.0")
    print("="*60)
    print("Multi-dummy task started - publishing to 10 topics every 10s")
    print("="*60 + "\n")
    
    while True:
        print(f"\n[MAIN] === Publishing Dummy Data - Iteration {iter_count} ===")
        
        # Update fake data (same logic as main.c)
        fake_lat += 0.0001
        if fake_lat > 30.05:
           fake_lat = 30.0444
        
        fake_lon += 0.0001
        if fake_lon > 31.24:
            fake_lon = 31.2357
        
        fake_temp = 22.0 + (iter_count % 20)
        fake_hum = 40.0 + (iter_count % 50)
        fake_speed = 40 + (iter_count % 6)
       
        fake_distance = 20 + (iter_count % 180)
        fake_light = 100 + (iter_count % 900)
        
        # Publish to all topics (same as main.c)
        if is_connected:
            # 1. GPS Topic
            payload = json.dumps({
                "latitude": round(fake_lat, 4),
                "longitude": round(fake_lon, 4),
                "speed": fake_speed
            })
            mqtt_publish(MQTT_TOPIC_GPS, payload, 1, 0)
            
            # 2. GSM Topic - Only payload with alert messages
            # Different alerts for temperature, cabin, engine, and driver status
            if fake_temp > 35.0:
                gsm_alert = "Temperature Alert: Engine temperature critical!"
            elif fake_temp > 30.0:
                gsm_alert = "Cabin Alert: Cabin temperature high!"
            elif iter_count % 10 == 0:
                gsm_alert = "Driver Status Alert: Driver attention required!"
            elif iter_count % 7 == 0:
                gsm_alert = "Engine Alert: Engine performance check needed!"
            else:
                gsm_alert = "Temperature Alert: High temperature detected!"
            mqtt_publish(MQTT_TOPIC_GSM, gsm_alert, 1, 0)
            
            # 3. IMU Tracking Topic
            payload = json.dumps({
                "accel_x": 1.23,
                "accel_y": -0.45,
                "accel_z": 9.81
            })
            mqtt_publish(MQTT_TOPIC_IMU_TRA, payload, 1, 0)
            
            # 4. Temperature Cabin Engine Topic
            payload = f"{fake_temp:.1f}"
            mqtt_publish(MQTT_TOPIC_TEMP_CABIN_ENGINE, payload, 1, 0)
            
            # 4b. Temperature Engine1 Topic
            fake_temp_engine1 = fake_temp + 5.0  # Engine temp is usually higher
            payload = f"{fake_temp_engine1:.1f}"
            mqtt_publish(MQTT_TOPIC_TEMP_ENGINE1, payload, 1, 0)
            
            # 5. Humidity Topic
            payload = f"{fake_hum:.1f}"
            mqtt_publish(MQTT_TOPIC_HUMIDITY, payload, 1, 0)
            
            # 6. Environment Topic
            payload = json.dumps({
                "temperature": round(fake_temp, 1),
                "humidity": round(fake_hum, 1)
            })
            mqtt_publish(MQTT_TOPIC_ENV, payload, 1, 0)
            
            # 7. Ultrasonic Topic
            payload = json.dumps({
                "distance_cm": fake_distance
            })
            mqtt_publish(MQTT_TOPIC_ULTRASONIC, payload, 1, 0)
            
            # 8. RFID Topic
            payload = json.dumps({
                "card_id": "CARD_1234",
                "access_granted": True
            })
            mqtt_publish(MQTT_TOPIC_RFID, payload, 1, 0)
            
            # 9. Light Topic
            payload = str(fake_light)
            mqtt_publish(MQTT_TOPIC_LIGHT, payload, 1, 0)
            
            # 10. IMU Vibration Topic
            payload = json.dumps({
                "accel_x": 0.05,
                "accel_y": 0.03,
                "accel_z": 0.08
            })
            mqtt_publish(MQTT_TOPIC_IMU_VIB, payload, 1, 0)
            
            print("[MAIN] ✓ All 11 dummy topics published successfully!")
        else:
            print("[MAIN] ✗ MQTT disabled — no publish (not connected)")
        
        iter_count += 1
        time.sleep(PUBLISH_INTERVAL)  # Wait 10 seconds (same as main.c)

# ==================== Main Function (simulates app_main) ====================
def main():
    # """
    # Simulates app_main() from main.c
    # Initializes WiFi and MQTT, then starts the environment task.
    # """
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

