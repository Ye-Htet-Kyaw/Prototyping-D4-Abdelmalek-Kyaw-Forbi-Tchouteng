// =================================================================
//   LINE-FOLLOWING ROBOT CAR
//   Stop-line detection + obstacle avoidance (half-circle, right)
// =================================================================

// --- PIN CONFIGURATION ---
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

const int leftIR  = 2;
const int rightIR = 3;

// Front ultrasonic sensor (HC-SR04)
// >>> CHANGE THESE TWO PINS TO MATCH YOUR WIRING <
const int trigPin = 12;
const int echoPin = 11;

// =================================================================
//   TUNABLE SETTINGS  
// =================================================================

// --- Driving ---
const int baseSpeed = 60;    // normal speed on straight line
const int turnSpeed = 70;    // base speed during a pivot
const int kickSpeed = 120;   // brief burst to START the motors
const int kickTime  = 50;    // ms the burst lasts

// --- Turn ramping (pivot gets stronger the longer the line is lost) ---
const int rampStep = 15;                  // added per step
const int rampMax  = 45;                  // ceiling for the extra speed
const unsigned long rampInterval = 75;    // ms between ramp steps

// --- Stop line ---
const unsigned long stopProbeTime = 350;  // ms of double-black = stop

// --- Obstacle avoidance  ---
const int obstacleDistance       = 14;    // cm - trigger distance
const int obstacleConfirm        = 2;     // readings in a row to confirm
const unsigned long pingInterval = 60;    // ms between ultrasonic pings
const int pivotSpeed             = 100;   // speed of the blind pivot
const unsigned long pivotTime    = 350;   // ms - blind pivot duration
const int arcOuterSpeed          = 110;   // fast wheel during the arc
const int arcInnerSpeed          = 50;    // slow wheel during the arc
const unsigned long avoidTimeout = 6000;  // ms - give up if no line found

// --- STATE / TIMING ---
bool wasMoving = false;
int  turnR = 0;
int  turnL = 0;
unsigned long lastRampTime = 0;

unsigned long lastLogTime  = 0;
const unsigned long logInterval = 500;

unsigned long lastPingTime = 0;
long lastDistance  = 999;
int  obstacleCount = 0;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println("--- Robot Car Telemetry Initialized ---");

  // --- SAFETY START DELAY ---
  Serial.println("Starting in 3 seconds... Place robot on track!");
  delay(3000);
  Serial.println("System GO!");
}

void loop() {

  // ---------- PRIORITY 1: OBSTACLE CHECK ----------
  if (millis() - lastPingTime >= pingInterval) {
    lastPingTime = millis();
    lastDistance = readDistanceCM();

    if (lastDistance <= obstacleDistance) obstacleCount++;
    else                                  obstacleCount = 0;

    if (obstacleCount >= obstacleConfirm) {
      obstacleCount = 0;
      avoidObstacle();   // blocking maneuver, returns once back on the line
      return;            // start a fresh loop pass afterwards
    }
  }

  // ---------- PRIORITY 2: LINE FOLLOWING ----------
  int leftIRState  = digitalRead(leftIR);
  int rightIRState = digitalRead(rightIR);

  if (millis() - lastLogTime >= logInterval) {
    printSensorStatus(leftIRState, rightIRState);
    lastLogTime = millis();
  }

  // Rule 1: BOTH IR on black -> stop line OR intersection
  if (leftIRState == HIGH && rightIRState == HIGH) {
    unsigned long blackStartTime = millis();
    bool reachedStopLine = true;

    // Drive on while checking if the double-black persists
    while (millis() - blackStartTime < stopProbeTime) {
      moveForward();

      // One sensor back to WHITE = it was only a crossing / S-curve
      if (digitalRead(leftIR) == LOW || digitalRead(rightIR) == LOW) {
        reachedStopLine = false;
        break;
      }
    }

    // Still both black after the probe -> intentional stop signal
    if (reachedStopLine) {
      Serial.println("--- STOP LINE DETECTED: Shutting down motors ---");
      motorStop();
      while (true) { }   // locked until reset (comment out for bench tests)
    }
  }
  // Rule 2: RIGHT IR on black -> correct toward the line
  else if (leftIRState == LOW && rightIRState == HIGH) {
    turnR = 0;
    turnLeft();
    if (millis() - lastRampTime >= rampInterval) {
      turnL = min(turnL + rampStep, rampMax);
      lastRampTime = millis();
    }
  }
  // Rule 3: LEFT IR on black -> correct toward the line
  else if (leftIRState == HIGH && rightIRState == LOW) {
    turnL = 0;
    turnRight();
    if (millis() - lastRampTime >= rampInterval) {
      turnR = min(turnR + rampStep, rampMax);
      lastRampTime = millis();
    }
  }
  // Rule 4: BOTH on white -> go straight
  else {
    turnR = 0;
    turnL = 0;
    moveForward();
  }
}

// =================================================================
//   OBSTACLE AVOIDANCE  (half-circle around the right side)
// =================================================================

void avoidObstacle() {
  Serial.println(">>> OBSTACLE DETECTED - starting avoidance");

  // Phase 1a: blind pivot RIGHT on a timer. The IR sensors sweep
  // over our own line during this rotation, so we must NOT listen
  // to them yet - that is why this part is time-based.
  Serial.println(">>> Phase 1a: pivot right");
  avoidPivotRight();

  // Phase 1b: arc LEFT around the obstacle until the LEFT sensor
  // touches the line on the far side.
  Serial.println(">>> Phase 1b: arc around the obstacle");
  startArcLeft();

  unsigned long t0 = millis();
  while (digitalRead(leftIR) == LOW) {
    if (millis() - t0 > avoidTimeout) failSafeStop();
  }

  // Phase 2: keep the same arc while the nose crosses the line,
  // until the RIGHT sensor confirms it.
  Serial.println(">>> Phase 2: left IR found the line, crossing it");
  t0 = millis();
  while (digitalRead(rightIR) == LOW) {
    if (millis() - t0 > avoidTimeout) failSafeStop();
  }

  // Phase 3: hand control back to the normal line-following rules
  Serial.println(">>> Phase 3: line re-acquired, resuming");
  turnL = 0;
  turnR = 0;
  wasMoving = true;   // motors are already spinning
}

// Blind pivot to the physical RIGHT.
// NOTE: this uses the same pin pattern as turnLeft(), because on this
// chassis that is the combination that physically swings the nose to
// the right (your steering calls in loop() are swapped the same way).
// TEST ONCE: trigger the ultrasonic with your hand - the nose must
// swing RIGHT. If it swings LEFT instead: swap the four IN states
// here AND swap the two arc speeds in startArcLeft().
void avoidPivotRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, kickSpeed);    // burst to break static friction
  analogWrite(ENB, kickSpeed);
  delay(kickTime);

  analogWrite(ENA, pivotSpeed);
  analogWrite(ENB, pivotSpeed);
  delay(pivotTime);
}

// Left-curving arc: both wheels forward, outer wheel faster.
// The motor on ENA is the outer wheel for this maneuver.
void startArcLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, kickSpeed);    // short kick so both wheels start
  analogWrite(ENB, kickSpeed);
  delay(kickTime);

  analogWrite(ENA, arcOuterSpeed);   // outer wheel (fast)
  analogWrite(ENB, arcInnerSpeed);   // inner wheel (slow)
}

// Emergency stop if the line is never found during avoidance
void failSafeStop() {
  motorStop();
  Serial.println("!!! LINE LOST during avoidance - halted, press reset");
  while (true) { }
}

// Front ultrasonic (HC-SR04). Returns distance in cm, 999 = nothing.
long readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 12000);   // ~2 m max range
  if (duration == 0) return 999;
  return duration / 58;
}

void printSensorStatus(int irL, int irR) {
  Serial.print("IR | Left: ");
  Serial.print(irL == HIGH ? "BLACK" : "WHITE");
  Serial.print("  Right: ");
  Serial.print(irR == HIGH ? "BLACK" : "WHITE");
  Serial.print("  |  Distance: ");
  if (lastDistance >= 999) Serial.println("--");
  else { Serial.print(lastDistance); Serial.println(" cm"); }
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

// Full pivot: one wheel forward, one reversed
void turnRight() {
  analogWrite(ENA, turnSpeed + turnR);
  analogWrite(ENB, turnSpeed + turnR);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  wasMoving = true;
}

// Full pivot: one wheel forward, one reversed
void turnLeft() {
  analogWrite(ENA, turnSpeed + turnL);
  analogWrite(ENB, turnSpeed + turnL);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
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