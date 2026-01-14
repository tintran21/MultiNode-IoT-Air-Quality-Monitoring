#include "handle.h"

/* ================= GLOBAL DEFINITIONS ================= */
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
String uartBuffer = "";

bool ledLdrState = false;
bool mq2AlarmState = false;
bool mq135AlarmState = false;
bool fanState = false;

unsigned long lastBlink = 0;
const unsigned long BLINK_INTERVAL = 400;
bool blinkState = false;

/* ================= UART HANDLER ================= */
void handleUARTCommand(String cmd) {
  int sep = cmd.indexOf(':');
  if (sep == -1) return;

  String key = cmd.substring(0, sep);
  String value = cmd.substring(sep + 1);
  key.trim();
  value.trim();

  if (key == "FAN") {
    fanState = (value == "ON");
  }
  else if (key == "LED_LDR") {
    ledLdrState = (value == "ON");
  }
  else if (key == "MQ2_ALARM") {
    bool newState = (value == "ON");
    if (mq2AlarmState != newState) {
      mq2AlarmState = newState;
      updateLCDStatus();
    }
  }
  else if (key == "MQ135_ALARM") {
    bool newState = (value == "ON");
    if (mq135AlarmState != newState) {
      mq135AlarmState = newState;
      updateLCDStatus();
    }
  }
  else if (key == "TEMP") {
    lcd.setCursor(0, 1);
    lcd.print("T: ");
    lcd.print(value.toFloat(), 1);
    lcd.write((char)223);
  }
  else if (key == "HUMI") {
    lcd.setCursor(14, 1);
    lcd.print(value.toFloat(), 1);
    lcd.print("%");
  }
}

/* ================= LCD STATUS ================= */
void updateLCDStatus() {
  lcd.setCursor(5, 2);
  lcd.print(mq2AlarmState ? "DETECTED !!!    "
                          : "SAFE            ");

  lcd.setCursor(5, 3);
  lcd.print(mq135AlarmState ? "TOXIC / DANGER  "
                            : "GOOD            ");
}
