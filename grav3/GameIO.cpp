#include "GameIO.h"

int readScore(std::string path)
{
    std::ifstream inputFile(path);
    int highScore = 0;
    if (inputFile.is_open())
    {
        inputFile >> highScore;
        inputFile.close();
    }
    return highScore;
}

void writeScore(int score, std::string path)
{
    std::ofstream outputFile(path);
    if (outputFile.is_open())
    {
        outputFile << score;
        outputFile.close();
    }
}