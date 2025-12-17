//
// Created by Alyssa Wang on 2025/11/12.
//

#ifndef MINESWEEPER_TEXTUREMANAGER_H
#define MINESWEEPER_TEXTUREMANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class TextureManager {
private:
    std::map<std::string, sf::Texture> textures;

public:
    void LoadTexture(const std::string& fileName);
    sf::Texture& GetTexture(const std::string& fileName);
    void Clear();
};

#endif