//
// Created by Alyssa Wang on 2025/11/12.
//

#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"
#include <vector>
#include <SFML/Graphics.hpp>

class TextureManager;

class Board {
public:
    Board();
    void Initialize(int cols, int rows, int mines, TextureManager& textures);
    void Restart(TextureManager& textures);
    void Draw(sf::RenderWindow& window, bool paused, TextureManager& textures);
    void SetupNeighbors();
    void PlaceMines(int mineCount);
    void CalculateAdjacentMines();

    void LeftClickTile(float x, float y);
    void RightClickTile(float x, float y);
    void RevealEmptyTiles(Tile* tile);

    void ToggleDebugMode();

    Tile* GetTile(int col, int row);
    int GetTotalMines() const { return totalMines; }

    enum GameState { PLAYING, WIN, LOSE };
    GameState currentState = PLAYING;
    int flagsPlaced = 0;
    bool leaderboardShown = false;

private:
    int columns = 0;
    int rows = 0;
    int totalMines = 0;
    int tilesRevealed = 0;
    int totalTiles = 0;
    bool debugMode = false;

    std::vector<std::vector<Tile>> grid;

    bool GetTileIndices(float x, float y, int& col, int& row);
    std::vector<Tile*> GetNeighbors(int col, int row);
};

#endif