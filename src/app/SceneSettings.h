#pragma once
#include <array>
#include <glm/vec3.hpp>
#include "graphics/HumanoidPose.h"

namespace craft::app {
struct CharacterSettings {
    glm::vec3 offset{0.0F};
    float yaw = 0.0F;
    graphics::HumanoidPose pose;
};
struct SceneSettings {
    bool wireframe = false;
    bool fog = false;
    bool collision = true;
    float fogStart = 25.0F;
    float fogEnd = 65.0F;
    std::array<CharacterSettings, 2> characters;
};
} // namespace craft::app
