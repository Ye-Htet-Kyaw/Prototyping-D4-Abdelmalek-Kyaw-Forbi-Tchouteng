#include "LineFollow.h"

void processLineFollowing() {
  int leftIRState  = digitalRead(leftIR);
  int rightIRState = digitalRead(rightIR);

  // Rule 1: BOTH IR on black -> STOP
  if (leftIRState == HIGH && rightIRState == HIGH) {
    unsigned long blackStartTime = millis();
    bool reachedStopLine = true;

    while (millis() - blackStartTime < 350) {
      moveForward(); 
      if (digitalRead(leftIR) == LOW || digitalRead(rightIR) == LOW) {
        reachedStopLine = false; 
        break; 
      }
    }
    if (reachedStopLine) {
      Serial.println("--- STOP LINE DETECTED ---");
      motorStop();
      while(1); 
    }
  }

  // Rule 2: RIGHT IR on black -> rotate turn Right
  else if (leftIRState == LOW && rightIRState == HIGH) {
    turnL = 0; 
    rotateTurnRight();
    if(turnR < 60) turnR += 15; 
  }

  // Rule 3: LEFT IR on black -> rotate turn Left
  else if (leftIRState == HIGH && rightIRState == LOW) {
    turnR = 0;
    rotateTurnLeft();
    if(turnL < 60) turnL += 15;
  }

  // Rule 4: BOTH on white -> Go Straight
  else {
    turnR = 0;
    turnL = 0;
    moveForward();
  }
}