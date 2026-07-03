#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- PIN CONFIGURATION ---
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

const int leftIR = 2;
const int rightIR = 3;

const int trigRight = 12;
const int echoRight = 11;
const int trigLeft  = A0;
const int echoLeft  = A1;

// --- TUNABLE SETTINGS ---
const int baseSpeed = 65;
const int turnSpeed = 75;
const int kickSpeed = 150;
const int kickTime  = 60;

const int obstacleDistance = 16;
const int obstacleConfirm  = 3;
const unsigned long pingInterval = 60;

const int av_rotateSpeed = 95;
const unsigned long rotateOutTime = 200;
const int passSpeed = 85;
const unsigned long passTime = 600;
const unsigned long rotateBackTime = 200;

const int curveOuterSpeed = 82;
const int curveInnerSpeed = 60;
const unsigned long curveGuard = 200;

const int realignSpeed = 90;
const unsigned long realignTime = 150;
const unsigned long brakeTime = 500;
const unsigned long avoidTimeout = 6000;

// --- SHARED GLOBALS (Externed) ---
extern bool is180;
extern int turnR;
extern int turnL;
extern bool wasMoving;
extern long lastDistL;
extern long lastDistR;

#endif