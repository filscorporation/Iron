#pragma once

struct Sprite
{
public:
    unsigned int ID;
    const char* Path;
    unsigned int TextureID;
    int Width;
    int Height;

    bool IsSpriteSheet = false;
    int TileWidth;
    int TileHeight;

    void SetAsSpriteSheet(int tileWidth, int tileHeight);
    const float* GetTexCoord(int tileIndex);
    int TilesCount();
};