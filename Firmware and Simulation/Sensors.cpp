#include "Sensors.h"

long readDistanceCM(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 8000);
  if (duration == 0) return 999;
  return duration / 58;
}

void printSensorStatus(int irL, int irR) {
  Serial.print("IR SENSORS | Left: ");
  Serial.print(irL == HIGH ? "BLACK" : "WHITE");
  Serial.print("  Right: ");
  Serial.print(irR == HIGH ? "BLACK" : "WHITE");
  Serial.print("  |  US L: ");
  if (lastDistL >= 999) Serial.print("--"); else { Serial.print(lastDistL); Serial.print("cm"); }
  Serial.print("  US R: ");
  if (lastDistR >= 999) Serial.println("--"); else { Serial.print(lastDistR); Serial.println("cm"); }
}