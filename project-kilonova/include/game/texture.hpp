#ifndef GAME_TEXTURE_HPP
#define GAME_TEXTURE_HPP

struct Texture
{
    GLuint texture_id;

    Texture(const std::string &filename);
    ~Texture();

    void bind(unsigned int index) const;
};

#endif
