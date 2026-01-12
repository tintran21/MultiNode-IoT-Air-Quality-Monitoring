#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "web.h"
#include "logic.h"

extern WebServer webServer;
extern SensorState sensor;
extern ActState actState;
extern unsigned long lastReceive;
extern const unsigned long RECEIVE_TIMEOUT;
extern uint16_t MQ135_WARN;
extern float TEMP_THRESHOLD;

/* ================== API SENSOR ================== */
void apiSensor() {
  StaticJsonDocument<300> doc;
  doc["temp"] = sensor.temp;
  doc["humi"] = sensor.humi;
  doc["mq2"] = sensor.mq2;
  doc["mq135"] = sensor.mq135;

  doc["gas_detected"] = actState.mq2_alarm;
  doc["air_level"] = actState.mq135_alarm ? 2 : (sensor.mq135 > MQ135_WARN ? 1 : 0);

  bool sensorNodeOnline = (millis() - lastReceive < RECEIVE_TIMEOUT);
  doc["sensor_node_online"] = sensorNodeOnline;

  String sysStatus = "NORMAL";
  if (!sensorNodeOnline) sysStatus = "ERROR (LOST SIGNAL)";
  else if (actState.mq2_alarm || actState.mq135_alarm) sysStatus = "DANGER";
  else if (sensor.mq135 > MQ135_WARN || sensor.temp > TEMP_THRESHOLD) sysStatus = "WARNING";

  doc["system_status"] = sysStatus;

  String out;
  serializeJson(doc, out);
  webServer.send(200, "application/json", out);
}

/* ================== API HISTORY ================== */
void apiHistory() {
  StaticJsonDocument<2048> doc;
  JsonArray t = doc.createNestedArray("temperature");
  JsonArray a = doc.createNestedArray("air_quality");

  extern float tempHistory[];
  extern int airHistory[];
  extern uint8_t histIndex;

  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (histIndex + i) % HISTORY_SIZE;
    t.add(tempHistory[idx]);
    a.add(airHistory[idx]);
  }

  String out;
  serializeJson(doc, out);
  webServer.send(200, "application/json", out);
}

/* ================== WEB PAGE ================== */
String get_html_page() 
{
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Air Monitor Offline</title>
<style>
  :root {
    --bg: #1e1e2e;
    --card: #2a2a40;
    --text: #e0e0e0;
    --green: #2ecc71;
    --yellow: #f1c40f;
    --red: #e74c3c;
    --blue: #3498db;
    --gray: #888888;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { font-family: sans-serif; background: var(--bg); color: var(--text); padding: 20px; padding-bottom: 40px; }
  
  /* HEADER INFO */
  .header { display: flex; justify-content: space-between; align-items: center; background: var(--card); padding: 15px; border-radius: 10px; margin-bottom: 20px; border-bottom: 2px solid #444; }
  .status-row { display: flex; gap: 20px; font-size: 0.9rem; }
  .dot { height: 12px; width: 12px; background-color: #555; border-radius: 50%; display: inline-block; margin-right: 5px; }
  .dot.online { background-color: var(--green); box-shadow: 0 0 8px var(--green); }
  .dot.offline { background-color: var(--red); }
  
  .sys-status { font-weight: bold; padding: 4px 10px; border-radius: 4px; background: #444; }
  .sys-norm { color: var(--green); border: 1px solid var(--green); }
  .sys-warn { color: var(--yellow); border: 1px solid var(--yellow); }
  .sys-dang { color: var(--red); border: 1px solid var(--red); }

  /* SENSOR CARDS */
  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-bottom: 20px; }
  .card { background: var(--card); padding: 20px; border-radius: 12px; text-align: center; border-top: 3px solid transparent; }
  .val { font-size: 2rem; font-weight: bold; margin: 10px 0; }
  .lbl { font-size: 0.9rem; color: #aaa; }
  
  /* CHARTS */
  .chart-container { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-bottom: 40px; }
  .chart-box { background: var(--card); padding: 15px; border-radius: 12px; }
  canvas { width: 100%; height: 200px; background: #222; border-radius: 8px; border: 1px solid #444; }
  h4 { margin-bottom: 10px; color: var(--blue); }

  /* FOOTER SECTION */
  .footer { 
    text-align: center; 
    border-top: 1px solid #444; 
    padding-top: 20px; 
    color: var(--gray); 
    font-size: 0.9rem;
  }
  .footer p { margin-bottom: 8px; }
  .footer b { color: var(--text); }
  .social-links { display: flex; justify-content: center; gap: 15px; flex-wrap: wrap; }
  .social-links a { 
    color: var(--blue); 
    text-decoration: none; 
    font-weight: bold; 
    background: rgba(52, 152, 219, 0.1); 
    padding: 5px 10px; 
    border-radius: 5px; 
    transition: 0.3s;
  }
  .social-links a:hover { 
    background: var(--blue); 
    color: #fff; 
    box-shadow: 0 0 10px var(--blue);
  }

  /* Responsive */
  @media (max-width: 600px) { .header { flex-direction: column; gap: 10px; align-items: flex-start; } }
</style>
</head>
<body>

<div class="header">
  <h2>🏠 Home Air Monitor</h2>
  <div class="status-row">
    <div>
      Node Sensors: <span id="node-dot" class="dot"></span><span id="node-text">Checking...</span>
    </div>
    <div>
      System: <span id="sys-st" class="sys-status">--</span>
    </div>
  </div>
</div>

<div class="grid">
  <div class="card" style="border-color: var(--red);">
    <div class="lbl">Temperature</div>
    <div class="val"><span id="t">--</span><small>°C</small></div>
  </div>
  <div class="card" style="border-color: var(--blue);">
    <div class="lbl">Humidity</div>
    <div class="val"><span id="h">--</span><small>%</small></div>
  </div>
  <div class="card" style="border-color: var(--yellow);">
    <div class="lbl">MQ2 Gas</div>
    <div class="val" id="mq2">--</div>
    <div id="mq2-st" style="font-size:0.8rem">Safe</div>
  </div>
  <div class="card" style="border-color: var(--green);">
    <div class="lbl">MQ135 Air</div>
    <div class="val" id="mq135">--</div>
    <div id="mq135-st" style="font-size:0.8rem">Good</div>
  </div>
</div>

<div class="chart-container">
  <div class="chart-box">
    <h4>Temperature Trend</h4>
    <canvas id="cvsTemp" width="400" height="200"></canvas>
  </div>
  <div class="chart-box">
    <h4>Air Quality Trend (MQ135)</h4>
    <canvas id="cvsAir" width="400" height="200"></canvas>
  </div>
</div>

<div class="footer">
  <p>IoT Project Developed by <b>Tin Tran</b></p>
  <div class="social-links">
    <a href="https://github.com/tintran21" target="_blank">GitHub</a>
    <a href="https://www.linkedin.com/in/wthtintran/" target="_blank">LinkedIn</a>
    <a href="mailto:trantin2114@gmail.com">Email Contact</a>
  </div>
</div>

<script>
  // --- CORE LOGIC: DRAW GRAPH WITHOUT LIBRARY ---
  function drawGraph(canvasId, data, color, maxValFixed) {
    const cvs = document.getElementById(canvasId);
    if (!cvs) return;
    const ctx = cvs.getContext('2d');
    const w = cvs.width;
    const h = cvs.height;
    
    // Clear & Background
    ctx.clearRect(0, 0, w, h);
    ctx.fillStyle = "#252530";
    ctx.fillRect(0,0,w,h);
    
    if(data.length < 2) return;

    // Auto scale Y axis or use fixed
    let max = Math.max(...data);
    if(maxValFixed && max < maxValFixed) max = maxValFixed; 
    if(max == 0) max = 10; 

    const stepX = w / (data.length - 1);
    
    // Draw Grid Lines (Optional)
    ctx.strokeStyle = "#333";
    ctx.beginPath();
    ctx.moveTo(0, h/2); ctx.lineTo(w, h/2);
    ctx.stroke();

    // Draw Graph Line
    ctx.strokeStyle = color;
    ctx.lineWidth = 3;
    ctx.lineJoin = 'round';
    ctx.beginPath();

    for (let i = 0; i < data.length; i++) {
      let x = i * stepX;
      let y = h - (data[i] / max * h * 0.9) - 5; 
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();
    
    // Draw Area under line
    ctx.fillStyle = color + "33"; 
    ctx.lineTo(w, h);
    ctx.lineTo(0, h);
    ctx.fill();
  }

  // --- UPDATE DATA ---
  function update() {
    fetch('/api/sensor').then(r => r.json()).then(d => {
      document.getElementById('t').innerText = d.temp.toFixed(1);
      document.getElementById('h').innerText = d.humi.toFixed(1);
      document.getElementById('mq2').innerText = d.mq2;
      document.getElementById('mq135').innerText = d.mq135;
      
      const dot = document.getElementById('node-dot');
      const txt = document.getElementById('node-text');
      if (d.sensor_node_online) {
        dot.className = "dot online";
        txt.innerText = "Online";
        txt.style.color = "#2ecc71";
      } else {
        dot.className = "dot offline";
        txt.innerText = "Offline";
        txt.style.color = "#e74c3c";
      }

      const st = document.getElementById('sys-st');
      st.innerText = d.system_status;
      st.className = "sys-status"; 
      if(d.system_status.includes("NORMAL")) st.classList.add("sys-norm");
      else if(d.system_status.includes("WARNING")) st.classList.add("sys-warn");
      else st.classList.add("sys-dang");

      const m2 = document.getElementById('mq2-st');
      m2.innerText = d.gas_detected ? "⚠️ GAS DETECTED" : "Safe";
      m2.style.color = d.gas_detected ? "#e74c3c" : "#2ecc71";

      const m135 = document.getElementById('mq135-st');
      const airLabels = ["Fresh Air", "Warning", "Toxic"];
      const airColors = ["#2ecc71", "#f1c40f", "#e74c3c"];
      m135.innerText = airLabels[d.air_level];
      m135.style.color = airColors[d.air_level];
    });

    fetch('/api/history').then(r => r.json()).then(d => {
      drawGraph('cvsTemp', d.temperature, '#e74c3c', 40); 
      drawGraph('cvsAir', d.air_quality, '#2ecc71', 800);
    });
  }

  setInterval(update, 2000);
  update();
</script>
</body>
</html>
)rawliteral";
}
