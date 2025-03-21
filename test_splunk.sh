#!/bin/bash

# Set timeout values
CONNECT_TIMEOUT=5
MAX_TIMEOUT=10

# Test data that matches our Particle device format
echo "Testing Splunk HEC endpoint..."
# Try different connection methods
echo "Testing Splunk connectivity..."

# Method 1: Direct HTTPS (with IP)
echo "\nMethod 1: Testing direct HTTPS connection to IP..."
curl -v -k --ipv4 "https://104.21.48.1:8088/services/collector" \
  --resolve "splunk.tipflips.com:8088:104.21.48.1" \
  -H "Authorization: Splunk f697778b-57ab-46b5-8207-c858760b76fc" \
  -H "Content-Type: application/json" \
  --connect-timeout $CONNECT_TIMEOUT \
  --max-time $MAX_TIMEOUT \
  -w "\nHTTP Status: %{http_code}\nTime: %{time_total} seconds\n" \
  -d '{
    "event": "particle_sensor_data",
    "fields": {
      "pm2_5": 25.45,
      "pm10": 75.32,
      "temperature": 27.80,
      "device_id": "e00fce68bd9b3403195a5d08",
      "timestamp": '$(date +%s)'
    }
  }'

# Check curl exit status
STATUS=$?
if [ $STATUS -eq 0 ]; then
    echo "Test completed successfully"
elif [ $STATUS -eq 28 ]; then
    echo "Error: Request timed out after $MAX_TIMEOUT seconds"
else
    echo "Error: curl command failed with status $STATUS"
fi
