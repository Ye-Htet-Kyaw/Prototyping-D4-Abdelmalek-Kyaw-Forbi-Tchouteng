/* ============================================================================
 *  Autonomous Line-Following Vehicle  -  First Code Draft (Task 2)
 *  Hochschule Hamm-Lippstadt - Systems Engineering
 *
 *  Hardware:
 *    - Arduino Uno
 *    - L293D H-bridge motor driver (16-pin DIP chip on breadboard)
 *    - 2x Hobby Gearmotor (left wheel / right wheel)
 *    - 2x IR line sensors, 3-pin, active-LOW (LOW = over black line)
 *    - 2x HC-SR04 ultrasonic sensors (one on each side of chassis)
 *    - Battery pack ~7.4V + master switch
 *
 *  Control behaviour (matches Activity / Sequence / State Machine diagrams):
 *    Each cycle ->
 *      1. Trigger both ultrasonic sensors, read echo via pulseIn, convert to cm.
 *      2. If obstacle closer than OBSTACLE_CM -> EMERGENCY STOP
 *         (ENA=ENB=0, all IN pins LOW). Safety has PRIORITY over line tracking.
 *      3. Otherwise read the two IR sensors (digital HIGH/LOW) and steer:
 *           both LOW  (both on line)   -> stop    (intersection / end of track)
 *           left LOW  (left on line)   -> turn left  at TURN_SPEED
 *           right LOW (right on line)  -> turn right at TURN_SPEED
 *           both HIGH (both on white)  -> go forward at CRUISE_SPEED
 * ============================================================================ */

// ========================= PIN ASSIGNMENTS ==================================
// L293D motor driver
const int ENA = 5;    // PWM speed, left motor  (PWM-capable pin)
const int IN1 = 7;    // left motor direction A
const int IN2 = 8;    // left motor direction B
const int IN3 = 4;    // right motor direction A
const int IN4 = 2;    // right motor direction B
const int ENB = 6;    // PWM speed, right motor (PWM-capable pin)

// IR line sensors — 3-pin modules, active-LOW
// LOW  = sensor is over the BLACK line
// HIGH = sensor is over WHITE surface
const int IR_LEFT  = 12;
const int IR_RIGHT = 13;

// HC-SR04 ultrasonic sensors — 4-pin (separate TRIG and ECHO)
const int US1_TRIG = 9;
const int US1_ECHO = 10;
const int US2_TRIG = A0;   // analog pins work as digital I/O
const int US2_ECHO = A1;

// ========================= TUNABLE VALUES ===================================
const int  CRUISE_SPEED = 180;   // 0–255 PWM  (straight line)
const int  TURN_SPEED   = 140;   // 0–255 PWM  (turning / correcting)
const long OBSTACLE_CM  = 15;    // emergency-stop threshold in centimetres

// Change to HIGH if your IR modules output HIGH when over the black line
const int ON_LINE = LOW;

// ========================= SETUP ============================================
void setup() {
  // Motor driver
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // IR sensors
  pinMode(IR_LEFT,  INPUT);
  pinMode(IR_RIGHT, INPUT);

  // Ultrasonic sensors
  pinMode(US1_TRIG, OUTPUT);
  pinMode(US1_ECHO, INPUT);
  pinMode(US2_TRIG, OUTPUT);
  pinMode(US2_ECHO, INPUT);

  stopMotors();           // we decided to start with the motors off
  Serial.begin(9600);
  Serial.println("=== Autonomous Vehicle - control loop starting ===");
}

// ========================= MAIN LOOP ========================================
void loop() {

  // --- Step 1: read both ultrasonic sensors ---
  long d1 = readDistanceCm(US1_TRIG, US1_ECHO);
  long d2 = readDistanceCm(US2_TRIG, US2_ECHO);
  long distance = min(d1, d2);          // use the closer of the two readings

  // --- Step 2: read both IR sensors ---
  int leftIR  = digitalRead(IR_LEFT);
  int rightIR = digitalRead(IR_RIGHT);

  // --- Debug output (open Serial Monitor at 9600 baud) ---
  Serial.print("US1="); Serial.print(d1);
  Serial.print("cm  US2="); Serial.print(d2);
  Serial.print("cm  |  IR_L="); Serial.print(leftIR);
  Serial.print("  IR_R="); Serial.print(rightIR);

  // --- Step 3: decide and act ---
  // Obstacle check has PRIORITY — mirrors the Activity and State Machine diagrams
  if (distance > 0 && distance < OBSTACLE_CM) {
    emergencyStop();
    Serial.println("  ->  OBSTACLE DETECTED: EMERGENCY STOP");
  }
  else {
    // Path is clear — so we will run this one next
    bool leftOnLine  = (leftIR  == ON_LINE);
    bool rightOnLine = (rightIR == ON_LINE);

    if (leftOnLine && rightOnLine) {
      stopMotors();
      Serial.println("  ->  both on line: STOP  (intersection / end)");
    }
    else if (leftOnLine && !rightOnLine) {
      turnLeft();
      Serial.println("  ->  left on line:  TURN LEFT");
    }
    else if (!leftOnLine && rightOnLine) {
      turnRight();
      Serial.println("  ->  right on line: TURN RIGHT");
    }
    else {
      forward();
      Serial.println("  ->  both on white: FORWARD (CRUISE_SPEED)");
    }
  }

  delay(50);   // 50 ms cycle; keeps Serial Monitor readable
}

// ========================= ULTRASONIC HELPER ================================
// Sends a 10 µs trigger pulse, measures echo duration, converts to cm.
// Returns 999 if no echo received (treat as "path clear").
long readDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000UL); // 30 ms timeout
  if (duration == 0) return 999;
  return duration * 0.034 / 2;   // speed of sound: 0.034 cm/µs, ÷2 for round trip
}

// ========================= MOTOR HELPERS ====================================
void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);   // left motor forward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);   // right motor forward
  analogWrite(ENA, CRUISE_SPEED);
  analogWrite(ENB, CRUISE_SPEED);
}

void turnLeft() {
  // Left wheel forward, right wheel reverse → pivots left
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
}

void turnRight() {
  // Right wheel forward, left wheel reverse → pivots right
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
}

void stopMotors() {
  // Stop both motors (coast)
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void emergencyStop() {
  // Cut all motor power immediately — ENA/ENB=0 AND direction pins LOW
  // This is the exact behaviour shown in our sequence and state machine diagrams
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
