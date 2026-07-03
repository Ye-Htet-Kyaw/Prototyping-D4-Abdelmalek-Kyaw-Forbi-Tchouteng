void checkobstacle() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastObstacleCheck >= obstacleCheckInterval) {
    int distanceLeft = getDistance(trigLeft, echoLeft);
    int distanceRight = getDistance(trigRight, echoRight);
    bool obstacleNow = ((distanceLeft > 0 && distanceLeft < 25) ||
                        (distanceRight > 0 && distanceRight < 25));
    if (obstacleNow) { stopMotors(); }
    obstacleDetected = obstacleNow;
    lastObstacleCheck = currentMillis;
  }
}
