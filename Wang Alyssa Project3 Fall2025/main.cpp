#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <chrono>
#include <optional>
#include "Board.h"
#include "TextureManager.h"
#include "Leaderboard.h"

void ReadConfig(int& columns, int& rows, int& mines) {
    std::fstream file("files/config.cfg");
    if (file.is_open()) {
        file >> columns;
        file >> rows;
        file >> mines;
        file.close();
    } else {
        std::cerr << "Error: Could not open config.cfg file!" << std::endl;
        columns = 25;
        rows = 16;
        mines = 50;
    }
}

void setText(sf::Text& text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
    text.setPosition({x, y});
}

void LoadAllTextures(TextureManager& textures) {
    textures.LoadTexture("tile_hidden.png");
    textures.LoadTexture("tile_revealed.png");
    textures.LoadTexture("mine.png");
    textures.LoadTexture("flag.png");
    textures.LoadTexture("face_happy.png");
    textures.LoadTexture("face_win.png");
    textures.LoadTexture("face_lose.png");
    textures.LoadTexture("debug.png");
    textures.LoadTexture("pause.png");
    textures.LoadTexture("play.png");
    textures.LoadTexture("leaderboard.png");
    for (int i = 1; i <= 8; ++i) {
        textures.LoadTexture("number_" + std::to_string(i) + ".png");
    }
    textures.LoadTexture("digits.png");
}

void DrawDigits(sf::RenderWindow& window, int number, float xOffset, int maxDigits, int rows, TextureManager& textures) {
    sf::Sprite digitSprite(textures.GetTexture("digits.png"));

    std::string s = std::to_string(std::abs(number));
    while (s.length() < maxDigits) {
        s.insert(0, "0");
    }
    s = s.substr(s.length() - maxDigits);

    float yPos = (float)rows * 32.0f + 16.0f;

    if (number < 0) {
        sf::IntRect rect({210, 0}, {21, 32});
        digitSprite.setTextureRect(rect);
        digitSprite.setPosition({xOffset, yPos});
        window.draw(digitSprite);
        xOffset += 21;
    }

    for (char digitChar : s) {
        int digit = digitChar - '0';
        sf::IntRect rect({digit * 21, 0}, {21, 32});
        digitSprite.setTextureRect(rect);
        digitSprite.setPosition({xOffset, yPos});
        xOffset += 21;
        window.draw(digitSprite);
    }
}

int main() {
    int columns = 0;
    int rows = 0;
    int mineCount = 0;
    ReadConfig(columns, rows, mineCount);

    unsigned int width = columns * 32;
    unsigned int height = rows * 32 + 100;

    sf::RenderWindow window(sf::VideoMode({width, height}), "Minesweeper");
    window.setFramerateLimit(60);

    TextureManager textureManager;
    LoadAllTextures(textureManager);

    sf::Font font;
    if (!font.openFromFile("files/font.ttf")) {
        std::cerr << "Error: Could not load font.ttf!" << std::endl;
        return 1;
    }

    sf::Text welcomeText(font);
    welcomeText.setString("WELCOME TO MINESWEEPER!");
    welcomeText.setCharacterSize(24);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcomeText, width / 2.0f, (float)height / 2.0f - 150);

    sf::Text promptText(font);
    promptText.setString("Enter your name:");
    promptText.setCharacterSize(20);
    promptText.setFillColor(sf::Color::White);
    promptText.setStyle(sf::Text::Bold);
    setText(promptText, width / 2.0f, (float)height / 2.0f - 75);

    std::string nameInput;
    sf::Text inputText(font);
    inputText.setString("|");
    inputText.setCharacterSize(18);
    inputText.setFillColor(sf::Color::Yellow);
    inputText.setStyle(sf::Text::Bold);
    setText(inputText, width / 2.0f, (float)height / 2.0f - 45);

    bool welcomeScreen = true;
    std::string playerName;

    Board gameBoard;
    gameBoard.Initialize(columns, rows, mineCount, textureManager);

    Leaderboard leaderboard;
    sf::RenderWindow leaderboardWindow;
    bool leaderboardOpen = false;

    auto startTime = std::chrono::high_resolution_clock::now();
    long long timeElapsed = 0;
    bool timeStopped = true;
    bool wasPausedBeforeLeaderboard = false;

    sf::Sprite happyFace(textureManager.GetTexture("face_happy.png"));
    happyFace.setPosition({width / 2.0f - 32.0f, (float)rows * 32.0f + 16.0f});

    sf::Sprite debugButton(textureManager.GetTexture("debug.png"));
    debugButton.setPosition({width - 304.0f, (float)rows * 32.0f + 16.0f});

    sf::Sprite pausePlayButton(textureManager.GetTexture("pause.png"));
    pausePlayButton.setPosition({width - 240.0f, (float)rows * 32.0f + 16.0f});

    sf::Sprite leaderboardButton(textureManager.GetTexture("leaderboard.png"));
    leaderboardButton.setPosition({width - 176.0f, (float)rows * 32.0f + 16.0f});

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (welcomeScreen) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    char32_t unicode = textEvent->unicode;
                    if (unicode == 8) {
                        if (!nameInput.empty()) nameInput.pop_back();
                    }
                    else if (unicode < 128 && std::isalpha(static_cast<char>(unicode))) {
                        if (nameInput.length() < 10) {
                            char c = static_cast<char>(unicode);
                            if (nameInput.empty()) nameInput += std::toupper(c);
                            else nameInput += std::tolower(c);
                        }
                    }
                    inputText.setString(nameInput.empty() ? "|" : nameInput + "|");
                    setText(inputText, width / 2.0f, (float)height / 2.0f - 45);
                }
                else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyEvent->code == sf::Keyboard::Key::Enter) {
                        if (!nameInput.empty()) {
                            welcomeScreen = false;
                            playerName = nameInput;
                            timeStopped = false;
                            startTime = std::chrono::high_resolution_clock::now();
                        }
                    }
                }
            } else {
                if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

                    bool clickedHappyFace = happyFace.getGlobalBounds().contains(mousePos);
                    bool clickedDebug = debugButton.getGlobalBounds().contains(mousePos);
                    bool clickedPause = pausePlayButton.getGlobalBounds().contains(mousePos);
                    bool clickedLeaderboard = leaderboardButton.getGlobalBounds().contains(mousePos);

                    if (clickedHappyFace) {
                        gameBoard.Restart(textureManager);
                        timeStopped = false;
                        startTime = std::chrono::high_resolution_clock::now();
                        pausePlayButton.setTexture(textureManager.GetTexture("pause.png"));
                    }
                    else if (gameBoard.currentState == Board::PLAYING) {
                        if (clickedDebug && !timeStopped) {
                            gameBoard.ToggleDebugMode();
                        }
                        else if (clickedPause) {
                            timeStopped = !timeStopped;
                            if (timeStopped) {
                                pausePlayButton.setTexture(textureManager.GetTexture("play.png"));
                            } else {
                                pausePlayButton.setTexture(textureManager.GetTexture("pause.png"));
                                startTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(timeElapsed);
                            }
                        }
                        else if (clickedLeaderboard) {
                            wasPausedBeforeLeaderboard = timeStopped;
                            timeStopped = true;
                            leaderboardOpen = true;
                            leaderboardWindow.create(sf::VideoMode({width / 2, height / 2}), "Minesweeper", sf::Style::Titlebar | sf::Style::Close);
                            sf::Vector2i pos = window.getPosition();
                            leaderboardWindow.setPosition({pos.x + (int)width / 4, pos.y + (int)height / 4});
                        }
                        else if (mousePos.y < (float)rows * 32.0f && !timeStopped && !leaderboardOpen) {
                            if (mouseEvent->button == sf::Mouse::Button::Left) {
                                gameBoard.LeftClickTile(mousePos.x, mousePos.y);
                            }
                            else if (mouseEvent->button == sf::Mouse::Button::Right) {
                                gameBoard.RightClickTile(mousePos.x, mousePos.y);
                            }
                        }
                    }
                    else if (clickedLeaderboard) {
                         leaderboardOpen = true;
                         leaderboardWindow.create(sf::VideoMode({width / 2, height / 2}), "Minesweeper", sf::Style::Titlebar | sf::Style::Close);
                         sf::Vector2i pos = window.getPosition();
                         leaderboardWindow.setPosition({pos.x + (int)width / 4, pos.y + (int)height / 4});
                    }
                }
            }
        }

        if (leaderboardOpen) {
            while (const std::optional lbEvent = leaderboardWindow.pollEvent()) {
                if (lbEvent->is<sf::Event::Closed>()) {
                    leaderboardOpen = false;
                    leaderboardWindow.close();

                    if (gameBoard.currentState == Board::PLAYING && !wasPausedBeforeLeaderboard) {
                        timeStopped = false;
                        startTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(timeElapsed);
                        pausePlayButton.setTexture(textureManager.GetTexture("pause.png"));
                    }
                }
            }
        }

        if (!timeStopped && gameBoard.currentState == Board::PLAYING) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (timeElapsed > 99 * 60 + 59) {
                timeElapsed = 99 * 60 + 59;
            }
        }

        if (gameBoard.currentState == Board::WIN && !leaderboardOpen && !gameBoard.leaderboardShown) {
             gameBoard.leaderboardShown = true;
             timeStopped = true;
             leaderboard.UpdateLeaderboard(playerName, timeElapsed);
             leaderboardOpen = true;
             leaderboardWindow.create(sf::VideoMode({width / 2, height / 2}), "Minesweeper", sf::Style::Titlebar | sf::Style::Close);
             sf::Vector2i pos = window.getPosition();
             leaderboardWindow.setPosition({pos.x + (int)width / 4, pos.y + (int)height / 4});
        }

        if (welcomeScreen) {
            window.clear(sf::Color::Blue);
            window.draw(welcomeText);
            window.draw(promptText);
            window.draw(inputText);
        } else {
            window.clear(sf::Color::White);

            bool forceRevealed = (leaderboardOpen || (timeStopped && gameBoard.currentState == Board::PLAYING));
            gameBoard.Draw(window, forceRevealed, textureManager);

            sf::Sprite currentFace = happyFace;
            if (gameBoard.currentState == Board::WIN) {
                currentFace.setTexture(textureManager.GetTexture("face_win.png"));
            } else if (gameBoard.currentState == Board::LOSE) {
                currentFace.setTexture(textureManager.GetTexture("face_lose.png"));
            }
            window.draw(currentFace);
            window.draw(debugButton);
            window.draw(pausePlayButton);
            window.draw(leaderboardButton);

            int mineCounter = gameBoard.GetTotalMines() - gameBoard.flagsPlaced;
            DrawDigits(window, mineCounter, 33.0f, 3, rows, textureManager);

            int minutes = (int)(timeElapsed / 60);
            int seconds = (int)(timeElapsed % 60);
            DrawDigits(window, minutes, width - 97.0f, 2, rows, textureManager);
            DrawDigits(window, seconds, width - 54.0f, 2, rows, textureManager);

            if (leaderboardOpen) {
                leaderboardWindow.clear(sf::Color::Blue);
                sf::Text leaderboardContent(font);
                leaderboardContent.setString(leaderboard.GetFormattedContent());
                leaderboardContent.setCharacterSize(18);
                leaderboardContent.setFillColor(sf::Color::White);
                leaderboardContent.setStyle(sf::Text::Bold);

                setText(leaderboardContent, width / 4.0f, height / 4.0f);

                leaderboardWindow.draw(leaderboardContent);
                leaderboardWindow.display();
            }
        }
        window.display();
    }

    return 0;
}