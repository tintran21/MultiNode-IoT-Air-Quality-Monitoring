
#include <Arduino.h>
#include <Wire.h>
#include "handle.h"

void setup() {
  Serial.begin(115200);

  pinMode(LED_LDR_PIN, OUTPUT);
  pinMode(LED_MQ2_PIN, OUTPUT);
  pinMode(BUZZER_MQ2_PIN, OUTPUT);
  pinMode(LED_MQ135_PIN, OUTPUT);
  pinMode(BUZZER_MQ135_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(LED_LDR_PIN, LOW);
  digitalWrite(LED_MQ2_PIN, LOW);
  digitalWrite(BUZZER_MQ2_PIN, LOW);
  digitalWrite(LED_MQ135_PIN, LOW);
  digitalWrite(BUZZER_MQ135_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("   AIR MONITORING   ");

  lcd.setCursor(0, 1);
  lcd.print("T: --.-");
  lcd.write(223);
  lcd.print(" | H: --.-%");

  lcd.setCursor(0, 2);
  lcd.print("GAS: Checking...    ");

  lcd.setCursor(0, 3);
  lcd.print("AIR: Checking...    ");

  Serial.println("Nano Ready!");
}

void loop() {
  while (Serial.available()) 
  {
    char c = Serial.read();
    if (c == '\n') {
      uartBuffer.trim();
      if (uartBuffer.length()) handleUARTCommand(uartBuffer);
      uartBuffer = "";
    } else {
      uartBuffer += c;
    }
  }

  unsigned long now = millis();
  if (now - lastBlink >= BLINK_INTERVAL) 
  {
    lastBlink = now;
    blinkState = !blinkState;

    digitalWrite(LED_MQ2_PIN, mq2AlarmState && blinkState);
    digitalWrite(BUZZER_MQ2_PIN, mq2AlarmState && blinkState);

    digitalWrite(LED_MQ135_PIN, mq135AlarmState && blinkState);
    digitalWrite(BUZZER_MQ135_PIN, mq135AlarmState && blinkState && !mq2AlarmState);
  }

  digitalWrite(LED_LDR_PIN, ledLdrState);
  digitalWrite(RELAY_PIN, fanState ? LOW : HIGH);
}




















// #include <Arduino.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// /* =====================================================
//  * PIN CONFIGURATION
//  * ===================================================== */
// // Output Pins
// #define LED_LDR_PIN       5    
// #define LED_MQ2_PIN       6
// #define BUZZER_MQ2_PIN    7
// #define LED_MQ135_PIN     8
// #define BUZZER_MQ135_PIN  9
// #define RELAY_PIN         10   // Relay Active LOW (Mức thấp là Bật)

// // LCD Config
// #define LCD_ADDR          0x27 // Hoặc 0x3F tùy module
// #define LCD_COLS          20
// #define LCD_ROWS          4

// /* =====================================================
//  * GLOBAL OBJECTS
//  * ===================================================== */
// LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
// String uartBuffer = ""; 

// /* =====================================================
//  * STATE VARIABLES
//  * ===================================================== */
// bool ledLdrState = false;
// bool mq2AlarmState = false;
// bool mq135AlarmState = false;
// bool fanState = false;

// // Variables để lưu giá trị hiển thị (tránh in lại nếu ko đổi)
// float lastTemp = 0.0;
// float lastHumi = 0.0;

// /* =====================================================
//  * TIMING (BLINKING)
//  * ===================================================== */
// unsigned long lastBlink = 0;
// const unsigned long BLINK_INTERVAL = 400; // 200ms chớp tắt nhanh cảnh báo
// bool blinkState = false;

// /* =====================================================
//  * FUNCTION PROTOTYPES
//  * ===================================================== */
// void handleUARTCommand(String cmd);
// void updateLCDStatus(); // Cập nhật dòng chữ trạng thái (Safe/Danger)

// /* =====================================================
//  * SETUP
//  * ===================================================== */
// void setup() {
//   Serial.begin(115200); // Phải khớp với NodeMaster

//   // Cấu hình chân Output
//   pinMode(LED_LDR_PIN, OUTPUT);
//   pinMode(LED_MQ2_PIN, OUTPUT);
//   pinMode(BUZZER_MQ2_PIN, OUTPUT);
//   pinMode(LED_MQ135_PIN, OUTPUT);
//   pinMode(BUZZER_MQ135_PIN, OUTPUT);
//   pinMode(RELAY_PIN, OUTPUT);

//   // Trạng thái ban đầu (Tắt hết)
//   digitalWrite(LED_LDR_PIN, LOW);
//   digitalWrite(LED_MQ2_PIN, LOW);
//   digitalWrite(BUZZER_MQ2_PIN, LOW);
//   digitalWrite(LED_MQ135_PIN, LOW);
//   digitalWrite(BUZZER_MQ135_PIN, LOW);
  
//   // Relay Active LOW -> Khởi động để HIGH để TẮT quạt ngay lập tức
//   digitalWrite(RELAY_PIN, HIGH); 

//   // Khởi động LCD
//   lcd.init();
//   lcd.backlight();
//   lcd.clear();
//   delay(100);

//   // Hàng 0: Tiêu đề
//   lcd.setCursor(0, 0);
//   lcd.print("   AIR MONITORING   ");

//   // Hàng 1: Nhiệt độ - Độ ẩm (Vẽ khuôn mẫu trước)
//   // T: --.-* | H: --.-%
//   lcd.setCursor(0, 1);
//   lcd.print("T: --.-");
//   lcd.write(223); // Ký tự độ C (dấu chấm tròn nhỏ) trong bảng mã ASCII LCD
//   lcd.print(" | H: --.-%");

//   // Hàng 2: Gas
//   lcd.setCursor(0, 2);
//   lcd.print("GAS: Checking...    ");

//   // Hàng 3: Air Quality
//   lcd.setCursor(0, 3);
//   lcd.print("AIR: Checking...    ");
  
//   Serial.println("Nano Ready!");
// }

// /* =====================================================
//  * LOOP
//  * ===================================================== */
// void loop() {
//   // 1. Đọc UART không chặn (Non-blocking UART Read)
//   while (Serial.available()) {
//     char c = (char)Serial.read();
//     if (c == '\n') { // Gặp ký tự xuống dòng là kết thúc lệnh
//       uartBuffer.trim(); // Xóa \r hoặc khoảng trắng thừa
//       if (uartBuffer.length() > 0) {
//         handleUARTCommand(uartBuffer);
//       }
//       uartBuffer = ""; // Xóa buffer để nhận lệnh mới
//     } else {
//       uartBuffer += c;
//     }
//   }

//   // 2. Xử lý hiệu ứng Chớp tắt (Blinking) cho Alarm
//   unsigned long now = millis();
//   if (now - lastBlink >= BLINK_INTERVAL) {
//     lastBlink = now;
//     blinkState = !blinkState;

//     // Alarm MQ2 (Gas)
//     if (mq2AlarmState) {
//       digitalWrite(LED_MQ2_PIN, blinkState ? HIGH : LOW);
//       digitalWrite(BUZZER_MQ2_PIN, blinkState ? HIGH : LOW);
//     } else {
//       digitalWrite(LED_MQ2_PIN, LOW);
//       digitalWrite(BUZZER_MQ2_PIN, LOW);
//     }

//     // Alarm MQ135 (Air Quality)
//     // Nếu MQ2 đang kêu thì ưu tiên MQ2, MQ135 chỉ nháy đèn, không kêu để đỡ ồn
//     if (mq135AlarmState) {
//       digitalWrite(LED_MQ135_PIN, blinkState ? HIGH : LOW);
//       if (!mq2AlarmState) { 
//          digitalWrite(BUZZER_MQ135_PIN, blinkState ? HIGH : LOW);
//       }
//     } else {
//       digitalWrite(LED_MQ135_PIN, LOW);
//       digitalWrite(BUZZER_MQ135_PIN, LOW);
//     }
//   }

//   // 3. Điều khiển LED LDR (Sáng đứng yên)
//   digitalWrite(LED_LDR_PIN, ledLdrState ? HIGH : LOW);

//   // 4. Điều khiển Relay (Logic ngược: LOW là BẬT)
//   if (fanState) {
//     digitalWrite(RELAY_PIN, LOW);  // Bật quạt
//   } else {
//     digitalWrite(RELAY_PIN, HIGH); // Tắt quạt
//   }
// }

// /* =====================================================
//  * HANDLE UART COMMANDS
//  * ===================================================== */
// void handleUARTCommand(String cmd) {
//   // Cú pháp: KEY:VALUE
//   int sep = cmd.indexOf(':');
//   if (sep == -1) return;

//   String key = cmd.substring(0, sep);
//   String value = cmd.substring(sep + 1);
  
//   // Chuẩn hóa
//   key.trim();
//   value.trim();

//   // --- PARSE DATA ---

//   if (key == "FAN") {
//     // Nhận "ON" -> fanState = true. Logic đảo mức Pin xử lý ở loop()
//     fanState = (value == "ON");
//   }
//   else if (key == "LED_LDR") {
//     ledLdrState = (value == "ON");
//   }
//   else if (key == "MQ2_ALARM") {
//     bool newState = (value == "ON");
//     if (mq2AlarmState != newState) { // Chỉ cập nhật LCD khi trạng thái thay đổi
//       mq2AlarmState = newState;
//       updateLCDStatus();
//     }
//   }
//   else if (key == "MQ135_ALARM") {
//     bool newState = (value == "ON");
//     if (mq135AlarmState != newState) {
//       mq135AlarmState = newState;
//       updateLCDStatus();
//     }
//   }
//   // --- CẬP NHẬT NHIỆT ĐỘ (Gọn nhẹ, in đè trực tiếp) ---
//   else if (key == "TEMP") {
//     lcd.setCursor(0, 1);
//     lcd.print("T: ");
//     lcd.print(value.toFloat(), 1); // In số (VD: 30.5)
//     lcd.write((char)223);          // In dấu độ ngay sau số
//   }
  
//   // --- CẬP NHẬT ĐỘ ẨM ---
//   else if (key == "HUMI") {
//     lcd.setCursor(14, 1); 
//     lcd.print(value.toFloat(), 1); // In số (VD: 70.5)     
//     lcd.print("%");                // In dấu % ngay sau số
//   }
// }

// /* =====================================================
//  * UPDATE LCD STATUS TEXT
//  * ===================================================== */
// void updateLCDStatus() {
//   // Cập nhật dòng 2 (MQ2)
//   lcd.setCursor(5, 2); // Vị trí sau "GAS: "
//   if (mq2AlarmState) {
//     lcd.print("DETECTED !!!    ");
//   } else {
//     lcd.print("SAFE            ");
//   }

//   // Cập nhật dòng 3 (MQ135)
//   lcd.setCursor(5, 3); // Vị trí sau "AIR: "
//   if (mq135AlarmState) {
//     lcd.print("TOXIC / DANGER  ");
//   } else {
//     lcd.print("GOOD            ");
//   }
// }