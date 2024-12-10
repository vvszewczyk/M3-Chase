#include "GameLogic.h"

std::random_device rd;
std::mt19937 rng(rd());

int getRandomNumber(int a, int b)
{
    std::uniform_int_distribution<int> uni(a, b / 10 * 10);
    return uni(rng);
}

double gameSpeedLevel(int score, double speedIncreasePerPoint, double maxSpeed, double baseSpeed)
{
    double normalizedScore = static_cast<double>(score) / 100.0; //normalize the result to the range [0, 1]
    double exponentialIncrease = baseSpeed * (1.0 - exp(-normalizedScore));
    double linearIncrease = normalizedScore * speedIncreasePerPoint;
    double newSpeed = baseSpeed + exponentialIncrease + linearIncrease;

    return std::min(newSpeed, maxSpeed); //max speed limit
}

void resetObstaclePosition(float& obsY, float& obsX, int screenHeight)
{
    obsY = HEIGHT_RESET;
    obsX = float(getRandomNumber(Core::borderLeft, Core::borderRight));
}

void initializeGameObjects(float& RacerX, float& RacerY, float& Obs1X, float& Obs1Y, float& Obs2X, float& Obs2Y, float& Obs3X, float& Obs3Y, float& Obs4X, float& Obs4Y) {
    RacerX = MIDDLE_POS;
    RacerY = IG_SCREEN_HEIGHT - RACER_HEIGHT - 110;

    Obs1X = float(getRandomNumber(Core::borderLeft, Core::borderRight));
    Obs2X = float(getRandomNumber(Core::borderLeft, Core::borderRight));
    Obs3X = float(getRandomNumber(Core::borderLeft, Core::borderRight));
    Obs4X = float(getRandomNumber(Core::borderLeft, Core::borderRight));

    Obs1Y = START_HEIGHT; Obs2Y = START_HEIGHT - 500; Obs3Y = START_HEIGHT - 1000; Obs4Y = START_HEIGHT - 1500;
}

void updateMoveValues(float sliderPosition)
{
    //in settings
    float normalizedPosition = (sliderPosition - Core::sliderMin) / Core::sliderWidth;
    float valueOffset = (normalizedPosition - 0.5f) * 300; //[-150, 150] (change 300 to rerange) relative to 260
    int currentValue = 260 + static_cast<int>(valueOffset);

    Core::MOVE_LEFT = -currentValue;
    Core::MOVE_RIGHT = currentValue;
}