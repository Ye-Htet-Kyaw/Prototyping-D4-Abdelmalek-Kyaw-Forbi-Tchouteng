// === Motor control pins ===
const int enA = 3;  // PWM
const int in1 = 8;
const int in2 = 9;
const int enB = 5;  // PWM
const int in3 = 10;
const int in4 = 11;

// === IR sensors ===
const int irLeft = 2;
const int irRight = 4;

// === Ultrasonic front-left and front-right sensors ===
const int trigLeft = 6;
const int echoLeft = 7;
const int trigRight = 12;
const int echoRight = 13;

// === Motor speed settings ===
const float MOTOR_SPEED = 100;
const float TURN_SPEED = 60;

// === PID constants ===
float Kp = 10.0;
float Ki = 0.0;
float Kd = 10.0;
float previousError = 0;
float integral = 0;

// === Obstacle detection state ===
unsigned long lastObstacleCheck = 0;
const unsigned long obstacleCheckInterval = 200;
bool obstacleDetected = false;

// === Movement & line states ===
enum MovementState { STOP, FORWARD, SEARCH_LEFT, SEARCH_RIGHT };
MovementState currentState = STOP;

enum LastSeen { NONE, LEFT, RIGHT };
LastSeen lastSeenLine = NONE;

void setup() {
  // Motor
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // IR
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);

  // Ultrasonic
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
}

void loop() {
  checkobstacle(); // Updates obstacleDetected flag

  if (obstacleDetected) {
    avoidobstacle(); // This handles the full avoidance sequence
  } else {
    // Normal Line Following
    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);
    
    if (leftIR == LOW && rightIR == LOW) {
      // Off line: Search logic
      if (lastSeenLine == LEFT) currentState = SEARCH_LEFT;
      else if (lastSeenLine == RIGHT) currentState = SEARCH_RIGHT;
      else currentState = STOP;
    } else {
      // On line: Follow line
      currentState = FORWARD;
      if (leftIR == HIGH && rightIR == LOW) lastSeenLine = LEFT;
      if (rightIR == HIGH && leftIR == LOW) lastSeenLine = RIGHT;
    }
    actOnState(currentState, leftIR, rightIR);
  }
}

void actOnState(MovementState state, int leftIR, int rightIR) {
  switch (state) {
    case STOP:
      stopMotors();
      break;

    case FORWARD: {
      int error = leftIR - rightIR;
      float derivative = error - previousError;
      integral += error;
      float correction = Kp * error + Ki * integral + Kd * derivative;
      previousError = error;

      int leftSpeed = constrain(MOTOR_SPEED - correction, 0, 255);
      int rightSpeed = constrain(MOTOR_SPEED + correction, 0, 255);
      setMotorSpeed(leftSpeed, rightSpeed);
      break;
    }

    case SEARCH_LEFT:
      setMotorSpeed(-TURN_SPEED, TURN_SPEED);
      break;

    case SEARCH_RIGHT:
      setMotorSpeed(TURN_SPEED, -TURN_SPEED);
      break;
  }
}

void checkobstacle() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastObstacleCheck >= obstacleCheckInterval) {
    int distanceLeft = getDistance(trigLeft, echoLeft);
    int distanceRight = getDistance(trigRight, echoRight);

    bool obstacleNow = ((distanceLeft > 0 && distanceLeft < 25) ||
                        (distanceRight > 0 && distanceRight < 25));

    if (obstacleNow) {
      stopMotors();
    } else if (!obstacleNow) {

    }

    obstacleDetected = obstacleNow;
    lastObstacleCheck = currentMillis;
  }
}

void avoidObstacle() {
  Serial.println("Avoiding obstacle: Reversing...");
  setMotorSpeed(-MOTOR_SPEED, -MOTOR_SPEED);

  // Reverse for 300ms while continuously checking obstacles but NOT checking line sensors
  unsigned long startTime = millis();
  while (millis() - startTime < 300) {
    checkobstacle();
    delay(10);
  }
  stopMotors();

  // Pivot left for 500ms with obstacle checking
  Serial.println("Pivoting left...");
  setMotorSpeed(-TURN_SPEED, TURN_SPEED);
  startTime = millis();
  while (millis() - startTime < 500) {
    checkobstacle();
    delay(10);
  }
  stopMotors();

  // Move forward half speed for 500ms with line search
  Serial.println("Moving forward to bypass obstacle...");
  setMotorSpeed(MOTOR_SPEED / 2, MOTOR_SPEED / 2);
  startTime = millis();
  while (millis() - startTime < 500) {
    checkobstacle();

    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);
    if (leftIR == HIGH || rightIR == HIGH) {
      stopMotors();
      currentState = FORWARD;
      return;
    }
    delay(10);
  }
  stopMotors();

  // Another forward step with searchForLineDuringWindow
  if (searchForLineDuringWindow(500)) {
    return;
  }

  stopMotors();
  checkobstacle();

  // Pivot right to realign with path
  Serial.println("Realigning right...");
  setMotorSpeed(TURN_SPEED, -TURN_SPEED);
  startTime = millis();
  while (millis() - startTime < 700) {
    checkobstacle();

    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);
    if (leftIR == HIGH || rightIR == HIGH) {
      stopMotors();
      currentState = FORWARD;
      return;
    }
    delay(10);
  }
  stopMotors();

  checkobstacle();

  // Move forward again to ensure clearance
  Serial.println("Moving forward again...");
  setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
  startTime = millis();
  while (millis() - startTime < 500) {
    checkobstacle();

    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);
    if (leftIR == HIGH || rightIR == HIGH) {
      stopMotors();
      currentState = FORWARD;
      return;
    }
    delay(10);
  }
  stopMotors();

  checkobstacle();

  // Final line search with searchForLineDuringWindow
  Serial.println("Searching for line...");
  if (searchForLineDuringWindow(1500)) {
    return;
  }

  stopMotors();
  Serial.println("Line not found, initiating search pattern...");

  for (int i = 0; i < 3; i++) {
    Serial.print("Sweep attempt "); Serial.println(i + 1);

    Serial.println(" → Nudge forward");
    setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
    delay(400);
    stopMotors();

    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);
    if (leftIR == HIGH || rightIR == HIGH) {
      Serial.println("Line found during left sweep!");
      stopMotors();
      currentState = FORWARD;
      return;
    }

    Serial.println(" → Sweep left");
    setMotorSpeed(-TURN_SPEED, TURN_SPEED);
    delay(500);
    stopMotors();

    leftIR = digitalRead(irLeft);
    rightIR = digitalRead(irRight);
    if (leftIR == HIGH || rightIR == HIGH) {
      Serial.println("Line found during left sweep!");
      stopMotors();
      currentState = FORWARD;
      return;
    }

    Serial.println(" → Sweep right");
    setMotorSpeed(TURN_SPEED, -TURN_SPEED);
    delay(1000);
    stopMotors();

    leftIR = digitalRead(irLeft);
    rightIR = digitalRead(irRight);
    if (leftIR == HIGH || rightIR == HIGH) {
      Serial.println("Line found during right sweep!");
      stopMotors();
      currentState = FORWARD;
      return;
    }

    Serial.println(" → Return to center");
    setMotorSpeed(-TURN_SPEED, TURN_SPEED);
    delay(500);
    stopMotors();
  }

  Serial.println("Line not found after search pattern. Stopping.");
  currentState = STOP;
}



// === Helper functions ===

void stopMotors() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
  if (leftSpeed > 0) {
    digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  }

  if (rightSpeed > 0) {
    digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  } else if (rightSpeed < 0) {
    digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  } else {
    digitalWrite(in3, LOW); digitalWrite(in4, LOW);
  }

  analogWrite(enA, abs(leftSpeed));
  analogWrite(enB, abs(rightSpeed));
}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 10000);
  if (duration == 0) return -1;
  int cm = duration * 0.034 / 2;
  if (cm < 2 || cm > 400) return -1;
  return cm;
}

bool searchForLineDuringWindow(unsigned long maxDuration) {
  unsigned long startTime = millis();
  setMotorSpeed(MOTOR_SPEED / 1.8, MOTOR_SPEED / 1.8);

  while (millis() - startTime < maxDuration) {
    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);

    if (leftIR == HIGH || rightIR == HIGH) {
      stopMotors();

      if (leftIR == HIGH && rightIR == LOW) lastSeenLine = LEFT;
      else if (rightIR == HIGH && leftIR == LOW) lastSeenLine = RIGHT;

      currentState = FORWARD;
      return true;
    }
  }

  stopMotors();
  return false;
}
