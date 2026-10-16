#include "graphics/BoxMesh.h"

namespace craft::graphics {

    BoxMesh::BoxMesh(float w, float h, float d, float u, float v) {
        constexpr float tw = 64.0F;
        constexpr float th = 32.0F;

        // 严谨的 UV 坐标转换：由于 stb 未翻转，V 轴用 py/th 还是 1-py/th
        // 这里统一标准：顶部 py=0 对应 v=1.0，底部 py=th 对应 v=0.0
        auto uv = [&](float px, float py) {
            return glm::vec2(px / tw, 1.0F - (py / th));
        };

        // -------------------------------------------------------------------------
        // Minecraft 64x32 标准立方体展开规则 (严格对齐 CCW 正向几何)
        // -------------------------------------------------------------------------
        // Top (+Y):
        const glm::vec2 topUV[4] = {
            uv(u + d, v + d), uv(u + d + w, v + d),
            uv(u + d + w, v), uv(u + d, v)
        };

        // Bottom (-Y):
        const glm::vec2 botUV[4] = {
            uv(u + d + w, v), uv(u + d + 2.0F * w, v),
            uv(u + d + 2.0F * w, v + d), uv(u + d + w, v + d)
        };

        // Front (+Z): 面向正前方 (例如正脸、胸腹腹肌)
        const glm::vec2 frontUV[4] = {
            uv(u + d, v + d + h),      // p0: 左下
            uv(u + d + w, v + d + h),  // p1: 右下
            uv(u + d + w, v + d),      // p2: 右上
            uv(u + d, v + d)           // p3: 左上
        };

        // Back (-Z): 面向正后方 (后脑勺、后背)
        const glm::vec2 backUV[4] = {
            uv(u + 2.0F * d + 2.0F * w, v + d + h), // p0: 左下 (面向背面时的左下)
            uv(u + 2.0F * d + w, v + d + h),        // p1: 右下
            uv(u + 2.0F * d + w, v + d),            // p2: 右上
            uv(u + 2.0F * d + 2.0F * w, v + d)      // p3: 左上
        };

        // Left (-X): 身体左侧 (外侧观察为右侧)
        const glm::vec2 leftUV[4] = {
            uv(u, v + d + h),
            uv(u + d, v + d + h),
            uv(u + d, v + d),
            uv(u, v + d)
        };

        // Right (+X): 身体右侧
        const glm::vec2 rightUV[4] = {
            uv(u + d + w, v + d + h),
            uv(u + 2.0F * d + w, v + d + h),
            uv(u + 2.0F * d + w, v + d),
            uv(u + d + w, v + d)
        };

        constexpr float s = 1.0F / 16.0F;
        const float x0 = -w * 0.5F * s, x1 = w * 0.5F * s;
        const float y0 = -h * 0.5F * s, y1 = h * 0.5F * s;
        const float z0 = -d * 0.5F * s, z1 = d * 0.5F * s;

        std::vector<BoxVertex> vertices;
        std::vector<unsigned int> indices;
        const glm::vec3 white(1.0F, 1.0F, 1.0F);

        auto addQuad = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
            const glm::vec2 uvs[4], float bright) {
                const unsigned int idx = static_cast<unsigned int>(vertices.size());
                vertices.push_back({ p0, uvs[0], bright, white });
                vertices.push_back({ p1, uvs[1], bright, white });
                vertices.push_back({ p2, uvs[2], bright, white });
                vertices.push_back({ p3, uvs[3], bright, white });

                // 逆时针正面三角形：(0, 1, 2) 与 (2, 3, 0)
                indices.push_back(idx + 0);
                indices.push_back(idx + 1);
                indices.push_back(idx + 2);
                indices.push_back(idx + 2);
                indices.push_back(idx + 3);
                indices.push_back(idx + 0);
        };

        // 6 个面 (严格逆时针)
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