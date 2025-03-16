#!/bin/bash

# Test data that matches our Particle device format
curl -k "http://splunk.tipflips.com:8088/services/collector" \
  -H "Authorization: Splunk f697778b-57ab-46b5-8207-c858760b76fc" \
  -H "Content-Type: application/json" \
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
