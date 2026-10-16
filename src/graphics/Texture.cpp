#include "graphics/Texture.h"

#include <stdexcept>
#include <stb_image.h>

namespace craft::graphics {

    Texture::Texture(const std::filesystem::path& filepath) {
        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_set_flip_vertically_on_load(0);
        unsigned char* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 4);

        if (!data) {
            throw std::runtime_error("Failed to load texture at path: " + filepath.string());
        }

        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }

    Texture::~Texture() {
        if (id_ != 0) {
            glDeleteTextures(1, &id_);
        }
    }

    void Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id_);
    }

}  // namespace craft::graphics