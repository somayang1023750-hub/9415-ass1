#pragma once

#include <array>
#include <glm/vec2.hpp>

namespace craft::graphics {

// Minecraft classic-arm 64x64 skin, in top-left image pixel coordinates.
// Faces follow BoxMesh's outward CCW vertex order: top, bottom, front, back,
// right limb's outer side (-X), left limb's outer side (+X).
inline std::array<std::array<glm::vec2, 4>, 6> boxSkinUVs(
    float w, float h, float d, float u, float v) {
    const auto uv = [](float x, float y) { return glm::vec2(x / 64.0F, y / 64.0F); };
    return {{
        {uv(u+d, v+d), uv(u+d+w, v+d), uv(u+d+w, v), uv(u+d, v)},
        {uv(u+d+w, v), uv(u+d+2*w, v), uv(u+d+2*w, v+d), uv(u+d+w, v+d)},
        {uv(u+d, v+d+h), uv(u+d+w, v+d+h), uv(u+d+w, v+d), uv(u+d, v+d)},
        {uv(u+2*d+w, v+d+h), uv(u+2*d+2*w, v+d+h), uv(u+2*d+2*w, v+d), uv(u+2*d+w, v+d)},
        {uv(u, v+d+h), uv(u+d, v+d+h), uv(u+d, v+d), uv(u, v+d)},
        {uv(u+d+w, v+d+h), uv(u+2*d+w, v+d+h), uv(u+2*d+w, v+d), uv(u+d+w, v+d)}
    }};
}

} // namespace craft::graphics
