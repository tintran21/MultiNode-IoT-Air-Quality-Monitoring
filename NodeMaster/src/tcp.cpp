#include <WiFi.h>
#include <ArduinoJson.h>
#include "tcp.h"
#include "logic.h"

extern WiFiServer tcpServer;
extern WiFiClient tcpClient;
extern SensorState sensor;
extern unsigned long lastReceive;

void handleTCP() {
  if (!tcpClient || !tcpClient.connected()) {
    WiFiClient newClient = tcpServer.available();
    if (newClient) {
      tcpClient = newClient;
      Serial.println("TCP client connected");
    }
    return;
  }

  if (tcpClient.available()) {
    String line = tcpClient.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    StaticJsonDocument<256> doc;
    if (deserializeJson(doc, line) == DeserializationError::Ok) {
      sensor.temp  = doc["temp"];
      sensor.humi  = doc["humi"];
      sensor.ldr   = doc["ldr"];
      sensor.mq2   = doc["mq2"];
      sensor.mq135 = doc["mq135"];

      lastReceive = millis();

      Serial.printf("[TCP] T=%.1f H=%.1f LDR=%u MQ2=%u MQ135=%u\n",
                    sensor.temp, sensor.humi, sensor.ldr, sensor.mq2, sensor.mq135);

      evaluateLogic();
    }
  }
}
