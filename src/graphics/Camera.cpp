#include "graphics/Camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include "graphics/VoxelWorld.h"

namespace craft::graphics {

    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : position_(position),
        worldUp_(up),
        yaw_(yaw),
        pitch_(pitch) {
        updateCameraVectors();
    }

    glm::mat4 Camera::viewMatrix() const {
        return glm::lookAt(position_, position_ + front_, up_);
    }

    glm::mat4 Camera::projectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fov_), aspectRatio, 0.1F, 150.0F);
    }

    void Camera::processKeyboard(int key, int action, float deltaTime, const VoxelWorld* world) {
        if ((action != GLFW_PRESS && action != GLFW_REPEAT) || !std::isfinite(deltaTime) || deltaTime <= 0) return;
        // Ignore long suspended-window pauses rather than teleport through terrain.
        const float velocity = moveSpeed_ * std::min(deltaTime, 0.25F);
        glm::vec3 delta(0.0F);
        if (key == GLFW_KEY_W) delta += front_ * velocity;
        if (key == GLFW_KEY_S) delta -= front_ * velocity;
        if (key == GLFW_KEY_A) delta -= right_ * velocity;
        if (key == GLFW_KEY_D) delta += right_ * velocity;
        if (key == GLFW_KEY_SPACE || key == GLFW_KEY_E) delta += worldUp_ * velocity;
        if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_Q) delta -= worldUp_ * velocity;
        if (!world) { position_ += delta; return; }
        // Steps shorter than the eye AABB radius prevent tunnelling. Resolve each
        // axis independently so the camera slides along walls instead of sticking.
        const int steps = std::max(1, static_cast<int>(std::ceil(glm::length(delta) / 0.10F)));
        delta /= static_cast<float>(steps);
        for (int step = 0; step < steps; ++step) {
            for (int axis = 0; axis < 3; ++axis) {
                auto candidate = position_;
                candidate[axis] += delta[axis];
                candidate = glm::clamp(candidate, glm::vec3(-16.0F, 0.21F, -16.0F), glm::vec3(48.0F, 48.0F, 64.0F));
                if (!world->intersectsCamera(candidate)) position_ = candidate;
            }
        }
    }

    void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
        xoffset *= mouseSensitivity_;
        yoffset *= mouseSensitivity_;

        yaw_ += xoffset;
        pitch_ += yoffset;

        if (constrainPitch) {
            pitch_ = std::clamp(pitch_, -89.0F, 89.0F);
        }

        updateCameraVectors();
    }

    void Camera::updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
        newFront.y = std::sin(glm::radians(pitch_));
        newFront.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
        front_ = glm::normalize(newFront);

        right_ = glm::normalize(glm::cross(front_, worldUp_));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

}  // namespace craft::graphics