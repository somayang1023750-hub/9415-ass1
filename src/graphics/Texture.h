#pragma once

#include <filesystem>
#include "graphics/OpenGL.h"

namespace craft::graphics {

	class Texture {
	public:
		// The bundled terrain atlas uses opaque magenta as its cutout key.
        explicit Texture(const std::filesystem::path& filepath, bool magentaCutout = false);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		void bind(unsigned int slot = 0) const;

	private:
		GLuint id_ = 0;
	};

}  // namespace craft::graphics