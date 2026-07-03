void loop() {
  checkobstacle(); 

  if (obstacleDetected) {
    avoidObstacle(); 
  } else {
    int leftIR = digitalRead(irLeft);
    int rightIR = digitalRead(irRight);
    
    if (leftIR == LOW && rightIR == LOW) {
      if (lastSeenLine == LEFT) currentState = SEARCH_LEFT;
      else if (lastSeenLine == RIGHT) currentState = SEARCH_RIGHT;
      else currentState = STOP;
    } else {
      currentState = FORWARD;
      if (leftIR == HIGH && rightIR == LOW) lastSeenLine = LEFT;
      if (rightIR == HIGH && leftIR == LOW) lastSeenLine = RIGHT;
    }
    actOnState(currentState, leftIR, rightIR);
  }
}
