#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <array>

namespace craft::graphics {

    enum class BlockType : uint8_t {
        Air = 0,
        Grass,
        Dirt,
        Stone,
        Wood,
        Brick,
        Leaves
    };

    enum class FaceDirection : uint8_t {
        Top = 0,    // +Y
        Bottom,     // -Y
        North,      // -Z
        South,      // +Z
        West,       // -X
        East        // +X
    };

    struct VoxelVertex {
        glm::vec3 position;
        glm::vec2 texCoord;
        float brightness;
        glm::vec3 colorModifier;
    };

    inline std::array<glm::vec2, 4> atlasTileUVs(int tileX, int tileY) {
        constexpr float tileSize = 1.0F / 16.0F;
        constexpr float pad = 0.001F;
        const float u0 = tileX * tileSize + pad, u1 = (tileX+1)*tileSize-pad;
        const float bottom = (tileY+1)*tileSize-pad, top = tileY*tileSize+pad;
        return {glm::vec2(u0,bottom), glm::vec2(u1,bottom), glm::vec2(u1,top), glm::vec2(u0,top)};
    }

    inline std::array<glm::vec2, 4> getFaceUVs(BlockType type, FaceDirection face) {
        int tileX = 0;
        int tileY = 15;

        switch (type) {
        case BlockType::Grass:
            if (face == FaceDirection::Top) {
                tileX = 0; tileY = 13;
            }
            else if (face == FaceDirection::Bottom) {
                tileX = 6; tileY = 15;
            }
            else {
                tileX = 0; tileY = 14;
            }
            break;
        case BlockType::Dirt:
            tileX = 6; tileY = 15;
            break;
        case BlockType::Stone:
            tileX = 5; tileY = 15;
            break;
        case BlockType::Wood:
            tileX = 4;
            tileY = face == FaceDirection::Top || face == FaceDirection::Bottom ? 13 : 14;
            break;
        case BlockType::Leaves:
            tileX = 14; tileY = 15;
            break;
        case BlockType::Brick:
            tileX = 3; tileY = 15;
            break;
        default:
            tileX = 5; tileY = 15;
            break;
        }

        return atlasTileUVs(tileX,tileY);
    }

    inline float getFaceBrightness(FaceDirection face) {
        switch (face) {
        case FaceDirection::Top:    return 1.0F;
        case FaceDirection::North:
        case FaceDirection::South:  return 0.82F;
        case FaceDirection::West:
        case FaceDirection::East:   return 0.68F;
        case FaceDirection::Bottom: return 0.50F;
        }
        return 1.0F;
    }

}  // namespace craft::graphics