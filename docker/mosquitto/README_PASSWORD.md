# MQTT Password Setup Guide

## Quick Setup

### Option 1: Using Docker (Recommended)

**Windows:**
```cmd
docker\mosquitto\create_password.bat <username> <password>
```

**Linux/Mac:**
```bash
chmod +x docker/mosquitto/create_password.sh
docker/mosquitto/create_password.sh <username> <password>
```

**Or use Docker directly:**
```bash
docker run -it --rm -v "$PWD/docker/mosquitto/config:/mosquitto/config" eclipse-mosquitto:2.0 mosquitto_passwd -c -b /mosquitto/config/passwd <username> <password>
```

### Option 2: Manual Setup

1. Install Mosquitto client tools:
   - **Ubuntu/Debian**: `sudo apt-get install mosquitto-clients`
   - **macOS**: `brew install mosquitto`
   - **Windows**: Download from https://mosquitto.org/download/

2. Create password file:
   ```bash
   mosquitto_passwd -c -b docker/mosquitto/config/passwd <username> <password>
   ```

## Example

```bash
# Create user "mqtt_user" with password "SecurePass123"
docker run -it --rm -v "$PWD/docker/mosquitto/config:/mosquitto/config" eclipse-mosquitto:2.0 mosquitto_passwd -c -b /mosquitto/config/passwd mqtt_user SecurePass123
```

## After Creating Password File

1. **Update code.py:**
   ```python
   MQTT_USERNAME = "mqtt_user"
   MQTT_PASSWORD = "SecurePass123"
   ```

2. **Update docker/telegraf/telegraf.conf:**
   ```toml
   username = "mqtt_user"
   password = "SecurePass123"
   ```

3. **Restart containers:**
   ```bash
   docker-compose restart mosquitto telegraf
   ```

## Adding More Users

To add additional users (without -c flag to avoid overwriting):
```bash
docker run -it --rm -v "$PWD/docker/mosquitto/config:/mosquitto/config" eclipse-mosquitto:2.0 mosquitto_passwd -b /mosquitto/config/passwd <new_username> <new_password>
```

## Security Notes

- The password file is hashed, so it's safe to commit to version control
- For production, use strong passwords
- Consider using environment variables for passwords in production
- The password file is mounted as a volume in docker-compose.yml

