#include "graphics/BoxMesh.h"
#include "graphics/SkinUV.h"

namespace craft::graphics {

    BoxMesh::BoxMesh(float w, float h, float d, float u, float v) {
        const auto skinUVs = boxSkinUVs(w, h, d, u, v);
        const auto& topUV = skinUVs[0];
        const auto& botUV = skinUVs[1];
        const auto& frontUV = skinUVs[2];
        const auto& backUV = skinUVs[3];
        const auto& leftUV = skinUVs[4];
        const auto& rightUV = skinUVs[5];

        constexpr float s = 1.0F / 16.0F;
        const float x0 = -w * 0.5F * s, x1 = w * 0.5F * s;
        const float y0 = -h * 0.5F * s, y1 = h * 0.5F * s;
        const float z0 = -d * 0.5F * s, z1 = d * 0.5F * s;

        std::vector<BoxVertex> vertices;
        std::vector<unsigned int> indices;
        const glm::vec3 white(1.0F, 1.0F, 1.0F);

        auto addQuad = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
            const std::array<glm::vec2, 4>& uvs, float bright) {
                const unsigned int idx = static_cast<unsigned int>(vertices.size());
                vertices.push_back({ p0, uvs[0], bright, white });
                vertices.push_back({ p1, uvs[1], bright, white });
                vertices.push_back({ p2, uvs[2], bright, white });
                vertices.push_back({ p3, uvs[3], bright, white });

                indices.push_back(idx + 0);
                indices.push_back(idx + 1);
                indices.push_back(idx + 2);
                indices.push_back(idx + 2);
                indices.push_back(idx + 3);
                indices.push_back(idx + 0);
        };

        addQuad({ x0, y1, z1 }, { x1, y1, z1 }, { x1, y1, z0 }, { x0, y1, z0 }, topUV, 1.0F);    // Top (+Y)
        addQuad({ x0, y0, z0 }, { x1, y0, z0 }, { x1, y0, z1 }, { x0, y0, z1 }, botUV, 0.6F);    // Bottom (-Y)
        addQuad({ x0, y0, z1 }, { x1, y0, z1 }, { x1, y1, z1 }, { x0, y1, z1 }, frontUV, 0.9F);  // Front (+Z)
        addQuad({ x1, y0, z0 }, { x0, y0, z0 }, { x0, y1, z0 }, { x1, y1, z0 }, backUV, 0.9F);   // Back (-Z)
        addQuad({ x0, y0, z0 }, { x0, y0, z1 }, { x0, y1, z1 }, { x0, y1, z0 }, leftUV, 0.75F);  // Left (-X)
        addQuad({ x1, y0, z1 }, { x1, y0, z0 }, { x1, y1, z0 }, { x1, y1, z1 }, rightUV, 0.75F); // Right (+X)

        indexCount_ = static_cast<GLsizei>(indices.size());

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BoxVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoxVertex), (void*)offsetof(BoxVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BoxVertex), (void*)offsetof(BoxVertex, texCoord));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(BoxVertex), (void*)offsetof(BoxVertex, brightness));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(BoxVertex), (void*)offsetof(BoxVertex, colorModifier));

        glBindVertexArray(0);
    }

    BoxMesh::~BoxMesh() {
        if (ebo_ != 0) glDeleteBuffers(1, &ebo_);
        if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
        if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    }

    void BoxMesh::draw() const {
        if (vao_ != 0 && indexCount_ > 0) {
            glBindVertexArray(vao_);
            glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }

}  // namespace craft::graphics
