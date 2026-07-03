#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <Arduino.h>
#include "Config.h"
#include "Motors.h"
#include "Sensors.h"

void avoidObstacle();
void turn180();
void brakePause();
void av_rotateLeft(int speed, unsigned long ms);
void av_rotateRight(int speed, unsigned long ms);
void av_forwardStraight(int speed, unsigned long ms);
void av_pivotLeft();
void av_pivotRight();
bool av_curveBackUntilLeftBlack();

#endif