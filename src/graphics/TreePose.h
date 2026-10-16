#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace craft::graphics {
struct TreeControls {
    glm::vec3 offset{0};
    float yaw=0;
    float leftBranch=30;
    float rightBranch=30;
};
struct TreeTransforms {
    glm::mat4 trunk, topCrown, leftBranch, rightBranch, leftCrown, rightCrown;
};
inline TreeTransforms treeTransforms(const glm::mat4& root, float leftAngle=30, float rightAngle=30) {
    TreeTransforms m;
    m.trunk=glm::scale(glm::translate(root,glm::vec3(0,1.7F,0)),glm::vec3(.65F,3.4F,.65F));
    const auto top=glm::translate(root,glm::vec3(0,3.4F,0));
    m.topCrown=glm::scale(glm::translate(top,glm::vec3(0,.4F,0)),glm::vec3(2.3F,1.8F,2.3F));
    const auto split=glm::translate(root,glm::vec3(0,2.1F,0));
    const auto left=glm::rotate(split,glm::radians(leftAngle),glm::vec3(0,0,1));
    const auto right=glm::rotate(split,glm::radians(-rightAngle),glm::vec3(0,0,1));
    m.leftBranch=glm::scale(glm::translate(left,glm::vec3(0,.75F,0)),glm::vec3(.4F,1.5F,.4F));
    m.rightBranch=glm::scale(glm::translate(right,glm::vec3(0,.75F,0)),glm::vec3(.4F,1.5F,.4F));
    m.leftCrown=glm::scale(glm::translate(left,glm::vec3(0,1.7F,0)),glm::vec3(1.6F,1.5F,1.8F));
    m.rightCrown=glm::scale(glm::translate(right,glm::vec3(0,1.7F,0)),glm::vec3(1.6F,1.5F,1.8F));
    return m;
}
} // namespace craft::graphics
