#ifndef ATLAS_HPP
#define ATLAS_HPP

#include <glm/vec2.hpp>
#include <vector>

#include "texture.hpp"

namespace pk
{
	class atlas : public texture
	{
	public:
		atlas(const std::string &filename, GLuint size, GLuint texture_size);

		std::vector<GLfloat> get_texture_coords(const glm::vec2 &location);

	private:
		GLuint size;
		GLuint texture_size;
	};
} // namespace pk

#endif
