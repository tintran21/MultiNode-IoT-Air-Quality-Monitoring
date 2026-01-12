#include "logic.h"

/* extern states */
extern SensorState sensor;
extern ActState actState, prevActState;
extern float tempHistory[];
extern int airHistory[];
extern uint8_t histIndex;

extern const float TEMP_THRESHOLD;
extern const uint16_t LDR_THRESHOLD;
extern const uint16_t MQ2_THRESHOLD;
extern const uint16_t MQ135_DANGER;

/* UART helpers */
void sendUART(const String& cmd) {
  Serial2.println(cmd);
  Serial.print("[UART] ");
  Serial.println(cmd);
}

void updateUARTIfChanged() {
  if (actState.led_ldr != prevActState.led_ldr)
    sendUART("LED_LDR:" + String(actState.led_ldr ? "ON" : "OFF"));

  if (actState.mq2_alarm != prevActState.mq2_alarm)
    sendUART("MQ2_ALARM:" + String(actState.mq2_alarm ? "ON" : "OFF"));

  if (actState.mq135_alarm != prevActState.mq135_alarm)
    sendUART("MQ135_ALARM:" + String(actState.mq135_alarm ? "ON" : "OFF"));

  if (actState.fan != prevActState.fan)
    sendUART("FAN:" + String(actState.fan ? "ON" : "OFF"));

  prevActState = actState;
}

/* =====================================================
 *                  LOGIC CORE
 * ===================================================== */
void evaluateLogic() {
  actState = {};

  if (sensor.temp > TEMP_THRESHOLD)
    actState.fan = true;

  if (sensor.ldr > LDR_THRESHOLD)
    actState.led_ldr = true;

  if (sensor.mq2 >= MQ2_THRESHOLD) {
    actState.mq2_alarm = true;
    actState.fan = true;
  }
  else if (sensor.mq135 >= MQ135_DANGER) {
    actState.mq135_alarm = true;
    actState.fan = true;
  }

  updateUARTIfChanged();

  sendUART("TEMP:" + String(sensor.temp, 1));
  sendUART("HUMI:" + String(sensor.humi, 1));

  tempHistory[histIndex] = sensor.temp;
  airHistory[histIndex]  = sensor.mq135;
  histIndex = (histIndex + 1) % HISTORY_SIZE;
}
