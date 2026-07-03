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
