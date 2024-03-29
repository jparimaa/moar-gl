#ifndef ENGINE_H
#define ENGINE_H

#include "application.h"
#include "resourcemanager.h"
#include "gui.h"
#include "input.h"
#include "rendersettings.h"
#include "time.h"
#include "camera.h"
#include "object.h"
#include "renderer.h"

#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>

namespace moar
{

class Engine
{
public:
	struct PerformanceData 
	{
		int FPS = -1;
		int drawCount = -1;
		float gpuIdle = -1.0f;
	};

    explicit Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    void setApplication(Application* application);
    bool init(const std::string& settingsFile);
    void execute();

    ResourceManager* getResourceManager();
    Camera* getCamera();
    Input* getInput();
    RenderSettings* getRenderSettings();
    Time* getTime();
    Object* getObjectByName(const std::string& name) const;

    Object* createObject(const std::string& name = "");
    bool loadLevel(const std::string& level);
    void setDeferredRendering(bool enabled);

	const PerformanceData& getPerformanceData() const;

private:
    void resetLevel();
    void updateObjects();
	void updatePerformanceData();
	bool createSkybox();
    void printInfo(int windowWidth, int windowHeight);    

    std::shared_ptr<Application> app;

    GLFWwindow* window = nullptr;
    ResourceManager manager;
    GUI gui;
    Input input;
    RenderSettings renderSettings;
    Renderer renderer;
    Time time;

    std::vector<std::unique_ptr<Object>> objects;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Object> skybox;

	PerformanceData performanceData;
};

} // moar

#endif // ENGINE_H

