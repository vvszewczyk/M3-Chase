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


//consts
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


int borderLeft = 110;
int borderRight = 960;
int highScore;
int MOVE_LEFT = -260;
int MOVE_RIGHT = 260;
double speedIncreasePerPoint = 5.5;
double startGameSpeed = 40;
double maxSpeed = 500;

//sensitivity
float sliderWidth = 300.0f;
float sliderMin = 800.0f;
float sliderMax = sliderMin + sliderWidth;
float sliderMiddle = sliderMin + sliderWidth / 2.0f;
float initialSliderPosition = sliderMiddle - 10;
float lastSensitivitySliderPosition = initialSliderPosition;


//volume
float volumeBarWidth = 300.0f;
float volumeBarX = 800.0f;
float volumeBarY = 200.0f;
float lastVolumeSliderPosition;
float globalVolumeLevel = 100.0f;

int lastDifficulty = 1;

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