#include <WiFi.h>
#include <WebServer.h>

// === WiFi AP ===
const char* ssid = "ESP32-CAR";
const char* password = "12345678";
WebServer server(80);

// === Motor Pins ===
// Motor A (Left)
#define IN1 23
#define IN2 22
#define ENA 21   // PWM

// Motor B (Right)
#define IN3 19
#define IN4 18
#define ENB 5    // PWM

int speedValue = 180;   // default PWM

// ====== MOTOR FUNCTIONS ======
void setSpeed(int sp) {
  speedValue = constrain(sp, 0, 255);
  ledcWrite(0, speedValue);
  ledcWrite(1, speedValue);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ====== HTML PAGE ======
String htmlPage() {
  return R"=====(
<!DOCTYPE html>
<html>
<head>
<style>
body {
  background: #f4f4f4;
  text-align: center;
  font-family: Arial;
}

/* Wrapper to keep joystick and slider side by side */
#controls {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 40px;
  margin-top: 20px;
}

/* Big Joystick Area */
#joyArea {
  width: 300px;
  height: 300px;
  background: #ddd;
  border-radius: 50%;
  position: relative;
  touch-action: none;
}

/* Movable stick */
#stick {
  width: 90px;
  height: 90px;
  background: #222;
  border-radius: 50%;
  position: absolute;
  top: 105px;
  left: 105px;
  touch-action: none;
}

/* Speed slider box */
#speedBox {
  display: flex;
  flex-direction: column;
  align-items: center;
}

/* Vertical slider */
#speedSlider {
  width: 220px;              /* long, but rotated */
  transform: rotate(-90deg); /* make it vertical */
  -webkit-appearance: none;
  margin: 30px 0;
}

#speedText {
  font-size: 18px;
  margin-top: 10px;
}
</style>
</head>

<body>
<h2>ESP32 Car Controller</h2>

<div id="controls">
  <!-- Joystick -->
  <div id="joyArea">
    <div id="stick"></div>
  </div>

  <!-- Vertical speed slider -->
  <div id="speedBox">
    <p><b>Speed (PWM)</b></p>
    <input type="range" id="speedSlider" min="0" max="255" value="180"
           oninput="updateSpeed(this.value)">
    <div id="speedText">Speed: 180</div>
  </div>
</div>

<script>
let joyArea = document.getElementById("joyArea");
let stick = document.getElementById("stick");
let areaSize = 300;
let stickSize = 90;
let center = (areaSize - stickSize) / 2;
let maxMove = center;

// --- current speed value (same as default slider and ESP32) ---
let currentSpeed = 180;

/* -------- STOP motor -------- */
function stopCar() {
  fetch("/cmd?d=stop");
}

/* -------- SEND Direction Commands -------- */
function sendDirection(dx, dy) {
  let cmd = "stop";

  if (dy < -0.4) cmd = "forward";
  else if (dy > 0.4) cmd = "back";
  else if (dx < -0.4) cmd = "left";
  else if (dx > 0.4) cmd = "right";

  fetch("/cmd?d=" + cmd);
}

/* -------- HANDLE JOYSTICK MOVING -------- */
function handleMove(e) {
  let rect = joyArea.getBoundingClientRect();
  let x = (e.touches ? e.touches[0].clientX : e.clientX) - rect.left;
  let y = (e.touches ? e.touches[0].clientY : e.clientY) - rect.top;

  let dx = x - areaSize / 2;
  let dy = y - areaSize / 2;

  let dist = Math.sqrt(dx*dx + dy*dy);
  if (dist > maxMove) {
    dx *= maxMove / dist;
    dy *= maxMove / dist;
  }

  stick.style.left = (center + dx) + "px";
  stick.style.top = (center + dy) + "px";

  sendDirection(dx / maxMove, dy / maxMove);
}

/* -------- RESET ON RELEASE -------- */
function resetStick() {
  stick.style.left = center + "px";
  stick.style.top = center + "px";
  stopCar();
}

/* -------- SPEED SLIDER HANDLER -------- */
function updateSpeed(val) {
  currentSpeed = parseInt(val);
  document.getElementById("speedText").innerHTML = "Speed: " + currentSpeed;
  fetch("/speed?value=" + currentSpeed);
}

/* -------- EVENTS -------- */
joyArea.addEventListener("mousedown", e => handleMove(e));
joyArea.addEventListener("mousemove", e => {
  if (e.buttons) handleMove(e);
});
document.addEventListener("mouseup", resetStick);

joyArea.addEventListener("touchstart", handleMove);
joyArea.addEventListener("touchmove", handleMove);
joyArea.addEventListener("touchend", resetStick);

// Set initial speed on page load (optional)
updateSpeed(currentSpeed);
</script>

</body>
</html>


)=====";
}

// ====== SERVER HANDLERS ======
void handleRoot() { server.send(200, "text/html", htmlPage()); }

void handleCMD() {
  String d = server.arg("d");
  Serial.print("CMD: ");
  Serial.println(d);

  if (d == "forward") forward();
  else if (d == "back") backward();
  else if (d == "left") left();
  else if (d == "right") right();
  else stopCar();

  server.send(200, "text/plain", "OK");
}

void handleSpeed() {
  int sp = server.arg("value").toInt();
  Serial.print("Speed: ");
  Serial.println(sp);
  setSpeed(sp);
  server.send(200, "text/plain", "Speed updated");
}

void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM setup
  ledcSetup(0, 5000, 8); // channel 0, 5kHz, 8-bit
  ledcAttachPin(ENA, 0);

  ledcSetup(1, 5000, 8);
  ledcAttachPin(ENB, 1);

  // Start AP
  WiFi.softAP(ssid, password);
  Serial.println("AP Started");

  // Routes
  server.on("/", handleRoot);
  server.on("/cmd", handleCMD);
  server.on("/speed", handleSpeed);

  server.begin();
}

void loop() {
  server.handleClient();
}
