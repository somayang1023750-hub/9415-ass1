#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "graphics/BoxMesh.h"
#include "graphics/Shader.h"
#include "graphics/HumanoidPose.h"

namespace craft::graphics {

	class Humanoid {
	public:
		Humanoid();

		void draw(const Shader& shader, const glm::mat4& parentMatrix, const HumanoidPose& pose = {}) const;

	private:
		std::unique_ptr<BoxMesh> head_;
		std::unique_ptr<BoxMesh> torso_;
		std::unique_ptr<BoxMesh> leftArm_;
		std::unique_ptr<BoxMesh> rightArm_;
		std::unique_ptr<BoxMesh> leftLeg_;
		std::unique_ptr<BoxMesh> rightLeg_;
	};

}  // namespace craft::graphics