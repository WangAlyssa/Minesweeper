//
// Created by Alyssa Wang on 2025/11/12.
//

#include "Board.h"
#include "TextureManager.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

Board::Board() {}

bool Board::GetTileIndices(float x, float y, int& col, int& row) {
    col = static_cast<int>(x / 32.0f);
    row = static_cast<int>(y / 32.0f);
    return (col >= 0 && col < columns && row >= 0 && row < rows);
}

std::vector<Tile*> Board::GetNeighbors(int col, int row) {
    std::vector<Tile*> neighbors;
    for (int dc = -1; dc <= 1; ++dc) {
        for (int dr = -1; dr <= 1; ++dr) {
            if (dc == 0 && dr == 0) continue;
            int neighborCol = col + dc;
            int neighborRow = row + dr;
            if (neighborCol >= 0 && neighborCol < columns && neighborRow >= 0 && neighborRow < rows) {
                neighbors.push_back(&grid[neighborRow][neighborCol]);
            }
        }
    }
    return neighbors;
}

void Board::Initialize(int cols, int rows, int mines, TextureManager& textures) {
    columns = cols;
    this->rows = rows;
    totalMines = mines;
    totalTiles = cols * rows;
    tilesRevealed = 0;
    currentState = PLAYING;
    flagsPlaced = 0;
    debugMode = false;
    leaderboardShown = false;

    grid.clear();
    grid.resize(rows);
    for (int r = 0; r < rows; ++r) {
        grid[r].reserve(columns);
        for (int c = 0; c < columns; ++c) {
            grid[r].emplace_back(c, r, textures);
            grid[r][c].SetPosition((float)c * 32.0f, (float)r * 32.0f);
        }
    }

    PlaceMines(mines);
    SetupNeighbors();
    CalculateAdjacentMines();
}

void Board::Restart(TextureManager& textures) {
    Initialize(columns, rows, totalMines, textures);
}

void Board::PlaceMines(int mineCount) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    std::vector<int> tileIndices(totalTiles);
    for (int i = 0; i < totalTiles; ++i) tileIndices[i] = i;
    std::shuffle(tileIndices.begin(), tileIndices.end(), generator);

    for (int i = 0; i < mineCount; ++i) {
        int index = tileIndices[i];
        grid[index / columns][index % columns].isMine = true;
    }
}

void Board::SetupNeighbors() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            std::vector<Tile*> neighbors = GetNeighbors(c, r);
            for (Tile* neighbor : neighbors) {
                grid[r][c].AddNeighbor(neighbor);
            }
        }
    }
}

void Board::CalculateAdjacentMines() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            if (grid[r][c].isMine) continue;
            int count = 0;
            for (Tile* neighbor : grid[r][c].adjacentTiles) {
                if (neighbor->isMine) count++;
            }
            grid[r][c].adjacentMines = count;
        }
    }
}

void Board::Draw(sf::RenderWindow& window, bool paused, TextureManager& textures) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            if (paused) {
                sf::Sprite temp = grid[r][c].sprite;
                temp.setTexture(textures.GetTexture("tile_revealed.png"));
                window.draw(temp);
            } else {
                grid[r][c].UpdateTexture(textures);
                window.draw(grid[r][c].sprite);

                sf::Sprite overlaySprite(textures.GetTexture("tile_hidden.png"));
                overlaySprite.setPosition(grid[r][c].sprite.getPosition());

                if (!grid[r][c].isRevealed) {
                    if (grid[r][c].hasFlag) {
                        overlaySprite.setTexture(textures.GetTexture("flag.png"));
                        window.draw(overlaySprite);
                    }
                    else if (debugMode && grid[r][c].isMine) {
                        overlaySprite.setTexture(textures.GetTexture("mine.png"));
                        window.draw(overlaySprite);
                    }
                }
                else {
                    if (grid[r][c].isMine) {
                        overlaySprite.setTexture(textures.GetTexture("mine.png"));
                        window.draw(overlaySprite);
                    }
                    else if (grid[r][c].adjacentMines > 0) {
                        overlaySprite.setTexture(textures.GetTexture("number_" + std::to_string(grid[r][c].adjacentMines) + ".png"));
                        window.draw(overlaySprite);
                    }
                }
            }
        }
    }
}

void Board::LeftClickTile(float x, float y) {
    if (currentState != PLAYING) return;
    int c, r;
    if (GetTileIndices(x, y, c, r)) {
        Tile* tile = GetTile(c, r);
        if (tile->isRevealed || tile->hasFlag) return;

        tile->isRevealed = true;
        tilesRevealed++;

        if (tile->isMine) {
            currentState = LOSE;
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < columns; ++j) {
                     if(grid[i][j].isMine) {
                         grid[i][j].isRevealed = true;
                     }
                }
            }
        } else {
            if (tile->adjacentMines == 0) RevealEmptyTiles(tile);
            if (tilesRevealed == totalTiles - totalMines) {
                currentState = WIN;
                for (int i = 0; i < rows; ++i) {
                     for (int j = 0; j < columns; ++j) {
                         if(grid[i][j].isMine) grid[i][j].hasFlag = true;
                     }
                }
                flagsPlaced = totalMines;
            }
        }
    }
}

void Board::RightClickTile(float x, float y) {
    if (currentState != PLAYING) return;
    int c, r;
    if (GetTileIndices(x, y, c, r)) {
        Tile* tile = GetTile(c, r);
        if (!tile->isRevealed) {
            tile->hasFlag = !tile->hasFlag;
            flagsPlaced += (tile->hasFlag ? 1 : -1);
        }
    }
}

void Board::RevealEmptyTiles(Tile* tile) {
    for (Tile* neighbor : tile->adjacentTiles) {
        if (!neighbor->isRevealed && !neighbor->hasFlag && !neighbor->isMine) {
            neighbor->isRevealed = true;
            tilesRevealed++;
            if (neighbor->adjacentMines == 0) RevealEmptyTiles(neighbor);
        }
    }
}

void Board::ToggleDebugMode() {
    if (currentState == PLAYING) debugMode = !debugMode;
}

Tile* Board::GetTile(int col, int row) {
    return &grid[row][col];
}