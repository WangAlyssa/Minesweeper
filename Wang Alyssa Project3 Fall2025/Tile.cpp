//
// Created by Alyssa Wang on 2025/11/12.
//

#include "Tile.h"
#include "TextureManager.h"

Tile::Tile(int x, int y, TextureManager& textures)
    : sprite(textures.GetTexture("tile_hidden.png"))
{
    xPos = x;
    yPos = y;
}

void Tile::SetPosition(float x, float y) {
    sprite.setPosition({x, y});
}

void Tile::UpdateTexture(TextureManager& textures) {
    if (isRevealed) {
        sprite.setTexture(textures.GetTexture("tile_revealed.png"));
    } else {
        sprite.setTexture(textures.GetTexture("tile_hidden.png"));
    }
}

void Tile::AddNeighbor(Tile* neighbor) {
    adjacentTiles.push_back(neighbor);
}