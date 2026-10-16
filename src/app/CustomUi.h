#pragma once

#include <cstddef>
#include <glm/vec3.hpp>

namespace craft::app {

    struct DiagnosticsData {
        glm::vec3 cameraPosition{ 0.0F };
        float cameraYaw = 0.0F;
        float cameraPitch = 0.0F;
        int chunkWidth = 32;
        int chunkHeight = 32;
        int chunkDepth = 32;
        std::size_t triangleCount = 0;
        std::size_t solidBlockCount = 0;
    };

    class CustomUi {
    public:
        void drawContents(bool interactionDisabled);

        void updateDiagnostics(const DiagnosticsData& data) noexcept {
            data_ = data;
        }

        [[nodiscard]] bool isWireframe() const noexcept {
            return wireframe_;
        }

    private:
        DiagnosticsData data_;
        bool wireframe_ = false;
    };

}  // namespace craft::app