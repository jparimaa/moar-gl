#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "component.h"
#include "model.h"

#include <string>

namespace moar
{

class Renderer : public Component
{
public:
    static const glm::mat4* projection;
    static const glm::mat4* view;

    explicit Renderer();
    virtual ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    virtual void execute() final;

    void setModel(Model* model) { this->model = model; }

    virtual std::string getName() final { return "Renderer"; }
    virtual Component::Type getType() final { return Component::Type::RENDERER; }

private:
    Model* model;
    GLint currentShader;
};

} // moar

#endif // RENDEROBJECT_H