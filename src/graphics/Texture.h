#pragma once

#include <filesystem>
#include "graphics/OpenGL.h"

namespace craft::graphics {

	class Texture {
	public:
		explicit Texture(const std::filesystem::path& filepath);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		void bind(unsigned int slot = 0) const;

	private:
		GLuint id_ = 0;
	};

}  // namespace craft::graphics