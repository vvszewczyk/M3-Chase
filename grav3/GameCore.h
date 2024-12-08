#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "menu.h"
#include <thread>
#include <fstream>
#include <Windows.h>
#include <random>
#include <cassert>


// Sta³e
const int IG_SCREEN_WIDTH = 1000;
const int IG_SCREEN_HEIGHT = 839;
const int HEIGHT_RESET = -400;
const int MIDDLE_POS = 535;
const int START_HEIGHT = -1000;
const int IG_FONT_SIZE = 40;
const int OPT_FONT_SIZE = 70;
const int MENU_WIDTH = 1600;
const int MENU_HEIGHT = 1000;
const int RACER_HEIGHT = 70;
const int MIN_OBSTACLE_SPACING = 300;

// Deklaracje zmiennych globalnych
extern int borderLeft;
extern int borderRight;
extern int highScore;
extern int MOVE_LEFT;
extern int MOVE_RIGHT;
extern double speedIncreasePerPoint;
extern double startGameSpeed;
extern double maxSpeed;
extern float sliderWidth;
extern float sliderMin;
extern float sliderMax;
extern float sliderMiddle;
extern float initialSliderPosition;
extern float lastSensitivitySliderPosition;
extern float volumeBarWidth;
extern float volumeBarX;
extern float volumeBarY;
extern float lastVolumeSliderPosition;
extern float globalVolumeLevel;
extern int lastDifficulty;

struct GameResources
{
    sf::Texture background, racer, obs1, obs2, obs3, obs4, gameover, crash1, info;
    sf::Font myfont;
    sf::SoundBuffer crash, lobbySound, igSound;

    bool loadTexture(sf::Texture& texture, const std::string& filename);
    bool loadSound(sf::SoundBuffer& soundBuffer, const std::string& filename);
    bool loadFont(sf::Font& font, const std::string& filename);
    bool loadAllResources();
};