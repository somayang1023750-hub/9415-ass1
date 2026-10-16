#include "graphics/VoxelWorld.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace craft::graphics {

VoxelWorld::VoxelWorld(bool generate) {
    blocks_.fill(BlockType::Air);
    if (generate) generateTerrain();
}

bool VoxelWorld::contains(int x, int y, int z) {
    return x >= 0 && x < kWidth && y >= 0 && y < kHeight && z >= 0 && z < kDepth;
}
std::size_t VoxelWorld::index(int x, int y, int z) {
    return static_cast<std::size_t>(x + y * kWidth + z * kWidth * kHeight);
}
BlockType VoxelWorld::blockAt(int x, int y, int z) const {
    return contains(x, y, z) ? blocks_[index(x, y, z)] : BlockType::Air;
}
bool VoxelWorld::isSolid(int x, int y, int z) const {
    return blockAt(x, y, z) != BlockType::Air;
}
void VoxelWorld::setBlock(int x, int y, int z, BlockType type) {
    if (!contains(x, y, z)) throw std::out_of_range("Voxel outside the 32x32x32 chunk");
    auto& block = blocks_[index(x, y, z)];
    if (block != BlockType::Air) --solidCount_;
    block = type;
    if (block != BlockType::Air) ++solidCount_;
}
int VoxelWorld::terrainHeight(int x, int z) {
    // Fixed analytic rule, no random state: identical on every run.
    return std::clamp(8 + static_cast<int>(3.0 * std::sin(x * 0.25) +
                                          2.0 * std::cos(z * 0.25)), 3, 16);
}
void VoxelWorld::generateTerrain() {
    blocks_.fill(BlockType::Air);
    solidCount_ = 0;
    for (int x = 0; x < kWidth; ++x) {
        for (int z = 0; z < kDepth; ++z) {
            const int height = terrainHeight(x, z);
            for (int y = 0; y <= height; ++y) {
                setBlock(x, y, z, y == height ? BlockType::Grass :
                    (y >= height - 2 ? BlockType::Dirt : BlockType::Stone));
            }
        }
    }
    for (int x = 13; x <= 19; ++x) {
        for (int z = 13; z <= 19; ++z) {
            for (int y = 9; y <= 22; ++y) {
                const bool wall = x == 13 || x == 19 || z == 13 || z == 19 || y == 9 || y == 22;
                const bool doorway = z == 19 && x >= 16 && x <= 17 && y >= 10 && y <= 12;
                setBlock(x, y, z, wall && !doorway ? BlockType::Brick : BlockType::Air);
            }
        }
    }
}
bool VoxelWorld::intersectsCamera(const glm::vec3& eye, float radius) const {
    // A small eye-centred AABB; fly camera has no gravity or player physics.
    const glm::ivec3 lo(glm::floor(eye - radius));
    const glm::ivec3 hi(glm::floor(eye + radius));
    for (int x = lo.x; x <= hi.x; ++x)
        for (int y = lo.y; y <= hi.y; ++y)
            for (int z = lo.z; z <= hi.z; ++z)
                if (isSolid(x, y, z)) return true;
    return false;
}

} // namespace craft::graphics
