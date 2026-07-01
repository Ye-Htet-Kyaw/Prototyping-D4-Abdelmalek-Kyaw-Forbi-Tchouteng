// =================================================================
//   LINE-FOLLOWING ROBOT CAR - IR Testing (Ultrasonic Removed)
// =================================================================

// --- PIN CONFIGURATION ---
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

const int leftIR = 2;
const int rightIR = 3;

// =================================================================
//   TUNABLE SETTINGS
// =================================================================
const int baseSpeed = 60;    // Normal speed on straight line
const int turnSpeed = 70;    // Speed of the outer wheel during a turn

const int kickSpeed = 120;   // brief burst to START the motors
const int kickTime  = 50;    // ms the burst lasts
int turnR = 0;
int turnL = 0;
// --- STATE / TIMING ---
bool wasMoving = false;
unsigned long lastLogTime = 0;
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

  Serial.begin(9600);
  Serial.println("--- Robot Car Telemetry Initialized ---");
  
  // --- SAFETY START DELAY ---
  Serial.println("Starting in 3 seconds... Place robot on track!");
  delay(3000); 
  Serial.println("System GO!");
}

void loop() {
  int leftIRState  = digitalRead(leftIR);
  int rightIRState = digitalRead(rightIR);

  // Serial Logging
  if (millis() - lastLogTime >= logInterval) {
    printSensorStatus(leftIRState, rightIRState);
    lastLogTime = millis();
  }

  // --- LINE FOLLOWING LOGIC ---
  
  // Rule 1: BOTH IR on black -> STOP
  if (leftIRState == HIGH && rightIRState == HIGH) {
  unsigned long blackStartTime = millis(); // Record when we first hit double black
    bool reachedStopLine = true;

    // Move forward while monitoring how long both sensors stay black
    // We check for up to 350 milliseconds. Adjust this time if needed!
    while (millis() - blackStartTime < 350) {
      moveForward(); 
      
      // If even ONE sensor goes back to WHITE during this time, 
      // it means we successfully crossed an S-curve or intersection!
      if (digitalRead(leftIR) == LOW || digitalRead(rightIR) == LOW) {
        reachedStopLine = false; 
        break; // Exit this tracking loop and go back to normal driving
      }
    }

    // If both sensors STILL see black after 350ms, it's an intentional stop signal
    if (reachedStopLine) {
      Serial.println("--- STOP LINE DETECTED: Shutting down motors ---");
      motorStop();
      //while(1); // Infinite loop: Locks the robot here until you press Reset
    }
  }
  // Rule 2: RIGHT IR on black -> smooth turn LEFT
  else if (leftIRState == LOW && rightIRState == HIGH) {
    turnR = 0; // Fixed capitalization
    turnLeft(); 
    if(turnL < 45) // Fixed capitalization
      turnL += 15;  // Fixed capitalization
  }
  // Rule 3: LEFT IR on black -> smooth turn RIGHT
  else if (leftIRState == HIGH && rightIRState == LOW) {
    turnL = 0;
    turnRight();
    if(turnR < 45) // Fixed capitalization
      turnR += 10;  // Fixed capitalization
  }
  // Rule 4: BOTH on white -> Go Straight
  else {
    turnR = 0;
    turnL = 0;
    moveForward();
  }
}

void printSensorStatus(int irL, int irR) {
  Serial.print("IR SENSORS | Left: ");
  Serial.print(irL == HIGH ? "BLACK" : "WHITE");
  Serial.print("  Right: ");
  Serial.println(irR == HIGH ? "BLACK" : "WHITE");
}

// =================================================================
//   MOVEMENT FUNCTIONS 
// =================================================================

void kickIfNeeded() {
  if (!wasMoving) {
    analogWrite(ENA, kickSpeed);
    analogWrite(ENB, kickSpeed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(kickTime);
    wasMoving = true;
  }
}

void moveForward() {
  kickIfNeeded();
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
// Smooth Turn Right:
void turnRight() {
  analogWrite(ENA, turnSpeed + turnR); // Left motor runs
  analogWrite(ENB, turnSpeed + turnR);         // Right motor
  
  digitalWrite(IN1, HIGH);     // Left Forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);      // Right backward
  digitalWrite(IN4, HIGH);
  wasMoving = true;
}

void turnLeft() {
  analogWrite(ENA, turnSpeed + turnL);         // Left motor 
  analogWrite(ENB, turnSpeed + turnL); // Right motor runs
  
  digitalWrite(IN1, LOW);      // Left backward
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);     // Right Forward
  digitalWrite(IN4, LOW);
  wasMoving = true;
}

void motorStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  wasMoving = false;
}