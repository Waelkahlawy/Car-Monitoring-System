#!/bin/bash
# Script to create MQTT password file
# Usage: ./create_password.sh <username> <password>

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <username> <password>"
    echo "Example: $0 mqtt_user mypassword123"
    exit 1
fi

USERNAME=$1
PASSWORD=$2

# Check if mosquitto_passwd is available
if ! command -v mosquitto_passwd &> /dev/null; then
    echo "Error: mosquitto_passwd not found."
    echo "Please install mosquitto-clients package:"
    echo "  Ubuntu/Debian: sudo apt-get install mosquitto-clients"
    echo "  macOS: brew install mosquitto"
    echo "  Windows: Install Mosquitto from https://mosquitto.org/download/"
    exit 1
fi

# Create password file
echo "Creating password file..."
mosquitto_passwd -c -b docker/mosquitto/config/passwd "$USERNAME" "$PASSWORD"

if [ $? -eq 0 ]; then
    echo "✓ Password file created successfully!"
    echo "  Username: $USERNAME"
    echo "  Password file: docker/mosquitto/config/passwd"
    echo ""
    echo "Next steps:"
    echo "1. Update code.py with: MQTT_USERNAME = '$USERNAME', MQTT_PASSWORD = '$PASSWORD'"
    echo "2. Update docker/telegraf/telegraf.conf with the same credentials"
    echo "3. Restart containers: docker-compose restart mosquitto telegraf"
else
    echo "✗ Failed to create password file"
    exit 1
fi

