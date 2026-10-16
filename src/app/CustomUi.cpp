#include "app/CustomUi.h"
#include "imgui.h"

namespace craft::app {

    void CustomUi::drawContents(bool interactionDisabled) {
        ImGui::SeparatorText("Camera Diagnostics");
        ImGui::Text("Position: (%.1f, %.1f, %.1f)",
            data_.cameraPosition.x,
            data_.cameraPosition.y,
            data_.cameraPosition.z);
        ImGui::Text("Yaw: %.1f deg | Pitch: %.1f deg",
            data_.cameraYaw,
            data_.cameraPitch);

        ImGui::SeparatorText("Chunk & Scene");
        ImGui::Text("Dimensions: %d x %d x %d",
            data_.chunkWidth,
            data_.chunkHeight,
            data_.chunkDepth);
        ImGui::Text("Solid Blocks: %zu", data_.solidBlockCount);
        ImGui::Text("Triangles: %zu", data_.triangleCount);

        ImGui::SeparatorText("Render Settings");
        ImGui::BeginDisabled(interactionDisabled);
        ImGui::Checkbox("Wireframe Mode", &wireframe_);
        ImGui::EndDisabled();
    }

}  // namespace craft::app
