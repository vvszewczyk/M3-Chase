#include "GameEngine.h"
#include "GameLogic.h"
#include "GameIO.h"

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
        gameSpeed = float(gameSpeedLevel(score, speedIncreasePerPoint, maxSpeed));

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
            if (((RacerX >= (ObsX[i] - width[i])) && (RacerX <= (ObsX[i] + width[i]))) && ((RacerY - 25 >= (ObsY[i] - height[i])) && (RacerY - 25 <= (ObsY[i] + height[i]))))
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