#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "graphics/OpenGL.h"

namespace craft::graphics {

	struct BoxVertex {
		glm::vec3 position;
		glm::vec2 texCoord;
		float brightness;
		glm::vec3 colorModifier;
	};

	class BoxMesh {
	public:
		BoxMesh(float w, float h, float d, float u, float v);
		~BoxMesh();

		BoxMesh(const BoxMesh&) = delete;
		BoxMesh& operator=(const BoxMesh&) = delete;

		void draw() const;

	private:
		GLuint vao_ = 0;
		GLuint vbo_ = 0;
		GLuint ebo_ = 0;
		GLsizei indexCount_ = 0;
	};

}  // namespace craft::graphics