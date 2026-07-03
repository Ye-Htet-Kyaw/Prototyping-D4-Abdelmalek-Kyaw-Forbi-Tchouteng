int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 10000);
  if (duration == 0) return -1;
  int cm = duration * 0.034 / 2;
  if (cm < 2 || cm > 400) return -1;
  return cm;
}
