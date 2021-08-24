#ifndef ATLAS_HPP
#define ATLAS_HPP

#include <glm/vec2.hpp>
#include <vector>

#include "texture.hpp"

namespace liminal
{
	class atlas : public liminal::texture
	{
	public:
		atlas(const std::string &filename, GLuint size, GLuint texture_size);

		std::vector<GLfloat> get_texture_coords(const glm::vec2 &location);

	private:
		GLuint size;
		GLuint texture_size;
	};
}

#endif
