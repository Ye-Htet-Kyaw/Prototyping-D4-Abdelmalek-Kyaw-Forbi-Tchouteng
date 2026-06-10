// --- PIN CONFIGURATION ---

// L298N / L293D Motor Driver Pins
const int ENA = 5;  // Right motor speed (PWM)
const int IN1 = 6;  // Right motor forward
const int IN2 = 7;  // Right motor backward
const int IN3 = 8;  // Left motor forward
const int IN4 = 9;  // Left motor backward
const int ENB = 10; // Left motor speed (PWM)

// Ultrasonic Sensor 1 (Left)
const int trigPin1 = A0;
const int echoPin1 = A1;

// Ultrasonic Sensor 2 (Right)
const int trigPin2 = A2;
const int echoPin2 = A3;

// IR Line Tracking Sensors
const int leftIR = 2;
const int rightIR = 3;

// --- SETTINGS ---
const int obstacleDistance = 15; // Stop distance in cm
const int motorSpeed = 255;      // Speed of the robot (0 to 255)

// --- TIMING VARIABLES FOR SERIAL MONITOR ---
unsigned long lastLogTime = 0;
const unsigned long logInterval = 500; // Print data every 500ms

void setup() {
  // Initialize Motor Pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialize Ultrasonic Pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Initialize IR Pins
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);
  
  // Initialize Serial Communication at 9600 baud rate
  Serial.begin(9600); 
  Serial.println("--- Robot Car Telemetry Initialized ---");
}

void loop() {
  // 1. Read distances from both ultrasonic sensors
  int distanceLeft = getDistance(trigPin1, echoPin1);
  int distanceRight = getDistance(trigPin2, echoPin2);

  // 2. Read IR sensor states (HIGH = Black line, LOW = White space)
  int leftIRState = digitalRead(leftIR);
  int rightIRState = digitalRead(rightIR);

  // 3. Print Telemetry to Serial Monitor every 500ms
  if (millis() - lastLogTime >= logInterval) {
    printSensorStatus(distanceLeft, distanceRight, leftIRState, rightIRState);
    lastLogTime = millis();
  }

  // 4. Logic Control
  
  // Rule 1: If ANY ultrasonic sensor detects an obstacle within 15cm -> STOP
  if ((distanceLeft > 0 && distanceLeft <= obstacleDistance) || 
      (distanceRight > 0 && distanceRight <= obstacleDistance)) {
    motorStop();
  } 
  // Rule 2: Both IR sensors detect the black line -> STOP
  else if (leftIRState == HIGH && rightIRState == HIGH) {
    motorStop();
  }
  // Rule 3: Right IR detects black line -> Turn Right
  else if (leftIRState == LOW && rightIRState == HIGH) {
    turnRight();
  }
  // Rule 4: Left IR detects black line -> Turn Left
  else if (leftIRState == HIGH && rightIRState == LOW) {
    turnLeft();
  }
  // Rule 5: Both sensors on white space -> Go Straight
  else {
    moveForward();
  }
  
  delay(10); // Small delay for overall stability
}

// --- DEBUG LOGGING FUNCTION ---
void printSensorStatus(int distL, int distR, int irL, int irR) {
  Serial.print("ULTRASONIC | Left: ");
  if (distL == 999) Serial.print("Error/Timeout");
  else { Serial.print(distL); Serial.print("cm"); }
  
  Serial.print("  Right: ");
  if (distR == 999) Serial.print("Error/Timeout");
  else { Serial.print(distR); Serial.print("cm"); }

  Serial.print("  ||  IR SENSORS | Left: ");
  Serial.print(irL == HIGH ? "BLACK" : "WHITE");
  Serial.print("  Right: ");
  Serial.println(irR == HIGH ? "BLACK" : "WHITE");
}

// --- ROBOT MOVEMENT FUNCTIONS ---

void moveForward() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH); 
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// --- ULTRASONIC DISTANCE CALCULATION FUNCTION ---
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 25000); // 25ms timeout for efficiency
  
  if (duration == 0) {
    return 999; 
  }
  
  return duration * 0.034 / 2;
}