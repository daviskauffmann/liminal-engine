#ifndef GAME_PROGRAM_HPP
#define GAME_PROGRAM_HPP

namespace pk
{
    class program
    {
    public:
        program(const std::string &vertex_filename, const std::string &fragment_filename);
        ~program();

        void bind() const;
        void unbind(void) const;
        GLint get_location(const std::string &name);
        void set_int(const std::string &name, int value);
        void set_unsigned_int(const std::string &name, unsigned int value);
        void set_float(const std::string &name, float value);
        void set_vec3(const std::string &name, vec3 vec);
        void set_vec4(const std::string &name, vec4 vec);
        void set_mat4(const std::string &name, mat4 mat);
    private:
        GLuint program_id;
        std::unordered_map<std::string, GLint> uniforms;
    };
}

#endif
