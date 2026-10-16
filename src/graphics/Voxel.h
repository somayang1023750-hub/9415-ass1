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
        Brick
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

    inline std::array<glm::vec2, 4> getFaceUVs(BlockType type, FaceDirection face) {
        int tileX = 0;
        int tileY = 0;

        switch (type) {
        case BlockType::Grass:
            if (face == FaceDirection::Top) {
                tileX = 2; tileY = 0;
            }
            else if (face == FaceDirection::Bottom) {
                tileX = 6; tileY = 0;
            }
            else {
                tileX = 0; tileY = 0;
            }
            break;
        case BlockType::Dirt:
            tileX = 6; tileY = 0;
            break;
        case BlockType::Stone:
            tileX = 5; tileY = 0;
            break;
        case BlockType::Wood:
            tileX = 4; tileY = 0;
            break;
        case BlockType::Brick:
            tileX = 3; tileY = 0;
            break;
        default:
            tileX = 5; tileY = 0;
            break;
        }

        constexpr float tileSize = 1.0F / 16.0F;
        constexpr float pad = 0.001F;

        float u0 = tileX * tileSize + pad;
        float u1 = (tileX + 1) * tileSize - pad;
        float v0 = 1.0F - (tileY + 1) * tileSize + pad;
        float v1 = 1.0F - tileY * tileSize - pad;

        return {
            glm::vec2(u0, v0),
            glm::vec2(u1, v0),
            glm::vec2(u1, v1),
            glm::vec2(u0, v1)
        };
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