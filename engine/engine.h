#ifndef ENGINE
#define ENGINE

#include "application.h"
#include "resourcemanager.h"
#include "gui.h"
#include "input.h"
#include "camera.h"
#include "object.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

namespace moar
{

class Engine
{
public:
    explicit Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    void setApplication(std::shared_ptr<Application> application);
    bool init(const std::string& settingsFile);
    void execute();

    ResourceManager* getResourceManager();
    Camera* getCamera();
    Input* getInput();
    void addObject(Object* object);

private:
    void executeCustomComponents();
    void render();

    void printInfo(int windowWidth, int windowHeight);

    std::shared_ptr<Application> app;

    GLFWwindow* window;

    ResourceManager manager;
    GUI gui;
    Input input;
    Camera camera;
    std::vector<Object*> renderObjects;
    std::vector<Object*> lights;
    std::vector<std::shared_ptr<Object>> allObjects;

    bool useTimeLimit;
    double timeLimit;
    double time;
};

} // moar

#endif // ENGINE

