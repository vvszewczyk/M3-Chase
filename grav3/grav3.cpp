#include "grav3.h"

std::random_device rd;
std::mt19937 rng(rd());

bool GameResources::loadTexture(sf::Texture& texture, const std::string& filename)
{
    if (!texture.loadFromFile(filename))
    {
        std::cerr << "ERROR::GAMERESOURCES::LOADTEXTURE::Could not load texture file: " << filename << std::endl;
        return false;
    }
    return true;
}

bool GameResources::loadSound(sf::SoundBuffer& soundBuffer, const std::string& filename)
{
    if (!soundBuffer.loadFromFile(filename))
    {
        std::cerr << "ERROR::GAMERESOURCES::LOADSOUND::Could not load sound file: " << filename << std::endl;
        return false;
    }
    return true;
}

bool GameResources::loadFont(sf::Font& font, const std::string& filename)
{
    if (!font.loadFromFile(filename))
    {
        std::cerr << "ERROR::GAMERESOURCES::LOADFONT::Could not load font file: " << filename << std::endl;
        return false;
    }
    return true;
}

bool GameResources::loadAllResources()
{
    return
        loadTexture(background, "assets/road.png") &&
        loadTexture(racer, "assets/car.png") &&
        loadTexture(obs1, "assets/police1.png") &&
        loadTexture(obs2, "assets/police2.png") &&
        loadTexture(obs3, "assets/police3.png") &&
        loadTexture(obs4, "assets/amb1.png") &&
        loadTexture(crash1, "assets/BOOM.png") &&
        loadTexture(gameover, "assets/wasted.png") &&
        loadTexture(info, "assets/info.png") &&
        loadFont(myfont, "assets/font.ttf") &&
        loadSound(crash, "assets/crash.wav") &&
        loadSound(lobbySound, "assets/lobbySnoop.wav") &&
        loadSound(igSound, "assets/inGameMusic.wav");
}

int getRandomNumber(int a, int b)
{
    std::uniform_int_distribution<int> uni(a, b / 10 * 10);
    return uni(rng);
}

double gameSpeedLevel(int score, double speedIncreasePerPoint, double maxSpeed, double baseSpeed = 270)
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
    obsX = float(getRandomNumber(borderLeft, borderRight));
}

void initializeGameObjects(float& RacerX, float& RacerY, float& Obs1X, float& Obs1Y, float& Obs2X, float& Obs2Y, float& Obs3X, float& Obs3Y, float& Obs4X, float& Obs4Y) {
    RacerX = MIDDLE_POS;
    RacerY = IG_SCREEN_HEIGHT - RACER_HEIGHT - 110;

    Obs1X = float(getRandomNumber(borderLeft, borderRight));
    Obs2X = float(getRandomNumber(borderLeft, borderRight));
    Obs3X = float(getRandomNumber(borderLeft, borderRight));
    Obs4X = float(getRandomNumber(borderLeft, borderRight));

    Obs1Y = START_HEIGHT; Obs2Y = START_HEIGHT-500; Obs3Y = START_HEIGHT-1000; Obs4Y = START_HEIGHT-1500;
}

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

void updateMoveValues(float sliderPosition) 
{
    //in settings
    float normalizedPosition = (sliderPosition - sliderMin) / sliderWidth;
    float valueOffset = (normalizedPosition - 0.5f) * 300; //[-150, 150] (change 300 to rerange) relative to 260
    int currentValue = 260 + static_cast<int>(valueOffset);

    MOVE_LEFT = -currentValue;
    MOVE_RIGHT = currentValue;
}

bool gaming(sf::RenderWindow& gameWindow)
{
    
    bool game = true;
    gameWindow.setFramerateLimit(60);
    sf::Clock deltaClock;


    sf::SoundBuffer crash;
    if (!crash.loadFromFile("assets/crash.wav"))
    {
        std::cerr << "ERROR::PLAYER::INITSOUND::Could not load sound file";
    }
    sf::Sound GameCrash;
    GameCrash.setBuffer(crash);

    //loading in game music
    sf::SoundBuffer igSound;
    if (!igSound.loadFromFile("assets/inGameMusic.wav"))
    {
        std::cerr << "ERROR::PLAYER::INITSOUND::Could not load sound file";
    }

    //loading all assets
    GameResources resources;
    if (!resources.loadAllResources()) 
    {
        std::cerr << "Failed to load game resources." << std::endl;
        return EXIT_FAILURE;
    }

    //in game music
    sf::Sound inGameSound;
    inGameSound.setBuffer(igSound);
    inGameSound.setVolume(globalVolumeLevel);
    inGameSound.play();
    inGameSound.setLoop(true);

    //sprites
    sf::Sprite Background(resources.background),
        Background1(resources.background),
        Racer(resources.racer), 
        Obs1(resources.obs1),
        Obs2(resources.obs2), 
        Obs3(resources.obs3),
        Obs4(resources.obs4), 
        Explosion(resources.crash1), 
        Gameover(resources.gameover);

    float RacerX, RacerY, Obs1X, Obs1Y, Obs2X, Obs2Y, Obs3X, Obs3Y, Obs4X, Obs4Y;
    int score;
    float gameSpeed;

    //setting and initializing game objects
    initializeGameObjects(RacerX, RacerY, Obs1X, Obs1Y, Obs2X, Obs2Y, Obs3X, Obs3Y, Obs4X, Obs4Y);
    score = 0;
    gameSpeed = startGameSpeed;
    float BackgroundY1 = 0;
    float BackgroundY2 = -IG_SCREEN_HEIGHT;
    Racer.setRotation(90.f);
    Obs1.setRotation(90.f);
    Obs2.setRotation(90.f);
    Obs3.setRotation(90.f);
    Obs4.setRotation(90.f);

    highScore = readScore("highscore.txt");
    float acc = 0;
    const float drag = 0.8f;

    //to start delay
    sf::Clock obstacleResetClock;
    sf::Time obstacleResetTime = sf::seconds(1.0f);


    while (gameWindow.isOpen())
    {
        const auto deltaTime = deltaClock.restart().asSeconds();
        
        //in game score and highscore
        std::string stringscore = "";
        std::string stringscore2 = "";
        stringscore = std::to_string(score);
        stringscore2 = std::to_string(highScore);
        std::string textscore2 = "HIGH SCORE: " + stringscore2;
        std::string textscore = "SCORE: " + stringscore;
        sf::Text text2(textscore2, resources.myfont, IG_FONT_SIZE);
        sf::Text text(textscore, resources.myfont, IG_FONT_SIZE);
        text2.setOutlineColor(sf::Color::Blue);
        text2.setOutlineThickness(2.0f);
        text2.setPosition(5, 0);
        text.setPosition(5, 30);
        text.setOutlineColor(sf::Color::Blue);
        text.setOutlineThickness(2.0f);
        
        //new highscore
        writeScore((score > highScore) ? score : highScore, "highscore.txt");

        //last score
        writeScore(score, "lastscore.txt");
        
        //car and police positions
        Racer.setPosition(RacerX, RacerY);
        Obs1.setPosition(Obs1X, Obs1Y);
        Obs2.setPosition(Obs2X, Obs2Y);
        Obs3.setPosition(Obs3X, Obs3Y);
        Obs4.setPosition(Obs4X, Obs4Y);

        //explosion and gameover sprites
        Explosion.setPosition(-1000, -1000);
        Gameover.setPosition(-1000, -1000);

        //scrolling background (road)
        Background.setPosition(0, BackgroundY1);
        Background1.setPosition(0, BackgroundY2);
        if (BackgroundY2 > 0)
        {
            BackgroundY1 = 0;
            BackgroundY2 = BackgroundY1 - IG_SCREEN_HEIGHT;
        }
        BackgroundY1 += 500 * deltaTime;
        BackgroundY2 += 500 * deltaTime;

        //obstacles spawn
        if (obstacleResetClock.getElapsedTime() >= obstacleResetTime)
        {
            if (Obs1Y > IG_SCREEN_HEIGHT)
            {
                resetObstaclePosition(Obs1Y, Obs1X, IG_SCREEN_HEIGHT);
                score++;
            }
            else
            {
                Obs1Y = Obs1Y + gameSpeed * deltaTime * 2.5f;
            }

            if (Obs2Y > IG_SCREEN_HEIGHT)
            {
                resetObstaclePosition(Obs2Y, Obs2X, IG_SCREEN_HEIGHT);
                score++;
            }
            else
            {
                Obs2Y = Obs2Y + gameSpeed * deltaTime * 2.5f;
            }

            if (Obs3Y > IG_SCREEN_HEIGHT)
            {
                resetObstaclePosition(Obs3Y, Obs3X, IG_SCREEN_HEIGHT);
                score++;
            }
            else
            {
                Obs3Y = Obs3Y + gameSpeed * deltaTime * 2.5f;
            }

            if (Obs4Y > IG_SCREEN_HEIGHT)
            {
                resetObstaclePosition(Obs4Y, Obs4X, IG_SCREEN_HEIGHT);
                score++;
            }
            else
            {
                Obs4Y = Obs4Y + gameSpeed * deltaTime * 2.5f;
            }
        }

        //game difficulty
        gameSpeed = float(gameSpeedLevel(score,speedIncreasePerPoint, maxSpeed));

        //vehicle steering 
        sf::Event event;
        while (gameWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                gameWindow.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                {
                    acc += MOVE_LEFT;
                }

                if (event.key.code == sf::Keyboard::Right)
                {
                    acc += MOVE_RIGHT;
                }

            }
        }
        RacerX += acc * 5 * deltaTime;
        acc *= drag;

        //walls
        if (RacerX < borderLeft)
        {
            RacerX = float(borderLeft);
            acc = 0;
        }
        if (RacerX > borderRight)
        {
            RacerX = float(borderRight);
            acc = 0;
        }
        if (game == false)
        {
            //5 seconds delay after gameover
            std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(5));
            return game;
        }
        
        //collision positions
        float ObsX[] = { Obs1X, Obs2X, Obs3X, Obs4X };
        float ObsY[] = { Obs1Y, Obs2Y, Obs3Y, Obs4Y };
        int width[] = { 83, 73, 70, 73 };
        int height[] = { 144, 135, 135, 135 };

        //check for collision
        for (int i = 0; i < 4; i++) 
        {
            if (((RacerX >= (ObsX[i] - width[i])) && (RacerX <= (ObsX[i] + width[i]))) && ((RacerY-25 >= (ObsY[i] - height[i])) && (RacerY-25 <= (ObsY[i] + height[i])))) 
            {
                Explosion.setPosition(RacerX - 125, RacerY);
                Gameover.setScale(sf::Vector2f(0.5, 0.5));
                Gameover.setPosition(290, 320);
                inGameSound.stop();
                GameCrash.setVolume(globalVolumeLevel);
                GameCrash.play();
                game = false;
                break;
            }
        }

        gameWindow.clear();
        gameWindow.draw(Background);
        gameWindow.draw(Background1);
        gameWindow.draw(Racer);
        gameWindow.draw(Obs1);
        gameWindow.draw(Obs2);
        gameWindow.draw(Obs3);
        gameWindow.draw(Obs4);
        gameWindow.draw(Explosion);
        gameWindow.draw(Gameover);
        gameWindow.draw(text);
        gameWindow.draw(text2);
        gameWindow.display();
    }
    return false;
}

void makeMenu()
{
    //lobby music
    sf::SoundBuffer lobbySound;
    if (!lobbySound.loadFromFile("assets/lobbySnoop.wav"))
    {
        std::cerr << "ERROR::PLAYER::INITSOUND::Could not load sound file:assets/lobbySnoop.wav" << std::endl;;
    }
    sf::Sound menuSound;
    menuSound.setBuffer(lobbySound);
    menuSound.play();
    menuSound.setLoop(true);

    //menu window
    sf::RenderWindow MENU(sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), "M3 Chase", sf::Style::Titlebar | sf::Style::Close);
    MENU.setPosition(sf::Vector2i(160, 0));
    MainMenu mainMenu(float(MENU.getSize().x), float(MENU.getSize().y));
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
    sf::Texture menuTexture;
    menuTexture.loadFromFile("assets/menu.png");
    background.setTexture(&menuTexture);


    //play window assets
    sf::RectangleShape playBackground;
    playBackground.setSize(sf::Vector2f(IG_SCREEN_WIDTH, IG_SCREEN_HEIGHT));
    sf::Texture playTexture;
    playTexture.loadFromFile("assets/road.png");
    playBackground.setTexture(&playTexture);

    //font loading
    sf::Font font;
    if (!font.loadFromFile("assets/font.ttf"))
    {
        std::cerr<<"ERROR::GAMERESOURCES::LOADFONT::Could not load font file :assets/font.ttf"<<std::endl;
    }

    //"press enter to start" string
    sf::Text pressEnter;
    pressEnter.setFont(font);
    pressEnter.setOutlineColor(sf::Color::Blue);
    pressEnter.setOutlineThickness(4);
    pressEnter.setString("  Press Enter to start\nor Esc to back to menu.");
    pressEnter.setCharacterSize(100);
    pressEnter.setPosition(100, 300);


    int lastScore;


    //settings window assets
    sf::RectangleShape settingsBackground;
    settingsBackground.setSize(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
    sf::Texture settingsTexture;
    settingsTexture.loadFromFile("assets/settings.png");
    settingsBackground.setTexture(&settingsTexture);

    //about window assets
    sf::RectangleShape aboutBackground;
    aboutBackground.setSize(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
    sf::Texture aboutTexture;
    aboutTexture.loadFromFile("assets/info.png");
    aboutBackground.setTexture(&aboutTexture);

    //menu loop
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
                    //option windows
                    sf::RenderWindow Play(sf::VideoMode(IG_SCREEN_WIDTH, IG_SCREEN_HEIGHT), "M3 Chase", sf::Style::Titlebar | sf::Style::Close);
                    sf::RenderWindow Options(sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), "M3 Chase", sf::Style::Titlebar | sf::Style::Close);
                    sf::RenderWindow About(sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), "M3 Chase", sf::Style::Titlebar | sf::Style::Close);
                    Play.setPosition(sf::Vector2i(460, 120));
                    Options.setPosition(sf::Vector2i(160, 0));
                    About.setPosition(sf::Vector2i(160, 0));


                    //sensitivity
                    sf::Text sensitivityText("Steering sensitivity", font, OPT_FONT_SIZE);
                    sensitivityText.setPosition(320, 150);
                    sensitivityText.setOutlineColor(sf::Color::Blue);

                    sf::CircleShape sensitivitySlider(15);
                    sensitivitySlider.setPosition(initialSliderPosition, 190);
                    sensitivitySlider.setFillColor(sf::Color::Blue);

                    sf::RectangleShape sensitivityBar(sf::Vector2f(300, 10));
                    sensitivityBar.setPosition(800, 200);
                    sensitivityBar.setFillColor(sf::Color::White);


                    //Volume
                    sf::Text volumeText("Volume", font, OPT_FONT_SIZE);
                    volumeText.setPosition(590, 260);
                    volumeText.setOutlineColor(sf::Color::Blue);

                    sf::CircleShape volumeSlider(15);
                    float volumeSliderInitialX = volumeBarX + (volumeBarWidth / 2.0f) - (volumeSlider.getRadius());
                    volumeSlider.setFillColor(sf::Color::Blue);

                    sf::RectangleShape volumeBar(sf::Vector2f(300, 10));
                    volumeBar.setPosition(800, 313);
                    volumeBar.setFillColor(sf::Color::White);
                    volumeSlider.setPosition(volumeSliderInitialX, 303);
                    lastVolumeSliderPosition = volumeSliderInitialX;


                    //game difficulty levels
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
                    
                    //settings selected option and difficulty position
                    int selectedSlider = 0;
                    int selectedDiff = lastDifficulty;

                    sensitivityText.setOutlineThickness((selectedSlider == 0) ? 4.0f : 0);
                    d1.setOutlineThickness((selectedDiff == 0) ? 4.0f : 0);
                    d2.setOutlineThickness((selectedDiff == 1) ? 4.0f : 0);
                    d3.setOutlineThickness((selectedDiff == 2) ? 4.0f : 0);
                
                    int x = mainMenu.mainMenuPressed();

                    //play chosen
                    if (x == 0)
                    {
                        menuSound.stop();
                        while (Play.isOpen())
                        {
                            //minimalize menu
                            ShowWindow(hwnd, SW_MINIMIZE);

                            //read scores
                            lastScore = readScore("lastscore.txt");
                            highScore = readScore("highscore.txt");
                            sf::Text lScore("LAST SCORE: " + std::to_string(lastScore), font, IG_FONT_SIZE);
                            sf::Text hScore("HIGH SCORE: " + std::to_string(highScore), font, IG_FONT_SIZE);
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

                                    //bring back menu
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
                                        //gaming moment
                                        bool continueGame = gaming(Play);
                                        if (!continueGame)
                                        {
                                            //no gaming
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
                    //settings chosen
                    if (x == 1)
                    {

                        sensitivitySlider.setPosition(lastSensitivitySliderPosition, sensitivitySlider.getPosition().y);

                        float currentVolume = menuSound.getVolume();
                        float normalizedVolumePosition = (currentVolume / 100.0f) * (volumeBar.getSize().x - volumeSlider.getRadius() * 2);
                        lastVolumeSliderPosition = volumeBar.getPosition().x + normalizedVolumePosition;
                        volumeSlider.setPosition(lastVolumeSliderPosition, volumeSlider.getPosition().y);

                        while (Options.isOpen())
                        {
                            //minimalize menu
                            ShowWindow(hwnd, SW_MINIMIZE);
                            sf::Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                {
                                    //bring back menu
                                    ShowWindow(hwnd, SW_NORMAL);
                                    Options.close();
                                }
                                if (aevent.type == sf::Event::KeyPressed) 
                                {
                                    //settings key events
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
                                        //sensitivity
                                        if (selectedSlider == 0) 
                                        {
                                            sensitivitySlider.move(moveAmount, 0);
                                            float newPos = sensitivitySlider.getPosition().x;
                                            //update MOVE_LEFT and MOVE_RIGHT
                                            updateMoveValues(newPos); 
                                            lastSensitivitySliderPosition = newPos;
                                        }
                                        //volume
                                        else if(selectedSlider == 1)
                                        {
                                            volumeSlider.move(moveAmount, 0);
                                            float newPos = volumeSlider.getPosition().x;
                                            float volumePositionNormalized = (newPos - volumeBar.getPosition().x) / (volumeBar.getSize().x - volumeSlider.getRadius() * 2);
                                            float newVolume = volumePositionNormalized * 100; // [0, 100]
                                            //range
                                            newVolume = std::max(0.0f, std::min(100.0f, newVolume));
                                            menuSound.setVolume(newVolume);
                                            globalVolumeLevel = newVolume;
                                            lastVolumeSliderPosition = newPos;
                                        }
                                        //game difficulty
                                        else if(selectedSlider == 2)
                                        {
                                            if (aevent.key.code == sf::Keyboard::Left)
                                            {
                                                selectedDiff = (selectedDiff - 1 + 3) % 3;
                                                //easy
                                                if (selectedDiff == 0)
                                                {
                                                    speedIncreasePerPoint = 1.0;
                                                    maxSpeed = 100;
                                                }
                                                //normal
                                                else if (selectedDiff == 1)
                                                {
                                                    speedIncreasePerPoint = 5.5;
                                                    maxSpeed = 500;
                                                }
                                                //frog
                                                else
                                                {
                                                    speedIncreasePerPoint = 100.0;
                                                    maxSpeed = 10000;
                                                }
                                                lastDifficulty = selectedDiff;
                                            }
                                            else if (aevent.key.code == sf::Keyboard::Right)
                                            {
                                                selectedDiff = (selectedDiff + 1) % 3;
                                                //easy
                                                if (selectedDiff == 0)
                                                {
                                                    speedIncreasePerPoint = 1.0;
                                                    maxSpeed = 100;
                                                }
                                                //normal
                                                else if (selectedDiff == 1)
                                                {
                                                    speedIncreasePerPoint = 5.5;
                                                    maxSpeed = 500;
                                                }
                                                //frog
                                                else
                                                {
                                                    speedIncreasePerPoint = 100.0;
                                                    maxSpeed = 10000;
                                                }
                                                lastDifficulty = selectedDiff;
                                            }
                                        }
                                        break;
                                    }

                                    //sensitivity slider range
                                    if (sensitivitySlider.getPosition().x < sensitivityBar.getPosition().x) 
                                    {
                                        sensitivitySlider.setPosition(sensitivityBar.getPosition().x, sensitivitySlider.getPosition().y);
                                    }
                                    else if (sensitivitySlider.getPosition().x > sensitivityBar.getPosition().x + sensitivityBar.getSize().x - sensitivitySlider.getRadius() * 2) 
                                    {
                                        sensitivitySlider.setPosition(sensitivityBar.getPosition().x + sensitivityBar.getSize().x - sensitivitySlider.getRadius() * 2, sensitivitySlider.getPosition().y);
                                    }


                                    //volume slider range
                                    if (volumeSlider.getPosition().x < volumeBar.getPosition().x) 
                                    {
                                        volumeSlider.setPosition(volumeBar.getPosition().x, volumeSlider.getPosition().y);
                                    }
                                    else if (volumeSlider.getPosition().x > volumeBar.getPosition().x + volumeBar.getSize().x - volumeSlider.getRadius() * 2) 
                                    {
                                        volumeSlider.setPosition(volumeBar.getPosition().x + volumeBar.getSize().x - volumeSlider.getRadius() * 2, volumeSlider.getPosition().y);
                                    }

                                    
                                    //main option thickness
                                    sensitivityText.setOutlineThickness((selectedSlider == 0) ? 4.0f : 0);
                                    volumeText.setOutlineThickness((selectedSlider == 1) ? 4.0f : 0);
                                    gameDifficulty.setOutlineThickness((selectedSlider == 2) ? 4.0f : 0);

                                    //difficulty thickness
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
                    //about chosen
                    if (x == 2)
                    {
                        while (About.isOpen())
                        {
                            //minimalize menu
                            ShowWindow(hwnd, SW_MINIMIZE);
                            sf::Event aevent;
                            while (About.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                {
                                    //bring back menu
                                    ShowWindow(hwnd, SW_NORMAL);
                                    About.close();
                                }
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                    {
                                        //bring back menu
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
                    //quit game
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

int main()
{
    //hide console while running
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, 0);

    //run menu
    makeMenu();
    return EXIT_SUCCESS;
}