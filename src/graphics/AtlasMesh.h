#pragma once
#include "graphics/Voxel.h"
#include "graphics/OpenGL.h"
#include <vector>

namespace craft::graphics {
// Unit cube centered at the origin, or two crossed, double-sided plant quads
// rooted at y=0. Both use the same atlas/vertex layout as the chunk.
class AtlasMesh {
public:
    explicit AtlasMesh(BlockType type);
    AtlasMesh(int tileX, int tileY);
    ~AtlasMesh();
    AtlasMesh(const AtlasMesh&) = delete;
    AtlasMesh& operator=(const AtlasMesh&) = delete;
    void draw() const;
private:
    void upload(const std::vector<VoxelVertex>& vertices, const std::vector<unsigned int>& indices);
    GLuint vao_=0, vbo_=0, ebo_=0;
    GLsizei count_=0;
};
} // namespace craft::graphics
