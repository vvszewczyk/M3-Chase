#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <string>

#define MAX_MAIN_MENU 4
#define FONT_SIZE 70
#define FONT_THICK1 4
#define FONT_THICK2 0
#define OPTION_WIDTH 1300

struct MainMenu 
{
private:
	int mainMenuSelected;
	sf::Font font;
	sf::Text mainMenu[MAX_MAIN_MENU];
	std::string optionNames[MAX_MAIN_MENU] = { "Play", "Settings", "About", "Quit" };
	int optionHeight[MAX_MAIN_MENU] = { 640, 700, 760, 820 };

public:
	MainMenu(float width, float height);
	~MainMenu();

	void draw(sf::RenderWindow& window);
	void moveUp();
	void moveDown();
	int mainMenuPressed()
	{
		return this->mainMenuSelected;
	}
};
