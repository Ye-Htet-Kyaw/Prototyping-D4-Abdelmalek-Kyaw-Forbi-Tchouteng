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
