# Particle Splunk IoT Data Logger

This project implements a sensor data logger using a Particle IoT device that sends environmental sensor data (PM2.5, PM10, temperature) to both the Particle event console and a Splunk server via HTTP Event Collector (HEC).

## Device Information
- **Device Name:** sky_magnetic
- **Device ID:** e00fce68bd9b3403195a5d08
- **Product ID:** 36723
- **Status:** Online

## Features
- Collects simulated sensor data (PM2.5, PM10, temperature)
- Publishes data to Particle event console every 60 seconds
- Sends data to Splunk HEC for data analytics
- Implements robust error handling and retry mechanisms
- Includes a stop program for halting sensor monitoring

## Project Structure
- Standard Particle project structure
- Uses HttpClient library for Splunk integration
- Includes both main sensor program and stop program

## Configuration
### Splunk Settings
```cpp
const char* SPLUNK_HOST = "splunk.tipflips.com";
const int SPLUNK_PORT = 8088;
const char* SPLUNK_TOKEN = "your-splunk-token";
```

## Data Format
### Particle Event Data
```json
{
    "device": {
        "id": "device-id",
        "name": "sky_magnetic",
        "product_id": 36723
    },
    "data": {
        "pm2_5": 0.00,
        "pm10": 0.00,
        "temperature": 0.00
    },
    "timestamp": 0
}
```

### Splunk Event Data
```json
{
    "event": "particle_sensor_data",
    "fields": {
        "pm2_5": 0.00,
        "pm10": 0.00,
        "temperature": 0.00,
        "device_id": "device-id",
        "timestamp": 0
    }
}
```

## Usage
1. Flash the main program to start sensor monitoring:
   ```bash
   particle flash sky_magnetic particle2splunk.cpp
   ```

2. Stop sensor monitoring:
   ```bash
   particle flash sky_magnetic stop.cpp
   ```

3. Monitor sensor data:
   ```bash
   particle subscribe "sky_magnetic/sensor_data"
   ```

## Error Handling
- Implements automatic reconnection to Particle cloud
- Retries failed event publishing up to 3 times
- Skips Splunk transmission if Particle publishing fails
- Provides detailed logging via Serial monitor

## Dependencies
- Particle Device OS
- HttpClient library (automatically installed)

## Testing
### Test Splunk Integration
Use the provided `test_splunk.sh` script to test the Splunk HEC endpoint:

```bash
./test_splunk.sh
```

This will send sample sensor data matching our device format:
- Device ID: e00fce68bd9b3403195a5d08
- PM2.5, PM10, and temperature readings
- Current timestamp

## Contributing
Feel free to submit issues and enhancement requests!
