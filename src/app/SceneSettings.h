#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <glm/vec3.hpp>
#include "graphics/HumanoidPose.h"

namespace craft::app {
struct CharacterSettings {
    glm::vec3 offset{0.0F};
    float yaw = 0.0F;
    graphics::HumanoidPose pose;
    bool walking = false;
    bool animateWalk = false;
    float walkPhase = 90.0F;
    float strideAngle = 30.0F;
    float walkSpeed = 1.0F; // cycles per second
    void advanceWalk(float deltaTime) {
        if (walking && animateWalk && std::isfinite(deltaTime) && deltaTime > 0)
            walkPhase = std::fmod(walkPhase + 360.0F * walkSpeed * std::min(deltaTime,0.25F), 360.0F);
    }
    [[nodiscard]] graphics::HumanoidPose effectivePose() const {
        return walking ? graphics::walkingPose(pose,walkPhase,strideAngle) : pose;
    }
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
