#ifndef GAME_PROGRAM_H
#define GAME_PROGRAM_H

class Program
{
public:
    Program(const std::string &vertexFilename, const std::string &fragmentFilename);
    ~Program();

    GLint getLocation(const std::string &name) const;
    void bind() const;
    void setInt(const std::string &name, int value) const;
    void setUnsignedInt(const std::string &name, unsigned int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, vec3 vec) const;
    void setVec4(const std::string &name, vec4 vec) const;
    void setMat4(const std::string &name, mat4 mat) const;
    void unbind(void) const;
private:
    GLuint programId;
    std::map<std::string, GLint> uniforms;
};

#endif
