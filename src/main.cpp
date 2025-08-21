// Copyright 2025
// By Nguyen Mai

#include <functional>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Sokoban.hpp"

#define DELAY 5.0f

void getMovementInput(
    SB::Sokoban& game,
    sf::Text& moveCounterText,
    sf::Window& window,
    bool& keyPressed,
    sf::Keyboard::Key key,
    const std::unordered_map<sf::Keyboard::Key, SB::Direction>& keyStates
);

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " level_file.lvl" << "[seed]" <<
        std::endl;
        return 1;
    }

    unsigned int input_seed;
    if (argc != 3) {
        input_seed = 0;
    } else {
        input_seed = std::stoi(argv[2]);
    }
    std::shared_ptr<unsigned int> seed = std::make_shared<unsigned int>(input_seed);
    std::string level_file = argv[1];
    SB::Sokoban game(seed);

    std::ifstream ifs(level_file, std::ifstream::in);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open " + level_file);
    }
    ifs >> game;

    sf::RenderWindow window(sf::VideoMode(game.pixelWidth(),
                                          game.pixelHeight()),
                                          "Sokoban!",
                                          sf::Style::Titlebar);

    // set up font
    sf::Font font;
    if (!font.loadFromFile("./sokoban/Fonts/3270NerdFontRegular.ttf")) {
        if (!font.loadFromFile("./3270NerdFontRegular.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
    }

    // set up move counter text
    sf::Text moveCounterText;
    moveCounterText.setFont(font);
    moveCounterText.setCharacterSize(24);
    moveCounterText.setFillColor(sf::Color::White);
    moveCounterText.setPosition(10, 10);
    moveCounterText.setString("Moves: 0");

    // set up win message text
    sf::Text winText;
    winText.setFont(font);
    winText.setCharacterSize(48);
    winText.setFillColor(sf::Color::Green);
    winText.setString("You Win!");

    // set up timer text
    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::Green);

    // set up time elapsed text
    sf::Text elapsedText;
    elapsedText.setFont(font);
    elapsedText.setCharacterSize(24);
    elapsedText.setFillColor(sf::Color::Green);

    // set up win sound
    sf::SoundBuffer winSoundBuffer;
    if (!winSoundBuffer.loadFromFile("./2001.wav")) {
        if (!winSoundBuffer.loadFromFile("./sokoban/Sounds/2001.wav")) {
            throw std::runtime_error("Failed to load win sound");
        }
    }
    sf::Sound winSound;
    winSound.setBuffer(winSoundBuffer);

    bool keyPressed = false;
    bool winMessage = false;
    float nextLevelTimer = DELAY;
    float timeToBeat = 0;
    sf::Clock winClock;
    sf::Clock elapsedClock;

    const std::unordered_map<sf::Keyboard::Key, SB::Direction> movementKeyStates {
        {sf::Keyboard::W,       SB::Direction::Up},
        {sf::Keyboard::Up,      SB::Direction::Up},
        {sf::Keyboard::A,       SB::Direction::Left},
        {sf::Keyboard::Left,    SB::Direction::Left},
        {sf::Keyboard::S,       SB::Direction::Down},
        {sf::Keyboard::Down,    SB::Direction::Down},
        {sf::Keyboard::D,       SB::Direction::Right},
        {sf::Keyboard::Right,   SB::Direction::Right}
    };

    const std::unordered_map<sf::Keyboard::Key, std::function<void()>> gameKeyStates {
        {sf::Keyboard::R, [&]() {
            game.reset();
            moveCounterText.setString("Moves: 0");
            keyPressed = true;
            winMessage = false;
            winSound.stop();
            elapsedClock.restart();
        }},
        {sf::Keyboard::U, [&]() {
            game.undo();
            moveCounterText.setString("Moves: " + std::to_string(game.getMoveCount()));
        }},
        {sf::Keyboard::Y, [&]() {
            game.redo();
            moveCounterText.setString("Moves: " + std::to_string(game.getMoveCount()));
        }},
        {sf::Keyboard::Escape, [&]() {
            window.close();
        }}
    };

    std::vector<std::string> levels = {
        "./sokoban/Levels/level1.lvl", "./sokoban/Levels/level2.lvl",
        "./sokoban/Levels/level3.lvl", "./sokoban/Levels/level4.lvl",
        "./sokoban/Levels/level5.lvl", "./sokoban/Levels/level6.lvl"
    };

    unsigned int level = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                game.reset();
                moveCounterText.setString("Moves: 0");
                keyPressed = true;
                winMessage = false;
                nextLevelTimer = DELAY;
                winSound.stop();
                winClock.restart();
                elapsedClock.restart();
                continue;
            }
            if (event.type == sf::Event::KeyReleased) {
                keyPressed = false;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            if (!game.isWon()) {
                // get key pressed
                if (event.type == sf::Event::KeyPressed) {
                    auto itGame = gameKeyStates.find(event.key.code);
                    auto itMovement = movementKeyStates.find(event.key.code);
                    // if key is not in either map, continue
                    if (itGame == gameKeyStates.end() &&
                        itMovement == movementKeyStates.end()) {
                        continue;
                        // if key is in gameKeyStates, then access its value (function)
                    } else if (itGame != gameKeyStates.end()) {
                        itGame->second();
                        continue;
                        // if key is in movementKeyStates, then pass it to getMovementInput
                    } else {
                        getMovementInput(game, moveCounterText, window, keyPressed,
                                 event.key.code, movementKeyStates);
                    }
                    std::cout << game;
                }
                // reset key pressed
                if (event.type == sf::Event::KeyReleased) {
                    keyPressed = false;
                }
            } else if (!winMessage) {
                // player won
                timeToBeat = elapsedClock.getElapsedTime().asSeconds();
                winMessage = true;
                nextLevelTimer = DELAY;
                winClock.restart();
                winSound.play();

                sf::FloatRect winTextBounds = winText.getLocalBounds();
                winText.setOrigin(winTextBounds.width / 2, winTextBounds.height / 2);
                winText.setPosition(window.getSize().x / 2, window.getSize().y / 2 - 30);
            }
        }

        if (game.isWon() && winMessage) {
            float elapsed = winClock.getElapsedTime().asSeconds();
            nextLevelTimer -= elapsed;
            winClock.restart();

            if (nextLevelTimer <= 0) {
                level++;
                if (level < levels.size()) {
                    elapsedClock.restart();
                    winSound.stop();
                    level_file = levels[level];
                    std::ifstream ifs(level_file, std::ifstream::in);
                    if (!ifs.is_open()) {
                        throw std::runtime_error("Failed to open " + level_file);
                    }
                    ifs >> game;

                    // close and reopen with new level's dimensions
                    window.close();
                    window.create(sf::VideoMode(game.pixelWidth(),
                                              game.pixelHeight()),
                                              "Sokoban!",
                                              sf::Style::Titlebar);
                    moveCounterText.setString("Moves: 0");
                    winMessage = false;
                } else {
                    window.close();
                }
            }
            elapsedText.setString("Time to beat: " +
                std::to_string(static_cast<int>(timeToBeat)) + "s");
            sf::FloatRect elapsedTextBounds = elapsedText.getLocalBounds();
            elapsedText.setOrigin(elapsedTextBounds.width / 2, elapsedTextBounds.height / 2);
            elapsedText.setPosition(window.getSize().x / 2, window.getSize().y / 2 + 30);

            timerText.setString("Next level in: " +
                std::to_string(static_cast<int>(nextLevelTimer) + 1) + "s");
            sf::FloatRect timerTextBounds = timerText.getLocalBounds();
            timerText.setOrigin(timerTextBounds.width / 2, timerTextBounds.height / 2);
            timerText.setPosition(window.getSize().x / 2, window.getSize().y / 2 + 90);
        }

        window.clear();
        window.draw(game);
        window.draw(moveCounterText);

        // IF PLAYER WON
        if (winMessage) {
            window.draw(winText);
            window.draw(elapsedText);
            window.draw(timerText);
        }
        window.display();
    }
    return 0;
}

void getMovementInput(
    SB::Sokoban& game,
    sf::Text& moveCounterText,
    sf::Window& window,
    bool& keyPressed,
    sf::Keyboard::Key key,
    const std::unordered_map<sf::Keyboard::Key, SB::Direction>& keyStates
) {
    auto checkKeyPressed = [&]() {
        if (sf::Keyboard::isKeyPressed(key) && !keyPressed) {
            game.movePlayer(keyStates.at(key));
            moveCounterText.setString("Moves: " + std::to_string(game.getMoveCount()));
            keyPressed = true;
        }
        return;
    };

    checkKeyPressed();
}
