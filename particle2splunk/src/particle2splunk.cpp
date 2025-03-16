/* 
 * Particle to Splunk Data Logger
 * Sends sensor data to Splunk HTTP Event Collector (HEC)
 */

#include "Particle.h"
#include "HttpClient.h"

// Splunk HTTP Event Collector (HEC) settings
const char* SPLUNK_HOST = "splunk.tipflips.com";
const int SPLUNK_PORT = 8088;
const char* SPLUNK_TOKEN = "f697778b-57ab-46b5-8207-c858760b76fc";

// Global objects
HttpClient http;

// Function declarations
void sendToSplunk(float pm2_5, float pm10, float temperature);

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected, 10000);
    
    // Initialize cloud connection
    Particle.connect();
    waitFor(Particle.connected, 30000);
    
    Serial.println("Device setup complete");
}

void loop() {
    if (!Particle.connected()) {
        Serial.println("Lost cloud connection. Reconnecting...");
        Particle.connect();
        return;
    }
    
    // Simulated Sensor Data (Replace with actual sensor readings)
    float pm2_5 = random(5, 100) / 1.0;
    float pm10 = random(10, 200) / 1.0;
    float temperature = random(20, 35) / 1.0;

    // Send data to Splunk
    sendToSplunk(pm2_5, pm10, temperature);

    delay(60000); // Wait for 60 seconds before next reading
}

void sendToSplunk(float pm2_5, float pm10, float temperature) {
    // Get device IP address
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
    
    // Create JSON payload manually with IP address
    char payload[512];
    snprintf(payload, sizeof(payload),
        "{\"event\":\"particle_sensor_data\",\"fields\":{\"pm2_5\":%0.2f,\"pm10\":%0.2f,\"temperature\":%0.2f,\"device_id\":\"%s\",\"ip_address\":\"%s\",\"timestamp\":%ld}}",
        pm2_5, pm10, temperature,
        System.deviceID().c_str(),
        ipStr.c_str(),
        Time.now()
    );
    
    // Log the IP address for debugging
    Serial.printlnf("Device IP: %s", ipStr.c_str());

    // Prepare HTTP request
    http_header_t headers[] = {
        {"Authorization", String("Splunk " + String(SPLUNK_TOKEN)).c_str()},
        {"Content-Type", "application/json"},
        {NULL, NULL}
    };

    http_request_t request;
    http_response_t response;

    request.hostname = SPLUNK_HOST;
    request.port = SPLUNK_PORT;
    request.path = "/services/collector";
    request.protocol = "https";

    // Print request details for debugging
    Serial.printlnf("Connecting to: https://%s:%d%s", request.hostname, request.port, request.path);

    // Send request
    Serial.println("Sending data to Splunk...");
    Serial.println(payload);
    
    http.post(request, response, headers);
    
    // Check response
    if (response.status == 200) {
        Serial.println("HTTP Request successful");
        Serial.print("Response status: ");
        Serial.println(response.status);
        Serial.print("Response: ");
        if (response.body != NULL) {
            Serial.println(response.body);
        }
    } else {
        Serial.print("HTTP Request failed with status: ");
        Serial.println(response.status);
    }
}