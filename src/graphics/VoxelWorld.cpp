#include "graphics/VoxelWorld.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cstdint>

namespace craft::graphics {
namespace {
// Seeded value noise: unsigned arithmetic and no mutable RNG state, so material
// lattice values are reproducible and independent of generation order.
constexpr std::uint32_t kMaterialSeed = 0x941526U;
float latticeValue(int x, int y, int z) {
    std::uint32_t h = kMaterialSeed ^ (static_cast<std::uint32_t>(x) * 0x8da6b343U)
        ^ (static_cast<std::uint32_t>(y) * 0xd8163841U)
        ^ (static_cast<std::uint32_t>(z) * 0xcb1ab31fU);
    h ^= h >> 16; h *= 0x7feb352dU;
    h ^= h >> 15; h *= 0x846ca68bU; h ^= h >> 16;
    return static_cast<float>(h >> 8) / 16777215.0F;
}
float materialNoise(float x, float y, float z) {
    const int ix = static_cast<int>(std::floor(x));
    const int iy = static_cast<int>(std::floor(y));
    const int iz = static_cast<int>(std::floor(z));
    const auto smooth = [](float t) { return t*t*(3.0F-2.0F*t); };
    const float sx = smooth(x-ix), sy = smooth(y-iy), sz = smooth(z-iz);
    const auto layer = [&](int dz) {
        return std::lerp(std::lerp(latticeValue(ix,iy,iz+dz), latticeValue(ix+1,iy,iz+dz), sx),
                         std::lerp(latticeValue(ix,iy+1,iz+dz), latticeValue(ix+1,iy+1,iz+dz), sx), sy);
    };
    return std::lerp(layer(0), layer(1), sz);
}
BlockType terrainMaterial(int x, int y, int z, int height) {
    const int depth = height-y;
    if (depth == 0) return BlockType::Grass;
    // Protect the upper soil, vary the rock/soil interface by column, and leave
    // sparse coherent soil pockets in the predominantly rocky lower volume.
    const int soilDepth = 2 + static_cast<int>(3.0F * materialNoise(x*0.18F, 3.7F, z*0.18F));
    if (depth <= soilDepth) return BlockType::Dirt;
    const float threshold = 0.68F + 0.008F * std::min(depth,10);
    return materialNoise(x*0.24F, y*0.35F, z*0.24F) > threshold ? BlockType::Dirt : BlockType::Stone;
}
} // namespace

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
                setBlock(x, y, z, terrainMaterial(x,y,z,height));
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
