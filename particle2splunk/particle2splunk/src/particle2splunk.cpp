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
    
    if (Particle.connected()) {
        Serial.println("Device connected to Particle Cloud");
        Serial.print("Device ID: ");
        Serial.println(System.deviceID());
    } else {
        Serial.println("Failed to connect to Particle Cloud");
    }
    
    Serial.println("Device setup complete");
}

void loop() {
    if (!Particle.connected()) {
        Serial.println("Lost cloud connection. Reconnecting...");
        Particle.connect();
        return;
    }
    
    Serial.println("\n--- Starting new sensor reading ---");
    Serial.print("Device ID: "); Serial.println(System.deviceID());
    Serial.print("Current time: "); Serial.println(Time.timeStr());
    
    // Simulated Sensor Data (Replace with actual sensor readings)
    float pm2_5 = random(5, 100) / 1.0;
    float pm10 = random(10, 200) / 1.0;
    float temperature = random(20, 35) / 1.0;

    Serial.println("Generated sensor readings:");
    Serial.print("PM2.5: "); Serial.println(pm2_5);
    Serial.print("PM10: "); Serial.println(pm10);
    Serial.print("Temperature: "); Serial.println(temperature);

    // Create JSON data for Particle events with device info
    char data[256];
    snprintf(data, sizeof(data),
        "{\"device\":{\"id\":\"%s\",\"name\":\"sky_magnetic\",\"product_id\":36723},\"data\":{\"pm2_5\":%0.2f,\"pm10\":%0.2f,\"temperature\":%0.2f},\"timestamp\":%ld}",
        System.deviceID().c_str(),
        pm2_5, pm10, temperature,
        Time.now()
    );

    // First, try to publish to Particle console
    PublishFlags flags = PRIVATE | WITH_ACK;
    const char* EVENT_NAME = "sky_magnetic/sensor_data";
    bool published = false;
    int retries = 0;
    const int MAX_RETRIES = 3;
    
    while (!published && retries < MAX_RETRIES) {
        if (Particle.connected()) {
            published = Particle.publish(EVENT_NAME, data, 60, flags);
            if (published) {
                Serial.printlnf("✓ Published to Particle console (attempt %d/%d)", retries + 1, MAX_RETRIES);
                break;
            }
        }
        
        retries++;
        if (!published && retries < MAX_RETRIES) {
            Serial.printlnf("✗ Publish attempt %d failed, retrying...", retries);
            delay(1000); // Wait a second before retry
            if (!Particle.connected()) {
                Serial.println("Reconnecting to Particle cloud...");
                Particle.connect();
                waitFor(Particle.connected, 10000);
            }
        }
    }
    
    if (!published) {
        Serial.printlnf("✗ Failed to publish after %d attempts", MAX_RETRIES);
    }

    // Only proceed with Splunk if Particle publish was successful
    if (published) {
        sendToSplunk(pm2_5, pm10, temperature);
    } else {
        Serial.println("Skipping Splunk send due to Particle publish failure");
    }

    delay(60000); // Wait for 60 seconds before next reading as per requirements
}

void sendToSplunk(float pm2_5, float pm10, float temperature) {
    // Create JSON payload manually
    char payload[256];
    snprintf(payload, sizeof(payload),
        "{\"event\":\"particle_sensor_data\",\"fields\":{\"pm2_5\":%0.2f,\"pm10\":%0.2f,\"temperature\":%0.2f,\"device_id\":\"%s\",\"timestamp\":%ld}}",
        pm2_5, pm10, temperature,
        System.deviceID().c_str(),
        Time.now()
    );

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
    
    Serial.println("\n=== Starting HTTP Request ===");
    Serial.print("Time: "); Serial.println(Time.timeStr());
    Serial.print("URL: http://"); Serial.print(request.hostname);
    Serial.print(":"); Serial.print(request.port); 
    Serial.println(request.path);
    Serial.println("Payload:");
    Serial.println(payload);

    // Send request
    Serial.println("Sending request...");
    http.post(request, response, headers);
    
    if (response.status == 200) {
        Serial.println("Request successful!");
        if (response.body != NULL) {
            Serial.print("Response: ");
            Serial.println(response.body);
        }
    } else {
        Serial.println("Request failed!");
        Serial.print("Status code: "); Serial.println(response.status);
        if (response.body != NULL) {
            Serial.print("Error: ");
            Serial.println(response.body);
        }
    }
    Serial.println("=== End of Request ===\n");
}