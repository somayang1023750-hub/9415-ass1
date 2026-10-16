#include "graphics/Humanoid.h"

namespace craft::graphics {

	Humanoid::Humanoid() {
		// Head: 8x8x8, UV 起点 (0, 0)
		head_ = std::make_unique<BoxMesh>(8.0F, 8.0F, 8.0F, 0.0F, 0.0F);

		// Torso: 8x12x4, UV 起点 (16, 16)
		torso_ = std::make_unique<BoxMesh>(8.0F, 12.0F, 4.0F, 16.0F, 16.0F);

		// Arms: 4x12x4, UV 起点 (40, 16)
		rightArm_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 40.0F, 16.0F);
		leftArm_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 40.0F, 16.0F);

		// Legs: 4x12x4, UV 起点 (0, 16)
		rightLeg_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 0.0F, 16.0F);
		leftLeg_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 0.0F, 16.0F);
	}

	void Humanoid::draw(const Shader& shader, const glm::mat4& parentMatrix, float time, bool neutralPose) const {
		constexpr float s = 1.0F / 16.0F;

		// 1. Root: 双脚着地基准点 (面向 +Z)
		const glm::mat4 root = parentMatrix;

		// 2. 双腿 (挂在 Root 下，脚底触碰 y=0，高 12px，中心在 6px)
		// 右腿 (X = -2px)
		glm::mat4 rLeg = glm::translate(root, glm::vec3(-2.0F * s, 6.0F * s, 0.0F));
		shader.setMat4("uModel", rLeg);
		rightLeg_->draw();

		// 左腿 (X = +2px, 沿 X 轴翻转以镜像材质)
		glm::mat4 lLeg = glm::translate(root, glm::vec3(2.0F * s, 6.0F * s, 0.0F));
		lLeg = glm::scale(lLeg, glm::vec3(-1.0F, 1.0F, 1.0F));
		shader.setMat4("uModel", lLeg);
		glDisable(GL_CULL_FACE);
		leftLeg_->draw();
		glEnable(GL_CULL_FACE);

		// 3. 躯干 (从腰部 y=12px 向上延伸到 y=24px，中心在 18px)
		glm::mat4 torsoMatrix = glm::translate(root, glm::vec3(0.0F, 18.0F * s, 0.0F));
		shader.setMat4("uModel", torsoMatrix);
		torso_->draw();

		// 4. 肩膀/脖子节点 (在躯干顶部 y=24px 处，即 relative +6px)
		glm::mat4 shoulderNode = glm::translate(torsoMatrix, glm::vec3(0.0F, 6.0F * s, 0.0F));

		// 头部 (挂在肩膀上方，高 8px，中心在 +4px)
		glm::mat4 headMatrix = glm::translate(shoulderNode, glm::vec3(0.0F, 4.0F * s, 0.0F));
		shader.setMat4("uModel", headMatrix);
		head_->draw();

		// 5. 双臂 (挂在肩膀两侧，长 12px，中心在肩膀下方 -6px)
		// 右臂 (X = -6px)
		glm::mat4 rArm = glm::translate(shoulderNode, glm::vec3(-6.0F * s, -6.0F * s, 0.0F));
		shader.setMat4("uModel", rArm);
		rightArm_->draw();

		// 左臂 (X = +6px, X 轴镜像)
		glm::mat4 lArm = glm::translate(shoulderNode, glm::vec3(6.0F * s, -6.0F * s, 0.0F));
		lArm = glm::scale(lArm, glm::vec3(-1.0F, 1.0F, 1.0F));
		shader.setMat4("uModel", lArm);
		glDisable(GL_CULL_FACE);
		leftArm_->draw();
		glEnable(GL_CULL_FACE);
	}

}  // namespace craft::graphics