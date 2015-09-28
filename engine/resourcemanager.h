#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "shader.h"
#include "light.h"
#include "model.h"
#include "texture.h"

#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <functional>

namespace moar
{

class ResourceManager
{
public:

    ResourceManager();
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    void setShaderPath(const std::string& path);
    void setModelPath(const std::string& path);
    void setTexturePath(const std::string& path);

    GLuint getShader(const std::string& name);    
    GLuint getShader(const std::string& shader, const Light::Type light);
    Model* getModel(const std::string& modelName);
    GLuint getTexture(const std::string& textureName);
    GLuint getTexture(std::vector<std::string> textureNames);

private:

    using ShaderKey = std::pair<std::string, Light::Type>;

    struct ShaderKeyHash
    {
        size_t operator()(const ShaderKey& key) const
        {
            return std::hash<std::string>()(key.first + std::to_string(key.second));
        }
    };

    static std::unordered_map<ShaderKey, std::string, ShaderKeyHash> shaderNames;

    std::string shaderPath;
    std::string modelPath;
    std::string texturePath;
    std::unordered_map<ShaderKey, GLuint, ShaderKeyHash> shadersByType;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shadersByName;
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<Texture>> cubeTextures;
};

} // moar

#endif // RESOURCEMANAGER_H
