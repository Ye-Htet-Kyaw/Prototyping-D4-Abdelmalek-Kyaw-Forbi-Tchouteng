#include "Config.h"
#include "Motors.h"
#include "Sensors.h"
#include "Obstacle.h"
#include "LineFollow.h"

// Definition of shared global variables (initialized here for memory allocation)
bool is180 = false;
int turnR = 0;
int turnL = 0;
bool wasMoving = false;
long lastDistL = 999;
long lastDistR = 999;

// Shared sensor timing and state variables
unsigned long lastLogTime = 0;
unsigned long lastPingTime = 0;

int  obstacleCheckCount = 0;
const unsigned long logInterval = 500;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);

  Serial.begin(9600);
  Serial.println("--- Robot Car Telemetry Initialized ---");
  
  // --- SAFETY START DELAY ---
  Serial.println("Starting in 1 seconds... Place robot on track!");
  delay(1000); 
  Serial.println("System GO!");
}

void loop() {
  // 1. Check for obstacles
  if (millis() - lastPingTime >= pingInterval) {
    lastPingTime = millis();
    lastDistL = readDistanceCM(trigLeft, echoLeft);
    lastDistR = readDistanceCM(trigRight, echoRight);
    long closest = min(lastDistL, lastDistR);

    if (closest <= obstacleDistance) obstacleCheckCount++;
    else obstacleCheckCount = 0;

    if (obstacleCheckCount >= obstacleConfirm) {
      obstacleCheckCount = 0;
      if(!is180){
        avoidObstacle();
        is180 = true;
        return; 
      }
      else{
        turn180();
        is180 = false;
        return;
      }
    }
  }

  // 2. Log status
  if (millis() - lastLogTime >= logInterval) {
    printSensorStatus(digitalRead(leftIR), digitalRead(rightIR));
    lastLogTime = millis();
  }

  // 3. Process movement
  processLineFollowing();
}