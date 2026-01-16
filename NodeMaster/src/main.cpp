#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include "logic.h"
#include "tcp.h"
#include "web.h"

/*Config*/
const char* AP_SSID = "ESP32_MASTER_AP";  // you can change the SSID and PASS
const char* AP_PASS = "12345678";         // but you have to change familiar to node sensors
const uint16_t TCP_PORT = 8888;

/* UART */
#define UART_RX_PIN 16
#define UART_TX_PIN 17
#define UART_BAUD   115200

/* Thresholds */
float    TEMP_THRESHOLD   = 30.0;
uint16_t LDR_THRESHOLD    = 3000;
uint16_t MQ2_THRESHOLD    = 2500;
uint16_t MQ135_WARN       = 800;
uint16_t MQ135_DANGER     = 1050;


WebServer webServer(80);
WiFiServer tcpServer(TCP_PORT);
WiFiClient tcpClient;

SensorState sensor;
ActState actState, prevActState;

float tempHistory[HISTORY_SIZE];
int airHistory[HISTORY_SIZE];
uint8_t histIndex = 0;

/* timing */
unsigned long lastReceive = 0;
unsigned long RECEIVE_TIMEOUT = 10000;

void setup() 
{
  Serial.begin(115200);
  Serial2.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println(WiFi.softAPIP());

  tcpServer.begin();

  webServer.on("/", []() {
    webServer.send(200, "text/html", get_html_page());
  });
  webServer.on("/api/sensor", apiSensor);
  webServer.on("/api/history", apiHistory);
  webServer.begin();

  Serial.println("NodeMaster READY");
}

void loop() 
{
  handleTCP();
  webServer.handleClient();
}































































// #include <WiFi.h>
// #include <WebServer.h>
// #include <ArduinoJson.h>

// const char* AP_SSID = "ESP32_MASTER_AP";
// const char* AP_PASS = "12345678";

// const uint16_t TCP_PORT = 8888;


// /* UART to Arduino Nano */
// #define UART_RX_PIN 16
// #define UART_TX_PIN 17
// #define UART_BAUD   115200

// /* Thresholds */
// const float    TEMP_THRESHOLD   = 30.0;
// const uint16_t LDR_THRESHOLD    = 3000;
// const uint16_t MQ2_THRESHOLD    = 2500;
// const uint16_t MQ135_WARN       = 800;
// const uint16_t MQ135_DANGER     = 1050;

// WebServer webServer(80);
// WiFiServer tcpServer(TCP_PORT);
// WiFiClient tcpClient;

// struct SensorState {
//   float temp = 0;
//   float humi = 0;
//   uint16_t ldr = 0;
//   uint16_t mq2 = 0;
//   uint16_t mq135 = 0;
// } sensor;

// struct ActState {
//   bool led_ldr = false;
//   bool mq2_alarm = false;
//   bool mq135_alarm = false;
//   bool fan = false;
// } actState, prevActState;

// /* history for charts */
// #define HISTORY_SIZE 100
// float tempHistory[HISTORY_SIZE];
// int airHistory[HISTORY_SIZE];
// uint8_t histIndex = 0;

// /* timing */
// unsigned long lastReceive = 0;
// const unsigned long RECEIVE_TIMEOUT = 10000;


// void sendUART(const String& cmd);
// void updateUARTIfChanged();
// void evaluateLogic();
// void handleTCP();
// void apiSensor();
// void apiHistory();
// String get_html_page();

// void setup() {
//   Serial.begin(115200);
//   Serial2.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

//   WiFi.softAP(AP_SSID, AP_PASS);
//   Serial.print("AP IP: ");
//   Serial.println(WiFi.softAPIP());

//   tcpServer.begin();

//   webServer.on("/", []() {
//     webServer.send(200, "text/html", get_html_page());
//   });
//   webServer.on("/api/sensor", apiSensor);
//   webServer.on("/api/history", apiHistory);
//   webServer.begin();

//   Serial.println("NodeMaster READY");
// }

// void loop() {
//   handleTCP();
//   webServer.handleClient();
// }


// void sendUART(const String& cmd) {
//   Serial2.println(cmd);
//   Serial.print("[UART] ");
//   Serial.println(cmd);
// }

// void updateUARTIfChanged() {
//   if (actState.led_ldr != prevActState.led_ldr)
//     sendUART("LED_LDR:" + String(actState.led_ldr ? "ON" : "OFF"));

//   if (actState.mq2_alarm != prevActState.mq2_alarm)
//     sendUART("MQ2_ALARM:" + String(actState.mq2_alarm ? "ON" : "OFF"));

//   if (actState.mq135_alarm != prevActState.mq135_alarm)
//     sendUART("MQ135_ALARM:" + String(actState.mq135_alarm ? "ON" : "OFF"));

//   if (actState.fan != prevActState.fan)
//     sendUART("FAN:" + String(actState.fan ? "ON" : "OFF"));

//   prevActState = actState;
// }

// void evaluateLogic() {
//   actState = {}; // reset

//   /* Temperature -> Fan */
//   if (sensor.temp > TEMP_THRESHOLD)
//     actState.fan = true;

//   /* LDR -> LED */
//   if (sensor.ldr > LDR_THRESHOLD)
//     actState.led_ldr = true;

//   /* MQ PRIORITY: MQ2 FIRST */
//   if (sensor.mq2 >= MQ2_THRESHOLD) {
//     actState.mq2_alarm = true;
//     actState.fan = true;
//   }
//   else if (sensor.mq135 >= MQ135_DANGER) {
//     actState.mq135_alarm = true;
//     actState.fan = true;
//   }

//   // 1. Cập nhật các trạng thái điều khiển (LED, Còi, Relay)
//   updateUARTIfChanged();

//   // 2. [QUAN TRỌNG] Gửi giá trị Nhiệt độ & Độ ẩm cho Nano hiển thị LCD
//   // Chỉ gửi 1 số lẻ thập phân cho gọn
//   sendUART("TEMP:" + String(sensor.temp, 1));
//   sendUART("HUMI:" + String(sensor.humi, 1));

//   /* history update */
//   tempHistory[histIndex] = sensor.temp;
//   airHistory[histIndex]  = sensor.mq135;
//   histIndex = (histIndex + 1) % HISTORY_SIZE;
// }

// void handleTCP() {
//   if (!tcpClient || !tcpClient.connected()) {
//     WiFiClient newClient = tcpServer.available();
//     if (newClient) {
//       tcpClient = newClient;
//       Serial.println("TCP client connected");
//     }
//     return;
//   }

//   if (tcpClient.available()) {
//     String line = tcpClient.readStringUntil('\n');
//     line.trim();
//     if (line.length() == 0) return;

//     StaticJsonDocument<256> doc;
//     if (deserializeJson(doc, line) == DeserializationError::Ok) {
//       sensor.temp  = doc["temp"];
//       sensor.humi  = doc["humi"];
//       sensor.ldr   = doc["ldr"];
//       sensor.mq2   = doc["mq2"];
//       sensor.mq135 = doc["mq135"];

//       lastReceive = millis();

//       Serial.printf("[TCP] T=%.1f H=%.1f LDR=%u MQ2=%u MQ135=%u\n",
//                     sensor.temp, sensor.humi, sensor.ldr, sensor.mq2, sensor.mq135);

//       evaluateLogic();
//     }
//   }
// }

// void apiSensor() {
//   StaticJsonDocument<300> doc;
//   doc["temp"] = sensor.temp;
//   doc["humi"] = sensor.humi;
//   doc["mq2"] = sensor.mq2;
//   doc["mq135"] = sensor.mq135;
  
//   // Logic cảnh báo
//   doc["gas_detected"] = actState.mq2_alarm;
//   // 0: Good, 1: Warn, 2: Danger
//   doc["air_level"] = actState.mq135_alarm ? 2 : (sensor.mq135 > MQ135_WARN ? 1 : 0);
  
//   // Kiểm tra kết nối với Node Sensors
//   bool sensorNodeOnline = (millis() - lastReceive < RECEIVE_TIMEOUT);
//   doc["sensor_node_online"] = sensorNodeOnline;

//   // Đánh giá trạng thái toàn hệ thống (System Status)
//   String sysStatus = "NORMAL";
//   if (!sensorNodeOnline) sysStatus = "ERROR (LOST SIGNAL)";
//   else if (actState.mq2_alarm || actState.mq135_alarm) sysStatus = "DANGER";
//   else if (sensor.mq135 > MQ135_WARN || sensor.temp > TEMP_THRESHOLD) sysStatus = "WARNING";
  
//   doc["system_status"] = sysStatus;

//   String out;
//   serializeJson(doc, out);
//   webServer.send(200, "application/json", out);
// }

// void apiHistory() {
//   StaticJsonDocument<2048> doc; // Tăng buffer lên chút cho an toàn
//   JsonArray t = doc.createNestedArray("temperature");
//   JsonArray a = doc.createNestedArray("air_quality");

//   // LOGIC SỬA ĐỔI: Sắp xếp lại dữ liệu từ cũ nhất đến mới nhất
//   // Bắt đầu từ vị trí histIndex (là vị trí cũ nhất trong vòng lặp)
//   for (int i = 0; i < HISTORY_SIZE; i++) {
//     int idx = (histIndex + i) % HISTORY_SIZE; // Công thức "unroll" circular buffer
//     t.add(tempHistory[idx]);
//     a.add(airHistory[idx]);
//   }

//   String out;
//   serializeJson(doc, out);
//   webServer.send(200, "application/json", out);
// }


// String get_html_page() {
//   return R"rawliteral(
// <!DOCTYPE html>
// <html lang="en">
// <head>
// <meta charset="UTF-8">
// <meta name="viewport" content="width=device-width, initial-scale=1.0">
// <title>Air Monitor Offline</title>
// <style>
//   :root {
//     --bg: #1e1e2e;
//     --card: #2a2a40;
//     --text: #e0e0e0;
//     --green: #2ecc71;
//     --yellow: #f1c40f;
//     --red: #e74c3c;
//     --blue: #3498db;
//     --gray: #888888;
//   }
//   * { box-sizing: border-box; margin: 0; padding: 0; }
//   body { font-family: sans-serif; background: var(--bg); color: var(--text); padding: 20px; padding-bottom: 40px; }
  
//   /* HEADER INFO */
//   .header { display: flex; justify-content: space-between; align-items: center; background: var(--card); padding: 15px; border-radius: 10px; margin-bottom: 20px; border-bottom: 2px solid #444; }
//   .status-row { display: flex; gap: 20px; font-size: 0.9rem; }
//   .dot { height: 12px; width: 12px; background-color: #555; border-radius: 50%; display: inline-block; margin-right: 5px; }
//   .dot.online { background-color: var(--green); box-shadow: 0 0 8px var(--green); }
//   .dot.offline { background-color: var(--red); }
  
//   .sys-status { font-weight: bold; padding: 4px 10px; border-radius: 4px; background: #444; }
//   .sys-norm { color: var(--green); border: 1px solid var(--green); }
//   .sys-warn { color: var(--yellow); border: 1px solid var(--yellow); }
//   .sys-dang { color: var(--red); border: 1px solid var(--red); }

//   /* SENSOR CARDS */
//   .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-bottom: 20px; }
//   .card { background: var(--card); padding: 20px; border-radius: 12px; text-align: center; border-top: 3px solid transparent; }
//   .val { font-size: 2rem; font-weight: bold; margin: 10px 0; }
//   .lbl { font-size: 0.9rem; color: #aaa; }
  
//   /* CHARTS */
//   .chart-container { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-bottom: 40px; }
//   .chart-box { background: var(--card); padding: 15px; border-radius: 12px; }
//   canvas { width: 100%; height: 200px; background: #222; border-radius: 8px; border: 1px solid #444; }
//   h4 { margin-bottom: 10px; color: var(--blue); }

//   /* FOOTER SECTION */
//   .footer { 
//     text-align: center; 
//     border-top: 1px solid #444; 
//     padding-top: 20px; 
//     color: var(--gray); 
//     font-size: 0.9rem;
//   }
//   .footer p { margin-bottom: 8px; }
//   .footer b { color: var(--text); }
//   .social-links { display: flex; justify-content: center; gap: 15px; flex-wrap: wrap; }
//   .social-links a { 
//     color: var(--blue); 
//     text-decoration: none; 
//     font-weight: bold; 
//     background: rgba(52, 152, 219, 0.1); 
//     padding: 5px 10px; 
//     border-radius: 5px; 
//     transition: 0.3s;
//   }
//   .social-links a:hover { 
//     background: var(--blue); 
//     color: #fff; 
//     box-shadow: 0 0 10px var(--blue);
//   }

//   /* Responsive */
//   @media (max-width: 600px) { .header { flex-direction: column; gap: 10px; align-items: flex-start; } }
// </style>
// </head>
// <body>

// <div class="header">
//   <h2>🏠 Home Air Monitor</h2>
//   <div class="status-row">
//     <div>
//       Node Sensors: <span id="node-dot" class="dot"></span><span id="node-text">Checking...</span>
//     </div>
//     <div>
//       System: <span id="sys-st" class="sys-status">--</span>
//     </div>
//   </div>
// </div>

// <div class="grid">
//   <div class="card" style="border-color: var(--red);">
//     <div class="lbl">Temperature</div>
//     <div class="val"><span id="t">--</span><small>°C</small></div>
//   </div>
//   <div class="card" style="border-color: var(--blue);">
//     <div class="lbl">Humidity</div>
//     <div class="val"><span id="h">--</span><small>%</small></div>
//   </div>
//   <div class="card" style="border-color: var(--yellow);">
//     <div class="lbl">MQ2 Gas</div>
//     <div class="val" id="mq2">--</div>
//     <div id="mq2-st" style="font-size:0.8rem">Safe</div>
//   </div>
//   <div class="card" style="border-color: var(--green);">
//     <div class="lbl">MQ135 Air</div>
//     <div class="val" id="mq135">--</div>
//     <div id="mq135-st" style="font-size:0.8rem">Good</div>
//   </div>
// </div>

// <div class="chart-container">
//   <div class="chart-box">
//     <h4>Temperature Trend</h4>
//     <canvas id="cvsTemp" width="400" height="200"></canvas>
//   </div>
//   <div class="chart-box">
//     <h4>Air Quality Trend (MQ135)</h4>
//     <canvas id="cvsAir" width="400" height="200"></canvas>
//   </div>
// </div>

// <div class="footer">
//   <p>IoT Project Developed by <b>Tin Tran</b></p>
//   <div class="social-links">
//     <a href="https://github.com/tintran21" target="_blank">GitHub</a>
//     <a href="https://www.linkedin.com/in/wthtintran/" target="_blank">LinkedIn</a>
//     <a href="mailto:trantin2114@gmail.com">Email Contact</a>
//   </div>
// </div>

// <script>
//   // --- CORE LOGIC: DRAW GRAPH WITHOUT LIBRARY ---
//   function drawGraph(canvasId, data, color, maxValFixed) {
//     const cvs = document.getElementById(canvasId);
//     if (!cvs) return;
//     const ctx = cvs.getContext('2d');
//     const w = cvs.width;
//     const h = cvs.height;
    
//     // Clear & Background
//     ctx.clearRect(0, 0, w, h);
//     ctx.fillStyle = "#252530";
//     ctx.fillRect(0,0,w,h);
    
//     if(data.length < 2) return;

//     // Auto scale Y axis or use fixed
//     let max = Math.max(...data);
//     if(maxValFixed && max < maxValFixed) max = maxValFixed; 
//     if(max == 0) max = 10; 

//     const stepX = w / (data.length - 1);
    
//     // Draw Grid Lines (Optional)
//     ctx.strokeStyle = "#333";
//     ctx.beginPath();
//     ctx.moveTo(0, h/2); ctx.lineTo(w, h/2);
//     ctx.stroke();

//     // Draw Graph Line
//     ctx.strokeStyle = color;
//     ctx.lineWidth = 3;
//     ctx.lineJoin = 'round';
//     ctx.beginPath();

//     for (let i = 0; i < data.length; i++) {
//       let x = i * stepX;
//       let y = h - (data[i] / max * h * 0.9) - 5; 
//       if (i === 0) ctx.moveTo(x, y);
//       else ctx.lineTo(x, y);
//     }
//     ctx.stroke();
    
//     // Draw Area under line
//     ctx.fillStyle = color + "33"; 
//     ctx.lineTo(w, h);
//     ctx.lineTo(0, h);
//     ctx.fill();
//   }

//   // --- UPDATE DATA ---
//   function update() {
//     fetch('/api/sensor').then(r => r.json()).then(d => {
//       document.getElementById('t').innerText = d.temp.toFixed(1);
//       document.getElementById('h').innerText = d.humi.toFixed(1);
//       document.getElementById('mq2').innerText = d.mq2;
//       document.getElementById('mq135').innerText = d.mq135;
      
//       const dot = document.getElementById('node-dot');
//       const txt = document.getElementById('node-text');
//       if (d.sensor_node_online) {
//         dot.className = "dot online";
//         txt.innerText = "Online";
//         txt.style.color = "#2ecc71";
//       } else {
//         dot.className = "dot offline";
//         txt.innerText = "Offline";
//         txt.style.color = "#e74c3c";
//       }

//       const st = document.getElementById('sys-st');
//       st.innerText = d.system_status;
//       st.className = "sys-status"; 
//       if(d.system_status.includes("NORMAL")) st.classList.add("sys-norm");
//       else if(d.system_status.includes("WARNING")) st.classList.add("sys-warn");
//       else st.classList.add("sys-dang");

//       const m2 = document.getElementById('mq2-st');
//       m2.innerText = d.gas_detected ? "⚠️ GAS DETECTED" : "Safe";
//       m2.style.color = d.gas_detected ? "#e74c3c" : "#2ecc71";

//       const m135 = document.getElementById('mq135-st');
//       const airLabels = ["Fresh Air", "Warning", "Toxic"];
//       const airColors = ["#2ecc71", "#f1c40f", "#e74c3c"];
//       m135.innerText = airLabels[d.air_level];
//       m135.style.color = airColors[d.air_level];
//     });

//     fetch('/api/history').then(r => r.json()).then(d => {
//       drawGraph('cvsTemp', d.temperature, '#e74c3c', 40); 
//       drawGraph('cvsAir', d.air_quality, '#2ecc71', 800);
//     });
//   }

//   setInterval(update, 2000);
//   update();
// </script>
// </body>
// </html>
// )rawliteral";
// }


