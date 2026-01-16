#pragma once
#include <Arduino.h>

struct SensorState 
{
  float temp = 0;
  float humi = 0;
  uint16_t ldr = 0;
  uint16_t mq2 = 0;
  uint16_t mq135 = 0;
};

struct ActState 
{
  bool led_ldr = false;
  bool mq2_alarm = false;
  bool mq135_alarm = false;
  bool fan = false;
};

#define HISTORY_SIZE 100

void evaluateLogic();
void sendUART(const String& cmd);
void updateUARTIfChanged();
