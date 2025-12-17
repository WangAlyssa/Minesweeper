//
// Created by Alyssa Wang on 2025/11/12.
//

#ifndef MINESWEEPER_TILE_H
#define MINESWEEPER_TILE_H
#include <SFML/Graphics.hpp>
#include <vector>

class TextureManager;

class Tile {
public:
    Tile(int x, int y, TextureManager& textures);

    bool isMine = false;
    bool isRevealed = false;
    bool hasFlag = false;

    std::vector<Tile*> adjacentTiles;
    int adjacentMines = 0;

    sf::Sprite sprite;

    void SetPosition(float x, float y);
    void UpdateTexture(TextureManager& textures);
    void AddNeighbor(Tile* neighbor);

private:
    int xPos;
    int yPos;
};

#endif