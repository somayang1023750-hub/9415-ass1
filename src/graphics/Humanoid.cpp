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

    void Humanoid::draw(const Shader& shader, const glm::mat4& parentMatrix, const HumanoidPose& pose) const {
        const auto m = humanoidTransforms(parentMatrix, pose);
        shader.setMat4("uModel", m.rightLeg); rightLeg_->draw();
        shader.setMat4("uModel", m.leftLeg); leftLeg_->draw();
        shader.setMat4("uModel", m.torso); torso_->draw();
        shader.setMat4("uModel", m.head); head_->draw();
        shader.setMat4("uModel", m.rightArm); rightArm_->draw();
        shader.setMat4("uModel", m.leftArm); leftArm_->draw();
    }

} // namespace craft::graphics
