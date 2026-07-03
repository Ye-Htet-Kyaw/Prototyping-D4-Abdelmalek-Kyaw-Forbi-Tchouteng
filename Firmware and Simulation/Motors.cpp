#include "Motors.h"

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

void motorStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  wasMoving = false;
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

void rotateTurnLeft() {
  analogWrite(ENA, turnSpeed + turnL); 
  analogWrite(ENB, turnSpeed + turnL); 
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  wasMoving = true;
}

void rotateTurnRight() {
  analogWrite(ENA, turnSpeed + turnR);
  analogWrite(ENB, turnSpeed + turnR);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  wasMoving = true;
}