#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define LDR_PIN         0
#define MQ2_PIN         1
#define MQ135_PIN       4
#define DHT_PIN         5
#define DHT_TYPE        DHT22

extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

extern const char* SERVER_IP;
extern const uint16_t SERVER_PORT;

#define SENSOR_READ_INTERVAL    2000
#define RECONNECT_INTERVAL      5000

typedef struct {
    float temperature;
    float humidity;
    uint16_t ldr;
    uint16_t mq2;
    uint16_t mq135;
} sensor_data_t;

extern DHT dht;
extern WiFiClient tcpClient;
extern sensor_data_t sensorData;

#endif
