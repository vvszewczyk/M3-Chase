#include "GameCore.h"

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