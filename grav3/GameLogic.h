#pragma once
#include "GameCore.h"



int getRandomNumber(int a, int b);
double gameSpeedLevel(int score, double speedIncreasePerPoint, double maxSpeed, double baseSpeed = 270);
void resetObstaclePosition(float& obsY, float& obsX, int screenHeight);
void initializeGameObjects(float& RacerX, float& RacerY, float& Obs1X, float& Obs1Y, float& Obs2X, float& Obs2Y, float& Obs3X, float& Obs3Y, float& Obs4X, float& Obs4Y);