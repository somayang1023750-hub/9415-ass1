#include "graphics/Chunk.h"
#include <algorithm>
#include <cmath>

namespace craft::graphics {

    Chunk::Chunk() {
        buildMesh();
    }

    Chunk::~Chunk() {
        if (ebo_ != 0) glDeleteBuffers(1, &ebo_);
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void Chunk::generateTerrain() {
        world_.generateTerrain();
    }

    bool Chunk::isSolid(int x, int y, int z) const {
        return world_.isSolid(x, y, z);
    }

    void Chunk::addFace(int x, int y, int z, FaceDirection face, BlockType type,
        std::vector<VoxelVertex>& vertices, std::vector<unsigned int>& indices) {
        const auto uvs = getFaceUVs(type, face);
        const float brightness = getFaceBrightness(face);
        const unsigned int startIndex = static_cast<unsigned int>(vertices.size());

        const float fx = static_cast<float>(x);
        const float fy = static_cast<float>(y);
        const float fz = static_cast<float>(z);

        glm::vec3 tint = glm::vec3(1.0F, 1.0F, 1.0F);
        // The atlas grass tiles already contain their final green colour.

        glm::vec3 p0, p1, p2, p3;

        switch (face) {
        case FaceDirection::Top: // +Y
            p0 = { fx,     fy + 1, fz + 1 };
            p1 = { fx + 1, fy + 1, fz + 1 };
            p2 = { fx + 1, fy + 1, fz };
            p3 = { fx,     fy + 1, fz };
            break;
        case FaceDirection::Bottom: // -Y
            p0 = { fx,     fy, fz };
            p1 = { fx + 1, fy, fz };
            p2 = { fx + 1, fy, fz + 1 };
            p3 = { fx,     fy, fz + 1 };
            break;
        case FaceDirection::North: // -Z
            p0 = { fx + 1, fy,     fz };
            p1 = { fx,     fy,     fz };
            p2 = { fx,     fy + 1, fz };
            p3 = { fx + 1, fy + 1, fz };
            break;
        case FaceDirection::South: // +Z
            p0 = { fx,     fy,     fz + 1 };
            p1 = { fx + 1, fy,     fz + 1 };
            p2 = { fx + 1, fy + 1, fz + 1 };
            p3 = { fx,     fy + 1, fz + 1 };
            break;
        case FaceDirection::West: // -X
            p0 = { fx,     fy,     fz };
            p1 = { fx,     fy,     fz + 1 };
            p2 = { fx,     fy + 1, fz + 1 };
            p3 = { fx,     fy + 1, fz };
            break;
        case FaceDirection::East: // +X
            p0 = { fx + 1, fy,     fz + 1 };
            p1 = { fx + 1, fy,     fz };
            p2 = { fx + 1, fy + 1, fz };
            p3 = { fx + 1, fy + 1, fz + 1 };
            break;
        }

        vertices.push_back({ p0, uvs[0], brightness, tint });
        vertices.push_back({ p1, uvs[1], brightness, tint });
        vertices.push_back({ p2, uvs[2], brightness, tint });
        vertices.push_back({ p3, uvs[3], brightness, tint });

        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
        indices.push_back(startIndex + 0);
    }

    void Chunk::buildMesh() {
        std::vector<VoxelVertex> vertices;
        std::vector<unsigned int> indices;

        for (int x = 0; x < kWidth; ++x) {
            for (int y = 0; y < kHeight; ++y) {
                for (int z = 0; z < kDepth; ++z) {
                    BlockType type = world_.blockAt(x, y, z);
                    if (type == BlockType::Air) continue;

                    if (!isSolid(x, y + 1, z)) addFace(x, y, z, FaceDirection::Top, type, vertices, indices);
                    if (!isSolid(x, y - 1, z)) addFace(x, y, z, FaceDirection::Bottom, type, vertices, indices);
                    if (!isSolid(x, y, z - 1)) addFace(x, y, z, FaceDirection::North, type, vertices, indices);
                    if (!isSolid(x, y, z + 1)) addFace(x, y, z, FaceDirection::South, type, vertices, indices);
                    if (!isSolid(x - 1, y, z)) addFace(x, y, z, FaceDirection::West, type, vertices, indices);
                    if (!isSolid(x + 1, y, z)) addFace(x, y, z, FaceDirection::East, type, vertices, indices);
                }
            }
        }

        indexCount_ = static_cast<GLsizei>(indices.size());
        triangleCount_ = indices.size() / 3;

        if (vao_ == 0) glGenVertexArrays(1, &vao_);
        if (vbo_ == 0) glGenBuffers(1, &vbo_);
        if (ebo_ == 0) glGenBuffers(1, &ebo_);

        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VoxelVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // aPosition (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, position));

        // aTexCoord (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, texCoord));

        // aBrightness (location = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, brightness));

        // aColorModifier (location = 3)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, colorModifier));

        glBindVertexArray(0);
    }

    void Chunk::draw() const {
        if (vao_ != 0 && indexCount_ > 0) {
            glBindVertexArray(vao_);
            glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }

}  // namespace craft::graphics