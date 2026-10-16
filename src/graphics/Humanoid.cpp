#include "graphics/Humanoid.h"

namespace craft::graphics {

	Humanoid::Humanoid() {
		// Base layer of the bundled 64x64 classic-arm skin. Left limbs
		// have their own UV islands and need no mirrored geometry.

		head_ = std::make_unique<BoxMesh>(8.0F, 8.0F, 8.0F, 0.0F, 0.0F);

		torso_ = std::make_unique<BoxMesh>(8.0F, 12.0F, 4.0F, 16.0F, 16.0F);

		rightArm_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 40.0F, 16.0F);
		leftArm_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 32.0F, 48.0F);

		rightLeg_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 0.0F, 16.0F);
		leftLeg_ = std::make_unique<BoxMesh>(4.0F, 12.0F, 4.0F, 16.0F, 48.0F);
	}

	void Humanoid::draw(const Shader& shader, const glm::mat4& parentMatrix, float time, bool neutralPose) const {
		constexpr float s = 1.0F / 16.0F;

		// Root is at the feet; shoulders parent the head and both arms.
		const glm::mat4 root = parentMatrix;

		glm::mat4 rLeg = glm::translate(root, glm::vec3(-2.0F * s, 6.0F * s, 0.0F));
		shader.setMat4("uModel", rLeg);
		rightLeg_->draw();

		glm::mat4 lLeg = glm::translate(root, glm::vec3(2.0F * s, 6.0F * s, 0.0F));
		shader.setMat4("uModel", lLeg);
		leftLeg_->draw();

		glm::mat4 torsoMatrix = glm::translate(root, glm::vec3(0.0F, 18.0F * s, 0.0F));
		shader.setMat4("uModel", torsoMatrix);
		torso_->draw();

		glm::mat4 shoulderNode = glm::translate(torsoMatrix, glm::vec3(0.0F, 6.0F * s, 0.0F));

		glm::mat4 headMatrix = glm::translate(shoulderNode, glm::vec3(0.0F, 4.0F * s, 0.0F));
		shader.setMat4("uModel", headMatrix);
		head_->draw();

		glm::mat4 rArm = glm::translate(shoulderNode, glm::vec3(-6.0F * s, -6.0F * s, 0.0F));
		shader.setMat4("uModel", rArm);
		rightArm_->draw();

		glm::mat4 lArm = glm::translate(shoulderNode, glm::vec3(6.0F * s, -6.0F * s, 0.0F));
		shader.setMat4("uModel", lArm);
		leftArm_->draw();
	}

}  // namespace craft::graphics
