#pragma once

#include <vector>
#include <cstddef>
#include <array>
#include "graphics/VoxelWorld.h"
#include "graphics/OpenGL.h"

namespace craft::graphics {

    class Chunk {
    public:
        static constexpr int kWidth = 32;
        static constexpr int kHeight = 32;
        static constexpr int kDepth = 32;

        Chunk();
        ~Chunk();

        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;

        void generateTerrain();
        void buildMesh();
        void draw() const;

        [[nodiscard]] std::size_t triangleCount() const noexcept { return triangleCount_; }
        [[nodiscard]] std::size_t solidBlockCount() const noexcept { return world_.solidBlockCount(); }

        [[nodiscard]] const VoxelWorld& world() const noexcept { return world_; }

    private:
        [[nodiscard]] bool isSolid(int x, int y, int z) const;
        void addFace(int x, int y, int z, FaceDirection face, BlockType type,
            std::vector<VoxelVertex>& vertices, std::vector<unsigned int>& indices);

        VoxelWorld world_;

        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ebo_ = 0;
        GLsizei indexCount_ = 0;
        std::size_t triangleCount_ = 0;
    };

}  // namespace craft::graphics