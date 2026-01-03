#!/usr/bin/env python3
"""
Test script to verify MQTT broker connection and topic subscriptions
Use this to test if MQTT broker is working before running the main simulator
"""

import paho.mqtt.client as mqtt
import json
import time

# MQTT Configuration
MQTT_BROKER_URI = "localhost"  # Change to your MQTT broker address
MQTT_BROKER_PORT = 1883
MQTT_CLIENT_ID = "test_client"

# MQTT Authentication - SET YOUR USERNAME AND PASSWORD HERE
# Must match the credentials in code.py and mosquitto passwd file
MQTT_USERNAME = "asm_user"  # CHANGE THIS to your MQTT username
MQTT_PASSWORD = "543as"  # CHANGE THIS to your MQTT password

# Topics to subscribe to
TOPICS = [
    "esp32_client/esp_main/gps/location",
    "esp32_client/esp_main/gps/speed",
    "esp32_client/esp_main/gsm/message",
    "esp32_client/esp_main/dht11_cabin/data",
    "esp32_client/esp_main/ldr/headlight",
    "esp32_client/esp_main/ultrasonic/distance",
    "esp32_client/esp_main/imu/vibration_engine",
    "esp32_client/esp_main/imu/rms",
    "esp32_client/esp_main/temp_engine/temperature"
]

def on_connect(client, userdata, flags, rc):
    """Callback when MQTT client connects"""
    if rc == 0:
        print(f"✓ Connected to MQTT broker ({MQTT_BROKER_URI}:{MQTT_BROKER_PORT})")
        print(f"Subscribing to {len(TOPICS)} topics...")
        for topic in TOPICS:
            client.subscribe(topic, qos=1)
            print(f"  - {topic}")
    else:
        print(f"✗ Connection failed with code {rc}")

def on_disconnect(client, userdata, rc):
    """Callback when MQTT client disconnects"""
    print(f"✗ Disconnected from MQTT broker (rc={rc})")

def on_message(client, userdata, msg):
    """Callback when message is received"""
    try:
        payload = json.loads(msg.payload.decode())
        print(f"\n[{msg.topic}]")
        print(f"  {json.dumps(payload, indent=2)}")
    except json.JSONDecodeError:
        print(f"\n[{msg.topic}]")
        print(f"  {msg.payload.decode()}")

def main():
    print("="*60)
    print("NOVA-CAR MQTT Test Client")
    print("="*60)
    print(f"Broker: {MQTT_BROKER_URI}:{MQTT_BROKER_PORT}")
    print("="*60 + "\n")
    
    # Create MQTT client
    client = mqtt.Client(client_id=MQTT_CLIENT_ID)
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message
    
    # Set username and password if provided
    if MQTT_USERNAME and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    
    try:
        # Connect to broker
        print(f"Connecting to MQTT broker...")
        if MQTT_USERNAME and MQTT_PASSWORD:
            print(f"Using authentication: username={MQTT_USERNAME}")
        client.connect(MQTT_BROKER_URI, MQTT_BROKER_PORT, keepalive=120)
        
        # Start network loop
        client.loop_start()
        
        # Wait for connection
        time.sleep(2)
        
        print("\n" + "="*60)
        print("Listening for messages... (Press Ctrl+C to stop)")
        print("="*60 + "\n")
        
        # Keep running
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n\nShutting down...")
        client.loop_stop()
        client.disconnect()
        print("Goodbye!")
    except Exception as e:
        print(f"\n✗ Error: {e}")
        print("\nMake sure:")
        print("1. Docker containers are running: docker-compose up -d")
        print("2. Mosquitto is accessible at localhost:1883")
        print("3. Firewall allows connection to port 1883")

if __name__ == "__main__":
    main()

