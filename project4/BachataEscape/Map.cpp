/**
* Author: Steven Dorry
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Map.h"
#include <cmath>

Map::Map(int mapColumns, int mapRows, unsigned int *levelData,
         const char *textureFilePath, float tileSize, int textureColumns,
         int textureRows, Vector2 origin) :
         mMapColumns {mapColumns}, mMapRows {mapRows},
         mTextureAtlas { textureFilePath && textureFilePath[0] ? LoadTexture(textureFilePath) : Texture2D{} },
         mLevelData {levelData}, mTileSize {tileSize},
         mTextureColumns {textureColumns}, mTextureRows {textureRows},
         mOrigin {origin}, mFallbackColor { Color{70, 70, 110, 255} } { build(); }

Map::~Map() { if (mTextureAtlas.id > 0) UnloadTexture(mTextureAtlas); }

void Map::build()
{
    mLeftBoundary   = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary  = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary    = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    mTextureAreas.clear();
    if (mTextureAtlas.id <= 0 || mTextureColumns <= 0 || mTextureRows <= 0) return;

    for (int row = 0; row < mTextureRows; row++)
    {
        for (int col = 0; col < mTextureColumns; col++)
        {
            Rectangle textureArea = {
                (float) col * (mTextureAtlas.width / mTextureColumns),
                (float) row * (mTextureAtlas.height / mTextureRows),
                (float) mTextureAtlas.width / mTextureColumns,
                (float) mTextureAtlas.height / mTextureRows
            };

            mTextureAreas.push_back(textureArea);
        }
    }
}

void Map::render()
{
    for (int row = 0; row < mMapRows; row++)
    {
        for (int col = 0; col < mMapColumns; col++)
        {
            int tile = mLevelData[row * mMapColumns + col];
            if (tile == 0) continue;

            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary  + row * mTileSize,
                mTileSize,
                mTileSize
            };

            if (mTextureAtlas.id > 0 && tile - 1 >= 0 && tile - 1 < (int)mTextureAreas.size())
            {
                DrawTexturePro(
                    mTextureAtlas,
                    mTextureAreas[tile - 1],
                    destinationArea,
                    {0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
            }
            else
            {
                DrawRectangleRec(destinationArea, mFallbackColor);
            }
        }
    }
}

bool Map::isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap)
{
    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    if (position.x < mLeftBoundary || position.x > mRightBoundary ||
        position.y < mTopBoundary  || position.y > mBottomBoundary)
        return false;

    int tileXIndex = floor((position.x - mLeftBoundary) / mTileSize);
    int tileYIndex = floor((position.y - mTopBoundary) / mTileSize);

    if (tileXIndex < 0 || tileXIndex >= mMapColumns ||
        tileYIndex < 0 || tileYIndex >= mMapRows)
        return false;

    int tile = mLevelData[tileYIndex * mMapColumns + tileXIndex];
    if (tile == 0) return false;

    float tileCentreX = mLeftBoundary + tileXIndex * mTileSize + mTileSize / 2.0f;
    float tileCentreY = mTopBoundary + tileYIndex * mTileSize + mTileSize / 2.0f;

    *xOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.x - tileCentreX));
    *yOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.y - tileCentreY));

    return true;
}
