#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace craft::graphics {

    class VoxelWorld;

    class Camera {
    public:
        Camera(glm::vec3 position = glm::vec3(16.0F, 16.0F, 40.0F),
            glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F),
            float yaw = -90.0F,
            float pitch = -15.0F);

        [[nodiscard]] glm::mat4 viewMatrix() const;
        [[nodiscard]] glm::mat4 projectionMatrix(float aspectRatio) const;

        void processKeyboard(int key, int action, float deltaTime, const VoxelWorld* collisionWorld = nullptr);
        void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

        [[nodiscard]] const glm::vec3& position() const noexcept { return position_; }
        [[nodiscard]] const glm::vec3& front() const noexcept { return front_; }
        [[nodiscard]] float yaw() const noexcept { return yaw_; }
        [[nodiscard]] float pitch() const noexcept { return pitch_; }

    private:
        void updateCameraVectors();

        glm::vec3 position_;
        glm::vec3 front_{ 0.0F, 0.0F, -1.0F };
        glm::vec3 up_{ 0.0F, 1.0F, 0.0F };
        glm::vec3 right_{ 1.0F, 0.0F, 0.0F };
        glm::vec3 worldUp_{ 0.0F, 1.0F, 0.0F };

        float yaw_;
        float pitch_;
        float moveSpeed_{ 12.0F };
        float mouseSensitivity_{ 0.1F };
        float fov_{ 55.0F };
    };

}  // namespace craft::graphics