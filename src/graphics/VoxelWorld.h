#pragma once

#include <array>
#include <cstddef>
#include "graphics/Voxel.h"

namespace craft::graphics {

// CPU world data is independent of the OpenGL mesh and can be tested headlessly.
class VoxelWorld {
public:
    static constexpr int kWidth = 32, kHeight = 32, kDepth = 32;
    explicit VoxelWorld(bool generate = true);
    void generateTerrain();
    static constexpr int kTerrainSeed = 941526;
    [[nodiscard]] static int terrainHeight(int x, int z, int seed = kTerrainSeed);
    [[nodiscard]] static int towerFloorHeight();
    [[nodiscard]] static int towerRoofHeight() { return towerFloorHeight()+13; }
    [[nodiscard]] BlockType blockAt(int x, int y, int z) const;
    [[nodiscard]] bool isSolid(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);
    [[nodiscard]] std::size_t solidBlockCount() const { return solidCount_; }
    [[nodiscard]] bool intersectsCamera(const glm::vec3& eye, float radius = 0.20F) const;
private:
    static bool contains(int x, int y, int z);
    static std::size_t index(int x, int y, int z);
    std::array<BlockType, kWidth * kHeight * kDepth> blocks_{};
    std::size_t solidCount_ = 0;
};

} // namespace craft::graphics
