#include "app/CustomUi.h"
#include "imgui.h"

namespace craft::app {

    void CustomUi::drawContents(bool interactionDisabled) {
        ImGui::TextUnformatted("WASD + mouse: fly | Space/Shift: up/down");
        ImGui::TextUnformatted("Tab: capture/release cursor | Esc: quit");
        ImGui::SeparatorText("Camera Diagnostics");
        ImGui::Text("Position: (%.1f, %.1f, %.1f)",
            data_.cameraPosition.x,
            data_.cameraPosition.y,
            data_.cameraPosition.z);
        ImGui::Text("Yaw: %.1f deg | Pitch: %.1f deg",
            data_.cameraYaw,
            data_.cameraPitch);

        if (data_.cameraBlocked) ImGui::TextUnformatted("Collision/boundary: movement blocked");
        ImGui::SeparatorText("Chunk & Scene");
        ImGui::Text("Dimensions: %d x %d x %d",
            data_.chunkWidth,
            data_.chunkHeight,
            data_.chunkDepth);
        ImGui::Text("Solid Blocks: %zu", data_.solidBlockCount);
        ImGui::Text("Visible faces: %zu | Triangles: %zu", data_.triangleCount / 2, data_.triangleCount);
        ImGui::TextUnformatted("Terrain: Perlin fBm / seed 941526");

        ImGui::SeparatorText("Render Settings");
        ImGui::BeginDisabled(interactionDisabled);
        ImGui::Checkbox("Wireframe Mode", &settings_.wireframe);
        ImGui::Checkbox("Camera collision", &settings_.collision);
        ImGui::Checkbox("Distance fog", &settings_.fog);
        if (settings_.fog) {
            ImGui::SliderFloat("Fog near", &settings_.fogStart, 0.0F, 60.0F, "%.1f");
            ImGui::SliderFloat("Fog far", &settings_.fogEnd, settings_.fogStart + 1.0F, 100.0F, "%.1f");
            if (settings_.fogEnd <= settings_.fogStart) settings_.fogEnd = settings_.fogStart + 1.0F;
        }
        if (ImGui::CollapsingHeader("Character hierarchy")) {
            ImGui::RadioButton("Tower", &selectedCharacter_, 0); ImGui::SameLine();
            ImGui::RadioButton("Ground", &selectedCharacter_, 1);
            auto& character = settings_.characters[selectedCharacter_];
            ImGui::SliderFloat3("Root offset", &character.offset.x, -3.0F, 3.0F, "%.1f");
            ImGui::SliderFloat("Root yaw", &character.yaw, -180.0F, 180.0F, "%.0f");
            ImGui::SliderFloat("Head yaw", &character.pose.headYaw, -75.0F, 75.0F, "%.0f");
            if (ImGui::TreeNode("Manual limb angles")) {
                ImGui::SliderFloat("Right arm", &character.pose.rightArmPitch, -90.0F, 90.0F, "%.0f");
                ImGui::SliderFloat("Left arm", &character.pose.leftArmPitch, -90.0F, 90.0F, "%.0f");
                ImGui::SliderFloat("Right leg", &character.pose.rightLegPitch, -60.0F, 60.0F, "%.0f");
                ImGui::SliderFloat("Left leg", &character.pose.leftLegPitch, -60.0F, 60.0F, "%.0f");
                ImGui::TreePop();
            }
            ImGui::Checkbox("Walking pose", &character.walking);
            if (character.walking) {
                ImGui::Checkbox("Animate walk", &character.animateWalk);
                ImGui::SliderFloat("Stride angle", &character.strideAngle, 0.0F, 45.0F, "%.0f deg");
                ImGui::BeginDisabled(character.animateWalk);
                ImGui::SliderFloat("Walk phase", &character.walkPhase, 0.0F, 360.0F, "%.0f deg");
                ImGui::EndDisabled();
                ImGui::SliderFloat("Walk speed", &character.walkSpeed, 0.2F, 3.0F, "%.1f Hz");
            }
            if (ImGui::Button("Reset selected to neutral")) character = {};
        }
        if (ImGui::CollapsingHeader("Tree hierarchy")) {
            ImGui::RadioButton("Tree 1",&selectedTree_,0); ImGui::SameLine();
            ImGui::RadioButton("Tree 2",&selectedTree_,1);
            auto& tree=settings_.trees[selectedTree_];
            ImGui::SliderFloat3("Tree offset",&tree.offset.x,-3.0F,3.0F,"%.1f");
            ImGui::SliderFloat("Tree yaw",&tree.yaw,-180.0F,180.0F,"%.0f");
            ImGui::SliderFloat("Left branch",&tree.leftBranch,15.0F,65.0F,"%.0f");
            ImGui::SliderFloat("Right branch",&tree.rightBranch,15.0F,65.0F,"%.0f");
            if (ImGui::Button("Reset selected tree")) tree={};
        }
        ImGui::EndDisabled();
    }

}  // namespace craft::app
