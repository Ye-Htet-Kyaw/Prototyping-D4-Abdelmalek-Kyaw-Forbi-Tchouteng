#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "Config.h"

long readDistanceCM(int trig, int echo);
void printSensorStatus(int irL, int irR);

#endif