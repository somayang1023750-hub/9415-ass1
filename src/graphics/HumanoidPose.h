#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace craft::graphics {
struct HumanoidPose {
    float headYaw = 0.0F;
    float rightArmPitch = 0.0F;
    float leftArmPitch = 0.0F;
    float rightLegPitch = 0.0F;
    float leftLegPitch = 0.0F;
};
// Walk in place: opposite legs/arms alternate, while contralateral limbs match.
// Base manual angles are preserved as offsets; zero stride gives the base pose.
inline HumanoidPose walkingPose(HumanoidPose base, float phaseDegrees, float strideDegrees) {
    const float swing = std::sin(glm::radians(phaseDegrees)) * strideDegrees;
    base.rightArmPitch += swing;
    base.leftArmPitch -= swing;
    base.rightLegPitch -= swing;
    base.leftLegPitch += swing;
    return base;
}
struct HumanoidTransforms {
    glm::mat4 rightLeg, leftLeg, torso, head, rightArm, leftArm;
};
inline HumanoidTransforms humanoidTransforms(const glm::mat4& root, const HumanoidPose& pose = {}) {
    constexpr float s = 1.0F / 16.0F;
    HumanoidTransforms m;
    const auto rightHip = glm::translate(root, glm::vec3(-2*s, 12*s, 0));
    const auto leftHip = glm::translate(root, glm::vec3(2*s, 12*s, 0));
    m.rightLeg = glm::translate(glm::rotate(rightHip, glm::radians(pose.rightLegPitch),
                               glm::vec3(1,0,0)), glm::vec3(0,-6*s,0));
    m.leftLeg = glm::translate(glm::rotate(leftHip, glm::radians(pose.leftLegPitch),
                              glm::vec3(1,0,0)), glm::vec3(0,-6*s,0));
    m.torso = glm::translate(root, glm::vec3(0, 18*s, 0));
    const auto shoulders = glm::translate(m.torso, glm::vec3(0, 6*s, 0));
    const auto neck = glm::rotate(shoulders, glm::radians(pose.headYaw), glm::vec3(0,1,0));
    m.head = glm::translate(neck, glm::vec3(0, 4*s, 0));
    const auto rightShoulder = glm::translate(shoulders, glm::vec3(-6*s,0,0));
    m.rightArm = glm::translate(glm::rotate(rightShoulder, glm::radians(pose.rightArmPitch),
                               glm::vec3(1,0,0)), glm::vec3(0,-6*s,0));
    const auto leftShoulder = glm::translate(shoulders, glm::vec3(6*s,0,0));
    m.leftArm = glm::translate(glm::rotate(leftShoulder, glm::radians(pose.leftArmPitch),
                              glm::vec3(1,0,0)), glm::vec3(0,-6*s,0));
    return m;
}
} // namespace craft::graphics
