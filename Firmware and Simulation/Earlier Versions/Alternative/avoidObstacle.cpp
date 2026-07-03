void avoidObstacle() {
  Serial.println("Avoiding obstacle: Reversing...");
  setMotorSpeed(-MOTOR_SPEED, -MOTOR_SPEED);

  // Reverse for 300ms
  unsigned long startTime = millis();
  while (millis() - startTime < 300) {
    checkobstacle();
    delay(10);
  }
  stopMotors();

  // Pivot left for 500ms
  Serial.println("Pivoting left...");
  setMotorSpeed(-TURN_SPEED, TURN_SPEED);
  startTime = millis();
  while (millis() - startTime < 500) {
    checkobstacle();
    delay(10);
  }
  stopMotors();

  // Forward bypass with line detection
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

  // Secondary search window
  if (searchForLineDuringWindow(500)) { return; }

  // Realign to the right
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

  // Final clearance forward
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

  // Final fallback search pattern
  if (searchForLineDuringWindow(1500)) { return; }

  // Fallback: Systematic sweep
  for (int i = 0; i < 3; i++) {
    setMotorSpeed(MOTOR_SPEED, MOTOR_SPEED);
    delay(400); // Nudge
    setMotorSpeed(-TURN_SPEED, TURN_SPEED);
    delay(500); // Sweep left
    setMotorSpeed(TURN_SPEED, -TURN_SPEED);
    delay(1000); // Sweep right
    setMotorSpeed(-TURN_SPEED, TURN_SPEED);
    delay(500); // Reset center
    stopMotors();
  }
  currentState = STOP;
}
