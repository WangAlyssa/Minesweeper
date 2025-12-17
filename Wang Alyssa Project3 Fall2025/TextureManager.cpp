//
// Created by Alyssa Wang on 2025/11/12.
//

#include "TextureManager.h"

void TextureManager::LoadTexture(const std::string& fileName) {
    if (textures.find(fileName) == textures.end()) {
        std::string path = "files/images/";
        path += fileName;
        textures[fileName].loadFromFile(path);
    }
}

sf::Texture& TextureManager::GetTexture(const std::string& fileName) {
    if (textures.find(fileName) == textures.end()) {
        LoadTexture(fileName);
    }
    return textures[fileName];
}

void TextureManager::Clear() {
    textures.clear();
}