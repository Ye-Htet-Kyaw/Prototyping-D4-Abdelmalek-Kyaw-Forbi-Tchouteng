#include "Obstacle.h"

void avoidObstacle() {
  brakePause();
  av_rotateLeft(av_rotateSpeed, rotateOutTime);    
  brakePause();

  av_forwardStraight(passSpeed, passTime);    
  brakePause();

  av_rotateRight(av_rotateSpeed, rotateBackTime);    
  brakePause();

  // Phase 4: Closed-loop movement back to the line
  if (!av_curveBackUntilLeftBlack()) {         
    motorStop();
    return;
  }

  brakePause();

  // Fine-tune alignment
  while(digitalRead(leftIR) == LOW){
    av_pivotLeft();
  }
  
  brakePause();

  turnL = 0;
  turnR = 0;
  wasMoving = false;
}

void turn180() {
  turnL = 0;
  turnR = 0;
  brakePause();
  // Rotate out wider to clear the track/obstacle
  av_rotateLeft(av_rotateSpeed, rotateOutTime + 200); 
  
  while(digitalRead(leftIR) == LOW){
    rotateTurnLeft(); // Using standard motor logic
  }
  brakePause();
  
  while(digitalRead(rightIR) == LOW){
    av_pivotRight();
  }
  brakePause();
}

void brakePause() {
  motorStop();
  delay(brakeTime);
}

void av_rotateRight(int speed, unsigned long ms) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, kickSpeed);
  analogWrite(ENB, kickSpeed);
  delay(kickTime);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  delay(ms);
  motorStop();
}

void av_rotateLeft(int speed, unsigned long ms) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, kickSpeed);
  analogWrite(ENB, kickSpeed);
  delay(kickTime);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  delay(ms);
  motorStop();
}

void av_forwardStraight(int speed, unsigned long ms) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, kickSpeed);
  analogWrite(ENB, kickSpeed);
  delay(kickTime);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  delay(ms);
  motorStop();
}

bool av_curveBackUntilLeftBlack() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, kickSpeed);
  analogWrite(ENB, kickSpeed);
  delay(kickTime);

  analogWrite(ENA, curveInnerSpeed);
  analogWrite(ENB, curveOuterSpeed);

  delay(curveGuard);   // Brief blind start 

  unsigned long t0 = millis();
  while (digitalRead(leftIR) == LOW) {
    if (millis() - t0 > avoidTimeout) {
      motorStop();
      return false;
    }
  }
  motorStop();
  return true;
}

void av_pivotLeft() {
  analogWrite(ENA, turnSpeed);    
  analogWrite(ENB, 0); 
  
  digitalWrite(IN1, HIGH);    
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);      
  digitalWrite(IN4, HIGH);
  wasMoving = true;
}

void av_pivotRight() {
  analogWrite(ENA, 0); 
  analogWrite(ENB, turnSpeed); 
  
  digitalWrite(IN1, LOW);      
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);     
  digitalWrite(IN4, LOW);
  wasMoving = true;
}