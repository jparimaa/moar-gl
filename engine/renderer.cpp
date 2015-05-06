#include "renderer.h"
#include "object.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace moar
{

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::execute()
{    
    model->render();
}

void Renderer::setModel(Model* model)
{
    this->model = model;
}

std::string Renderer::getName()
{
    return "Renderer";
}

Component::Type Renderer::getType()
{
    return Component::Type::RENDERER;
}

} // moar

