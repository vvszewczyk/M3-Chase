#include "menu.h"

MainMenu::MainMenu(float width, float height)
{
    if (!font.loadFromFile("assets/font.ttf"))
    {
        std::cerr << "No font is here";
    }

    mainMenuSelected = 0;

    for (std::size_t i = 0; i < MAX_MAIN_MENU; i++)
    {
        mainMenu[i].setFont(font);
        mainMenu[i].setOutlineColor(sf::Color::Blue);
        mainMenu[i].setString(optionNames[i]);
        mainMenu[i].setCharacterSize(FONT_SIZE);
        mainMenu[i].setPosition(OPTION_WIDTH, optionHeight[i]);

        if (i == mainMenuSelected) 
        {
            mainMenu[i].setOutlineThickness(FONT_THICK1);
        }
        else {
            mainMenu[i].setOutlineThickness(FONT_THICK2);
        }
    }
}


MainMenu::~MainMenu()
{

}

//draw MainMenu
void MainMenu::draw(sf::RenderWindow& window)
{
	for (int i = 0; i < MAX_MAIN_MENU; i++)
	{
		window.draw(mainMenu[i]);
	}
}


void MainMenu::moveUp()
{
    if (mainMenuSelected >= 0) 
    {
        mainMenu[mainMenuSelected].setOutlineThickness(0);
    }

    mainMenuSelected--;
    if (mainMenuSelected < 0)
    {
        mainMenuSelected = MAX_MAIN_MENU - 1;
    }

    mainMenu[mainMenuSelected].setOutlineThickness(4);
}

void MainMenu::moveDown()
{
    if (mainMenuSelected >= 0) 
    {
        mainMenu[mainMenuSelected].setOutlineThickness(0);
    }

    mainMenuSelected++;
    if (mainMenuSelected >= MAX_MAIN_MENU)
    {
        mainMenuSelected = 0;
    }

    mainMenu[mainMenuSelected].setOutlineThickness(4);
}
