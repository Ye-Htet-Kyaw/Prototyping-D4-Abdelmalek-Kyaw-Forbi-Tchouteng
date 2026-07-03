void setMotorSpeed(int leftSpeed, int rightSpeed) {
  if (leftSpeed > 0) { digitalWrite(in1, HIGH); digitalWrite(in2, LOW); }
  else if (leftSpeed < 0) { digitalWrite(in1, LOW); digitalWrite(in2, HIGH); }
  else { digitalWrite(in1, LOW); digitalWrite(in2, LOW); }

  if (rightSpeed > 0) { digitalWrite(in3, HIGH); digitalWrite(in4, LOW); }
  else if (rightSpeed < 0) { digitalWrite(in3, LOW); digitalWrite(in4, HIGH); }
  else { digitalWrite(in3, LOW); digitalWrite(in4, LOW); }

  analogWrite(enA, abs(leftSpeed));
  analogWrite(enB, abs(rightSpeed));
}
