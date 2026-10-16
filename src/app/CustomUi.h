#pragma once

#include <cstddef>
#include <glm/vec3.hpp>
#include "app/SceneSettings.h"

namespace craft::app {

    struct DiagnosticsData {
        glm::vec3 cameraPosition{ 0.0F };
        float cameraYaw = 0.0F;
        float cameraPitch = 0.0F;
        bool cameraBlocked = false;
        int chunkWidth = 32;
        int chunkHeight = 32;
        int chunkDepth = 32;
        std::size_t triangleCount = 0;
        std::size_t solidBlockCount = 0;
    };

    class CustomUi {
    public:
        void drawContents(bool interactionDisabled);
        SceneSettings& settings() noexcept { return settings_; }
        const SceneSettings& settings() const noexcept { return settings_; }

        void updateDiagnostics(const DiagnosticsData& data) noexcept {
            data_ = data;
        }

        [[nodiscard]] bool isWireframe() const noexcept {
            return settings_.wireframe;
        }

    private:
        DiagnosticsData data_;
        SceneSettings settings_;
        int selectedCharacter_ = 0;
    };

}  // namespace craft::app