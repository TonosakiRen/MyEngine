#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "Texture.h"

class TextureManager {
public:
    static TextureManager* GetInstance();

    uint32_t Load(const std::filesystem::path& path);

    Texture& GetTexture(const uint32_t& textureHandle) { return *textures_[textureHandle].second; }

private:
    std::vector<std::pair<std::wstring, std::unique_ptr<Texture>>> textures_;
};