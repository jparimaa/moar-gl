#ifndef SHADER_H
#define SHADER_H

#include "common/globals.h"

#include <GL/glew.h>

#include <vector>
#include <string>
#include <bitset>
#include <array>

namespace moar
{

class Shader
{
public:
    enum Type
    {
        UNDEFINED = 0,
        DIFFUSE = 1 << 0,
        SPECULAR = 1 << 1,
        NORMAL = 1 << 2,
        BUMP = 1 << 3,
        DEPTH = 1 << 4
    };

    static void loadCommonShaderCode(GLenum type, const std::string& file);
    static void addCommonShaderCode(GLenum type, const std::string& addition);

    explicit Shader();
    ~Shader();
    Shader(const Shader& rhs) = delete;
    Shader(Shader&& rhs) = delete;
    Shader& operator=(Shader rhs) = delete;
    Shader& operator=(Shader&& rhs) = delete;

    bool attachShader(GLenum shaderType, const std::string& filename, const std::string& defines = "");
    bool linkProgram();

    GLuint getProgram() const;
    bool hasUniform(GLuint location) const;

    GLuint getShadowMapLocation(int num) const;

private:
    static std::string* selectCommonCode(GLenum type);

    static std::string commonVertexShaderCode;
    static std::string commonFragmentShaderCode;

    void deleteShaders();
    bool compileShader(GLuint shader, const std::string& filename, const std::string& defines = "", const std::string& common = "");
    bool readUniformLocations();

    GLuint program;    
    std::vector<GLuint> shaders;
    std::bitset<MAX_UNIFORM_LOCATION> uniforms;
    std::array<GLuint, MAX_NUM_SHADOWMAPS> shadowMapLocations;
};

} // moar

#endif // SHADER_H
