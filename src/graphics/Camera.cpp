#include "graphics/Camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>

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

    void Camera::processKeyboard(int key, int action, float deltaTime) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) {
            return;
        }
        const float velocity = moveSpeed_ * deltaTime;
        if (key == GLFW_KEY_W) position_ += front_ * velocity;
        if (key == GLFW_KEY_S) position_ -= front_ * velocity;
        if (key == GLFW_KEY_A) position_ -= right_ * velocity;
        if (key == GLFW_KEY_D) position_ += right_ * velocity;
        if (key == GLFW_KEY_SPACE || key == GLFW_KEY_E) position_ += worldUp_ * velocity;
        if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_Q) position_ -= worldUp_ * velocity;
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