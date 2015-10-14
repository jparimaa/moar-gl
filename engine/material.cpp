#include "material.h"
#include "globals.h"

#include <iostream>

namespace moar
{

const Material::TextureInfo Material::textureInfos[] =
{
    {Material::TextureType::DIFFUSE, "DiffuseTex", GL_TEXTURE0, 0, DIFFUSE_TEX_LOCATION},
    {Material::TextureType::NORMAL, "NormalTex", GL_TEXTURE1, 1, NORMAL_TEX_LOCATION},
    {Material::TextureType::DISPLACEMENT, "DisplacementTex", GL_TEXTURE2, 2, DISPLACEMENT_TEX_LOCATION},
};

Material::Material() :
    shaderType(""),
    shader(0),
    specularity(0.0f)
{
}

Material::~Material()
{
}

void Material::execute()
{
    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(std::get<1>(textures[i])->unit);
        glBindTexture(std::get<2>(textures[i]), std::get<0>(textures[i]));
        glUniform1i(std::get<1>(textures[i])->location, std::get<1>(textures[i])->value);
    }

    // Todo: Custom uniforms like in postprocess; std::map<std::string, std::function<void()>>.
    if (specularity > 0.0f) {
        glUniform1f(SPECULAR_LOCATION, specularity);
    }
}

void Material::setShaderType(const std::string& shaderType)
{
    this->shaderType = shaderType;
    shader = 0;
}

void Material::setShader(GLuint shader)
{
    this->shader = shader;
    shaderType = "";
}

void Material::setTexture(GLuint texture, TextureType type, GLenum target)
{
    for (unsigned int i = 0; i < textures.size(); ++i) {
        if (std::get<1>(textures[i])->type == type) {
            std::get<0>(textures[i]) = texture;
            std::get<2>(textures[i]) = target;
            return;
        }
    }

    textures.push_back(std::make_tuple(texture, getTextureInfo(type), target));
}

void Material::setSpecularity(float specularity)
{
    this->specularity = specularity;
}

std::string Material::getName()
{
    return "Material";
}

Component::Type Material::getType()
{
    return Component::Type::MATERIAL;
}

const Material::TextureInfo* Material::getTextureInfo(TextureType type)
{
    for (unsigned int i = 0; i < (sizeof(textureInfos)/sizeof(*textureInfos)); ++i) {
        if (textureInfos[i].type == type) {
            return &textureInfos[i];
        }
    }

    std::cerr << "WARNING: Could not map material texture information" << std::endl;
    return nullptr;
}

std::string Material::getShaderType() const
{
    return shaderType;
}

GLuint Material::getShader() const
{
    return shader;
}

} // moar
