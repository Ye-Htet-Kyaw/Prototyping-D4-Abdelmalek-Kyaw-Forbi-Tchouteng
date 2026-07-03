#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include "Config.h"

// Movement Prototypes
void motorStop();
void moveForward();
void rotateTurnLeft();
void rotateTurnRight();
void kickIfNeeded();

#endif