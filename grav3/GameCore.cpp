#include "GameCore.h"
#include "GameEngine.h"
#include "GameIO.h"
#include "GameLogic.h"

namespace Core
{
int borderLeft = 110;
int borderRight = 960;
int highScore = 0;
int MOVE_LEFT = -260;
int MOVE_RIGHT = 260;
double speedIncreasePerPoint = 5.5;
double startGameSpeed = 40;
double maxSpeed = 500;
float sliderWidth = 300.0f;
float sliderMin = 800.0f;
float sliderMax = sliderMin + sliderWidth;
float sliderMiddle = sliderMin + sliderWidth / 2.0f;
float initialSliderPosition = sliderMiddle - 10;
float lastSensitivitySliderPosition = initialSliderPosition;
float volumeBarWidth = 300.0f;
float volumeBarX = 800.0f;
float volumeBarY = 200.0f;
float lastVolumeSliderPosition = 0.0f;
float globalVolumeLevel = 100.0f;
int lastDifficulty = 1;
} // namespace Core

void initializeMenuWindow(sf::RenderWindow &MENU, sf::RectangleShape &background, sf::Texture &menuTexture)
{
    MENU.create(sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), "M3 Chase", sf::Style::Titlebar | sf::Style::Close);
    MENU.setPosition(sf::Vector2i(160, 0));
    background.setSize(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
    menuTexture.loadFromFile("assets/menu.png");
    background.setTexture(&menuTexture);
}

void handleMenuEvents(sf::RenderWindow &MENU, MainMenu &mainMenu, sf::Sound &menuSound, int &lastScore,
                      sf::RectangleShape &playBackground, sf::Text &pressEnter, sf::RectangleShape &settingsBackground,
                      sf::RectangleShape &aboutBackground, sf::Font font, sf::RectangleShape background)
{
    while (MENU.isOpen())
    {
        HWND hwnd = MENU.getSystemHandle();
        sf::Event event;
        while (MENU.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                MENU.close();
            }

            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Up)
                {
                    mainMenu.moveUp();
                    break;
                }

                if (event.key.code == sf::Keyboard::Down)
                {
                    mainMenu.moveDown();
                    break;
                }

                if (event.key.code == sf::Keyboard::Return)
                {
                    // option windows
                    sf::RenderWindow Play(sf::VideoMode(IG_SCREEN_WIDTH, IG_SCREEN_HEIGHT), "M3 Chase",
                                          sf::Style::Titlebar | sf::Style::Close);
                    sf::RenderWindow Options(sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), "M3 Chase",
                                             sf::Style::Titlebar | sf::Style::Close);
                    sf::RenderWindow About(sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), "M3 Chase",
                                           sf::Style::Titlebar | sf::Style::Close);
                    Play.setPosition(sf::Vector2i(460, 120));
                    Options.setPosition(sf::Vector2i(160, 0));
                    About.setPosition(sf::Vector2i(160, 0));

                    // sensitivity
                    sf::Text sensitivityText("Steering sensitivity", font, OPT_FONT_SIZE);
                    sensitivityText.setPosition(320, 150);
                    sensitivityText.setOutlineColor(sf::Color::Blue);

                    sf::CircleShape sensitivitySlider(15);
                    sensitivitySlider.setPosition(Core::initialSliderPosition, 190);
                    sensitivitySlider.setFillColor(sf::Color::Blue);

                    sf::RectangleShape sensitivityBar(sf::Vector2f(300, 10));
                    sensitivityBar.setPosition(800, 200);
                    sensitivityBar.setFillColor(sf::Color::White);

                    // Volume
                    sf::Text volumeText("Volume", font, OPT_FONT_SIZE);
                    volumeText.setPosition(590, 260);
                    volumeText.setOutlineColor(sf::Color::Blue);

                    sf::CircleShape volumeSlider(15);
                    float volumeSliderInitialX =
                        Core::volumeBarX + (Core::volumeBarWidth / 2.0f) - (volumeSlider.getRadius());
                    volumeSlider.setFillColor(sf::Color::Blue);

                    sf::RectangleShape volumeBar(sf::Vector2f(300, 10));
                    volumeBar.setPosition(800, 313);
                    volumeBar.setFillColor(sf::Color::White);
                    volumeSlider.setPosition(volumeSliderInitialX, 303);
                    Core::lastVolumeSliderPosition = volumeSliderInitialX;

                    // game difficulty levels
                    sf::Text gameDifficulty("Game difficulty:", font, OPT_FONT_SIZE);
                    gameDifficulty.setPosition(320, 370);
                    gameDifficulty.setOutlineColor(sf::Color::Blue);
                    sf::Text d1("Easy", font, OPT_FONT_SIZE);
                    d1.setPosition(700, 370);
                    d1.setOutlineColor(sf::Color::Blue);
                    sf::Text d2("Normal", font, OPT_FONT_SIZE);
                    d2.setPosition(850, 370);
                    d2.setOutlineColor(sf::Color::Blue);
                    sf::Text d3("Frog", font, OPT_FONT_SIZE);
                    d3.setPosition(1050, 370);
                    d3.setOutlineColor(sf::Color::Blue);

                    // settings selected option and difficulty position
                    int selectedSlider = 0;
                    int selectedDiff = Core::lastDifficulty;

                    sensitivityText.setOutlineThickness((selectedSlider == 0) ? 4.0f : 0);
                    d1.setOutlineThickness((selectedDiff == 0) ? 4.0f : 0);
                    d2.setOutlineThickness((selectedDiff == 1) ? 4.0f : 0);
                    d3.setOutlineThickness((selectedDiff == 2) ? 4.0f : 0);

                    int x = mainMenu.mainMenuPressed();

                    // play chosen
                    if (x == 0)
                    {
                        menuSound.stop();
                        while (Play.isOpen())
                        {
                            // minimalize menu
                            ShowWindow(hwnd, SW_MINIMIZE);

                            // read scores
                            lastScore = readScore("lastscore.txt");
                            Core::highScore = readScore("highscore.txt");
                            sf::Text lScore("LAST SCORE: " + std::to_string(lastScore), font, IG_FONT_SIZE);
                            sf::Text hScore("HIGH SCORE: " + std::to_string(Core::highScore), font, IG_FONT_SIZE);
                            lScore.setOutlineColor(sf::Color::Blue);
                            lScore.setOutlineThickness(2.0f);
                            hScore.setOutlineColor(sf::Color::Blue);
                            hScore.setOutlineThickness(2.0f);
                            lScore.setPosition(5, 30);
                            hScore.setPosition(5, 0);

                            sf::Event aevent;
                            while (Play.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                {
                                    Play.close();

                                    // bring back menu
                                    ShowWindow(hwnd, SW_NORMAL);
                                }
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                    {
                                        Play.close();

                                        ShowWindow(hwnd, SW_NORMAL);
                                        menuSound.play();
                                    }
                                    if (aevent.key.code == sf::Keyboard::Enter)
                                    {
                                        menuSound.stop();
                                        // gaming moment
                                        bool continueGame = gaming(Play);
                                        if (!continueGame)
                                        {
                                            // no gaming
                                            break;
                                        }
                                    }
                                }
                            }
                            Options.close();
                            About.close();
                            Play.clear();
                            Play.draw(playBackground);
                            Play.draw(pressEnter);
                            Play.draw(lScore);
                            Play.draw(hScore);
                            Play.display();
                        }
                    }
                    // settings chosen
                    if (x == 1)
                    {

                        sensitivitySlider.setPosition(Core::lastSensitivitySliderPosition,
                                                      sensitivitySlider.getPosition().y);

                        float currentVolume = menuSound.getVolume();
                        float normalizedVolumePosition =
                            (currentVolume / 100.0f) * (volumeBar.getSize().x - volumeSlider.getRadius() * 2);
                        Core::lastVolumeSliderPosition = volumeBar.getPosition().x + normalizedVolumePosition;
                        volumeSlider.setPosition(Core::lastVolumeSliderPosition, volumeSlider.getPosition().y);

                        while (Options.isOpen())
                        {
                            // minimalize menu
                            ShowWindow(hwnd, SW_MINIMIZE);
                            sf::Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                {
                                    // bring back menu
                                    ShowWindow(hwnd, SW_NORMAL);
                                    Options.close();
                                }
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    // settings key events
                                    switch (aevent.key.code)
                                    {
                                    case sf::Keyboard::Escape:
                                        ShowWindow(hwnd, SW_NORMAL);
                                        Options.close();
                                        break;
                                    case sf::Keyboard::Up:
                                        selectedSlider = (selectedSlider - 1 + 3) % 3;
                                        break;
                                    case sf::Keyboard::Down:
                                        selectedSlider = (selectedSlider + 1) % 3;
                                        break;
                                    case sf::Keyboard::Left:
                                    case sf::Keyboard::Right:
                                        float moveAmount = (aevent.key.code == sf::Keyboard::Left) ? -10.0f : 10.0f;
                                        // sensitivity
                                        if (selectedSlider == 0)
                                        {
                                            sensitivitySlider.move(moveAmount, 0);
                                            float newPos = sensitivitySlider.getPosition().x;
                                            // update MOVE_LEFT and MOVE_RIGHT
                                            updateMoveValues(newPos);
                                            Core::lastSensitivitySliderPosition = newPos;
                                        }
                                        // volume
                                        else if (selectedSlider == 1)
                                        {
                                            volumeSlider.move(moveAmount, 0);
                                            float newPos = volumeSlider.getPosition().x;
                                            float volumePositionNormalized =
                                                (newPos - volumeBar.getPosition().x) /
                                                (volumeBar.getSize().x - volumeSlider.getRadius() * 2);
                                            float newVolume = volumePositionNormalized * 100; // [0, 100]
                                            // range
                                            newVolume = std::max(0.0f, std::min(100.0f, newVolume));
                                            menuSound.setVolume(newVolume);
                                            Core::globalVolumeLevel = newVolume;
                                            Core::lastVolumeSliderPosition = newPos;
                                        }
                                        // game difficulty
                                        else if (selectedSlider == 2)
                                        {
                                            if (aevent.key.code == sf::Keyboard::Left)
                                            {
                                                selectedDiff = (selectedDiff - 1 + 3) % 3;
                                                // easy
                                                if (selectedDiff == 0)
                                                {
                                                    Core::speedIncreasePerPoint = 1.0;
                                                    Core::maxSpeed = 100;
                                                }
                                                // normal
                                                else if (selectedDiff == 1)
                                                {
                                                    Core::speedIncreasePerPoint = 5.5;
                                                    Core::maxSpeed = 500;
                                                }
                                                // frog
                                                else
                                                {
                                                    Core::speedIncreasePerPoint = 100.0;
                                                    Core::maxSpeed = 10000;
                                                }
                                                Core::lastDifficulty = selectedDiff;
                                            }
                                            else if (aevent.key.code == sf::Keyboard::Right)
                                            {
                                                selectedDiff = (selectedDiff + 1) % 3;
                                                // easy
                                                if (selectedDiff == 0)
                                                {
                                                    Core::speedIncreasePerPoint = 1.0;
                                                    Core::maxSpeed = 100;
                                                }
                                                // normal
                                                else if (selectedDiff == 1)
                                                {
                                                    Core::speedIncreasePerPoint = 5.5;
                                                    Core::maxSpeed = 500;
                                                }
                                                // frog
                                                else
                                                {
                                                    Core::speedIncreasePerPoint = 100.0;
                                                    Core::maxSpeed = 10000;
                                                }
                                                Core::lastDifficulty = selectedDiff;
                                            }
                                        }
                                        break;
                                    }

                                    // sensitivity slider range
                                    if (sensitivitySlider.getPosition().x < sensitivityBar.getPosition().x)
                                    {
                                        sensitivitySlider.setPosition(sensitivityBar.getPosition().x,
                                                                      sensitivitySlider.getPosition().y);
                                    }
                                    else if (sensitivitySlider.getPosition().x > sensitivityBar.getPosition().x +
                                                                                     sensitivityBar.getSize().x -
                                                                                     sensitivitySlider.getRadius() * 2)
                                    {
                                        sensitivitySlider.setPosition(sensitivityBar.getPosition().x +
                                                                          sensitivityBar.getSize().x -
                                                                          sensitivitySlider.getRadius() * 2,
                                                                      sensitivitySlider.getPosition().y);
                                    }

                                    // volume slider range
                                    if (volumeSlider.getPosition().x < volumeBar.getPosition().x)
                                    {
                                        volumeSlider.setPosition(volumeBar.getPosition().x,
                                                                 volumeSlider.getPosition().y);
                                    }
                                    else if (volumeSlider.getPosition().x > volumeBar.getPosition().x +
                                                                                volumeBar.getSize().x -
                                                                                volumeSlider.getRadius() * 2)
                                    {
                                        volumeSlider.setPosition(volumeBar.getPosition().x + volumeBar.getSize().x -
                                                                     volumeSlider.getRadius() * 2,
                                                                 volumeSlider.getPosition().y);
                                    }

                                    // main option thickness
                                    sensitivityText.setOutlineThickness((selectedSlider == 0) ? 4.0f : 0);
                                    volumeText.setOutlineThickness((selectedSlider == 1) ? 4.0f : 0);
                                    gameDifficulty.setOutlineThickness((selectedSlider == 2) ? 4.0f : 0);

                                    // difficulty thickness
                                    d1.setOutlineThickness((selectedDiff == 0) ? 4.0f : 0);
                                    d2.setOutlineThickness((selectedDiff == 1) ? 4.0f : 0);
                                    d3.setOutlineThickness((selectedDiff == 2) ? 4.0f : 0);
                                }
                            }
                            Play.close();
                            About.close();
                            Options.clear();
                            Options.draw(settingsBackground);
                            Options.draw(sensitivityBar);
                            Options.draw(sensitivitySlider);
                            Options.draw(volumeBar);
                            Options.draw(volumeSlider);
                            Options.draw(sensitivityText);
                            Options.draw(volumeText);
                            Options.draw(gameDifficulty);
                            Options.draw(d1);
                            Options.draw(d2);
                            Options.draw(d3);
                            Options.display();
                        }
                    }
                    // about chosen
                    if (x == 2)
                    {
                        while (About.isOpen())
                        {
                            // minimalize menu
                            ShowWindow(hwnd, SW_MINIMIZE);
                            sf::Event aevent;
                            while (About.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                {
                                    // bring back menu
                                    ShowWindow(hwnd, SW_NORMAL);
                                    About.close();
                                }
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                    {
                                        // bring back menu
                                        ShowWindow(hwnd, SW_NORMAL);
                                        About.close();
                                    }
                                }
                            }
                            Play.close();
                            Options.close();
                            About.clear();
                            About.draw(aboutBackground);
                            About.display();
                        }
                    }
                    // quit game
                    if (x == 3)
                    {
                        MENU.close();
                        exit(0);
                    }
                    break;
                }
            }
        }
        MENU.clear();
        MENU.draw(background);
        mainMenu.draw(MENU);
        MENU.display();
    }
}

struct MenuContext
{
    sf::Sound menuSound;
    int lastScore;
    sf::RectangleShape playBackground;
    sf::Text pressEnter;
    sf::RectangleShape settingsBackground;
    sf::RectangleShape aboutBackground;
    sf::RectangleShape background;
    sf::Font font;
};

void runMenuLoop()
{
    MenuContext menuContext;

    // lobby music
    sf::SoundBuffer lobbySound;
    if (!lobbySound.loadFromFile("assets/lobbySnoop.wav"))
    {
        std::cerr << "ERROR::PLAYER::INITSOUND::Could not load sound file:assets/lobbySnoop.wav" << std::endl;
        ;
    }
    menuContext.menuSound.setBuffer(lobbySound);
    menuContext.menuSound.play();
    menuContext.menuSound.setLoop(true);

    sf::RenderWindow MENU;
    sf::Texture menuTexture;
    initializeMenuWindow(MENU, menuContext.background, menuTexture);

    sf::Texture playTexture;
    playTexture.loadFromFile("assets/road.png");
    menuContext.playBackground.setSize(sf::Vector2f(IG_SCREEN_WIDTH, IG_SCREEN_HEIGHT));
    menuContext.playBackground.setTexture(&playTexture);

    // Loading font
    if (!menuContext.font.loadFromFile("assets/font.ttf"))
    {
        std::cerr << "ERROR::GAMERESOURCES::LOADFONT::Could not load font file :assets/font.ttf" << std::endl;
    }

    //"press enter to start" string
    menuContext.pressEnter.setFont(menuContext.font);
    menuContext.pressEnter.setOutlineColor(sf::Color::Blue);
    menuContext.pressEnter.setOutlineThickness(4);
    menuContext.pressEnter.setString("  Press Enter to start\nor Esc to back to menu.");
    menuContext.pressEnter.setCharacterSize(100);
    menuContext.pressEnter.setPosition(100, 300);

    // settings window assets
    sf::RectangleShape settingsBackground;
    settingsBackground.setSize(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
    sf::Texture settingsTexture;
    settingsTexture.loadFromFile("assets/settings.png");
    settingsBackground.setTexture(&settingsTexture);

    // about window assets
    sf::RectangleShape aboutBackground;
    aboutBackground.setSize(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
    sf::Texture aboutTexture;
    aboutTexture.loadFromFile("assets/info.png");
    aboutBackground.setTexture(&aboutTexture);

    // menu loop
    MainMenu mainMenu(MENU_WIDTH, MENU_HEIGHT);
    handleMenuEvents(MENU, mainMenu, menuContext.menuSound, menuContext.lastScore, menuContext.playBackground,
                     menuContext.pressEnter, settingsBackground, aboutBackground, menuContext.font,
                     menuContext.background);
}

int main()
{
    // hide console while running
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, 0);

    // run menu
    runMenuLoop();
    return EXIT_SUCCESS;
}
