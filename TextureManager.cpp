#include "TextureManager.h"

TextureManager* TextureManager::GetInstance() {
    static TextureManager instance;
    return &instance;
}

uint32_t TextureManager::Load(const std::filesystem::path& path) {
    uint32_t handle;
    std::wstring wstr = path.wstring();
    // 読み込み済みか探す
    auto iter = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) { return texture.first == wstr; });
    // 読み込み済み
    if (iter != textures_.end()) {
        handle = std::distance(textures_.begin(), iter);
        return handle;
    }

    // 最後尾に読み込む
    handle = textures_.size();

    auto texture = std::make_unique<Texture>();
    texture->CreateFromWICFile(wstr);

    textures_.emplace_back(wstr, std::move(texture));
    return handle;
}
