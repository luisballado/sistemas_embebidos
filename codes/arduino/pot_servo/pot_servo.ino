
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "driver/ledc.h"  // Explicit include for LEDC functions

// Replace with your network credentials
const char* ssid     = "INFINITUME880";
const char* password = "Rp7Lg5Ee5c";


// Servo configuration
const int servoPin = 25;      // Servo control pin
Servo myServo;
int servoAngle = 75;          // Neutral steering position

// Motor configuration (using an H-bridge driver like L298N)
const int motorIn1 = 23;      // Motor direction pin 1
const int motorIn2 = 32;      // Motor direction pin 2
const int motorEn  = 33;      // Motor enable (PWM speed control)

// PWM configuration for motor speed control
const int pwmChannel   = 0;
const int pwmFreq      = 30000;  // Frequency in Hz
const int pwmResolution= 8;     // 8-bit resolution (0-255 duty cycle)

WebServer server(80);

String htmlPage = R"rawliteral(
<html>
  <head>
    <title>ESP32 Control</title>
    <style>
      body { font-family: Arial; text-align: center; margin-top: 50px; }
      .arrowButton { font-size: 2em; padding: 20px; margin: 10px; width: 60px; }
      #controlPanel { display: inline-block; }
      #motorButtons { margin-bottom: 20px; }
    </style>
  </head>
  <body>
    <h1>ESP32 Control</h1>
    <p>
      Use the on-screen arrows to control the motor and servo:
      <br>Up = motor forward, Down = motor backward,
      <br>Left/Right = steer servo.
    </p>
    <div id="controlPanel">
      <!-- Motor Control Buttons -->
      <div id="motorButtons">
        <button class="arrowButton" id="forwardBtn" onclick="startMotor('forward')"  >↑</button>
        <br>
        <button class="arrowButton" id="leftBtn" onclick="sendRequest('/updateServo?dir=left')">←</button>
        <button class="arrowButton" id="stopBtn" onclick="stopMotor()">■</button>
        <button class="arrowButton" id="rightBtn" onclick="sendRequest('/updateServo?dir=right')">→</button>
        <br>
        <button class="arrowButton" id="backwardBtn" onclick="startMotor('backward')" >↓</button>
      </div>
      <!-- Motor Speed Slider -->
      <p>Motor Speed Duty Cycle:</p>
      <input type="range" id="speedSlider" min="0" max="255" value="128" oninput="updateSpeed(this.value)">
      <span id="speedValue">128</span>
    </div>
    <script>
      var motorInterval = null;
      // Variable to track current active motor direction: 'forward', 'backward', or null
      var currentMotorDir = null;

      // Function to send a request to the ESP32
      function sendRequest(url) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", url, true);
        xhr.send();
      }

      // Start sending motor commands repeatedly while button is held down
      function startMotor(dir) {
        // If another motor direction is active and it is not the same as requested, ignore
        if(currentMotorDir && currentMotorDir !== dir) {
          return;
        }
        currentMotorDir = dir;
        sendRequest('/updateMotor?dir=' + dir);
        motorInterval = setInterval(function() {
          sendRequest('/updateMotor?dir=' + dir);
        }, 100);
      }

      // Stop sending motor commands and clear active direction
      function stopMotor() {
        if (motorInterval) {
          clearInterval(motorInterval);
          motorInterval = null;
        }
        // Send stop command only if a motor command was active
        if(currentMotorDir) {
          sendRequest('/updateMotor?dir=stop');
          currentMotorDir = null;
        }
      }

      // Update motor speed based on slider input
      function updateSpeed(val) {
        document.getElementById("speedValue").innerText = val;
        sendRequest('/updateSpeed?speed=' + val);
      }
    </script>
  </body>
</html>
)rawliteral";

// Handler for serving the main page
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Update servo angle based on on-screen button commands
void handleUpdateServo() {
  if (server.hasArg("dir")) {
    String dir = server.arg("dir");
    if (dir == "left") {
      servoAngle -= 5;
      if (servoAngle < 70) servoAngle = 70;
    }
    else if (dir == "right") {
      servoAngle += 5;
      if (servoAngle > 110) servoAngle = 110;
    }
    myServo.write(servoAngle);
    server.send(200, "text/plain", String(servoAngle));
  } else {
    server.send(400, "text/plain", "Missing 'dir' parameter");
  }
}

// Update motor direction based on on-screen button commands
void handleUpdateMotor() {
  if (server.hasArg("dir")) {
    String dir = server.arg("dir");
    if (dir == "forward") {
      digitalWrite(motorIn1, HIGH);
      digitalWrite(motorIn2, LOW);
    }
    else if (dir == "backward") {
      digitalWrite(motorIn1, LOW);
      digitalWrite(motorIn2, HIGH);
    }
    else if (dir == "stop") {
      digitalWrite(motorIn1, LOW);
      digitalWrite(motorIn2, LOW);
    }
    server.send(200, "text/plain", dir);
  } else {
    server.send(400, "text/plain", "Missing 'dir' parameter");
  }
}

// Update motor PWM duty cycle based on slider input
void handleUpdateSpeed() {
  if (server.hasArg("speed")) {
    int speed = server.arg("speed").toInt();
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;
    analogWrite(motorEn, speed);
    server.send(200, "text/plain", String(speed));
  } else {
    server.send(400, "text/plain", "Missing 'speed' parameter");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize servo
  myServo.attach(servoPin);
  myServo.write(servoAngle);
  
  // Initialize motor control pins
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorEn, 0);
    
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  // Set up web server endpoints
  server.on("/", handleRoot);
  server.on("/updateServo", handleUpdateServo);
  server.on("/updateMotor", handleUpdateMotor);
  server.on("/updateSpeed", handleUpdateSpeed);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}