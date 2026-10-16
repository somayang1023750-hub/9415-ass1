#pragma once
#include <array>
#include "graphics/AtlasMesh.h"
#include "graphics/TreePose.h"
#include "graphics/Shader.h"

namespace craft::graphics {
class NaturalProps {
public:
    NaturalProps();
    void draw(const Shader& shader, const std::array<TreeControls,2>& trees) const;
private:
    std::array<glm::vec3,2> treeOrigins_;
    std::array<glm::vec3,12> plantOrigins_;
    AtlasMesh wood_{BlockType::Wood}, leaves_{BlockType::Leaves};
    AtlasMesh grass_{0,12}, red_{2,12}, yellow_{4,12}, blue_{6,12};
};
} // namespace craft::graphics
