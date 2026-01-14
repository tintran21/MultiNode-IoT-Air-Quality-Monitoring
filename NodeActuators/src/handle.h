#ifndef HANDLE_H
#define HANDLE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

/* ================= PIN CONFIG ================= */
#define LED_LDR_PIN       5    
#define LED_MQ2_PIN       6
#define BUZZER_MQ2_PIN    7
#define LED_MQ135_PIN     8
#define BUZZER_MQ135_PIN  9
#define RELAY_PIN         10

#define LCD_ADDR          0x27
#define LCD_COLS          20
#define LCD_ROWS          4

/* ================= GLOBALS ================= */
extern LiquidCrystal_I2C lcd;
extern String uartBuffer;

extern bool ledLdrState;
extern bool mq2AlarmState;
extern bool mq135AlarmState;
extern bool fanState;

extern unsigned long lastBlink;
extern const unsigned long BLINK_INTERVAL;
extern bool blinkState;

/* ================= FUNCTIONS ================= */
void handleUARTCommand(String cmd);
void updateLCDStatus();

#endif
