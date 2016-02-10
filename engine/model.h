#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

namespace moar
{

class Model
{
    friend class ResourceManager;

public:
    explicit Model();
    ~Model();
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;

private:
    void addMesh(std::unique_ptr<Mesh> mesh);

    // Todo: Mesh without pointer.
    std::vector<std::unique_ptr<Mesh>> meshes;
};

} // moar

#endif // MODEL_H
