#include "engine.h"
#include "model.h"
#include "material.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <exception>
#include <utility>
#include <fstream>
#include <algorithm>

namespace
{

void APIENTRY debugCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
                                    const GLchar* message, void* /*userParam*/)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    std::cerr << "OpenGL debug callback function\n";
    std::cerr << "    Message: "<< message << "\n";

    std::cerr << "    Source: ";
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        std::cerr << "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cerr << "WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cerr << "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cerr << "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cerr << "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cerr << "OTHER";
        break;
    }
    std::cerr << "\n";

    std::cerr << "    Type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cerr << "TYPE_ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cerr << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cerr << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cerr << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cerr << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cerr << "MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cerr << "PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cerr << "POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cerr << "OTHER";
        break;
    default:
        std::cerr << "UNKNOWN";
        break;
    }
    std::cerr << "\n";

    std::cerr << "    ID: " << id << "\n";
    std::cerr << "    Severity: ";
    switch (severity){
    case GL_DEBUG_SEVERITY_LOW:
        std::cerr << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << "HIGH";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cerr << "NOTIFICATION";
        break;
    default:
        std::cerr << "UNKNOWN";
        break;
    }
    std::cerr << "\n\n";
}

void glfwErrorCallback(int error, const char* description )
{
    std::cerr << "ERROR GLFW: " << error << " " << description << "\n";
}

} // anonymous

namespace moar
{

Engine::Engine()
{
}

Engine::~Engine()
{
    glDeleteBuffers(1, &Object::transformationBlockBuffer);
    glDeleteBuffers(1, &Light::lightBlockBuffer);
    glfwTerminate();
}

void Engine::setApplication(Application* application)
{
    app.reset(application);
    app->setEngine(this);
}

bool Engine::init(const std::string& settingsFile)
{
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to initialize GLFW\n";
        return false;
    }

    glfwSetErrorCallback(glfwErrorCallback);

    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(settingsFile, pt);
    } catch (std::exception& e) {
        std::cerr << "ERROR: Could not load setting file; " << settingsFile << "\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    try {        
        glfwWindowHint(GLFW_SAMPLES, pt.get<int>("OpenGL.multisampling"));
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, pt.get<int>("OpenGL.major"));
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, pt.get<int>("OpenGL.minor"));
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "ERRROR: Could not load OpenGL version info from the .ini-file\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    int windowWidth = 800;
    int windowHeight = 600;
    try {
        windowWidth = pt.get<int>("Window.width");
        windowHeight = pt.get<int>("Window.height");        
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load window info from the .ini-file\n";
        std::cerr << e.what() << "\n";
    }

    renderSettings.windowWidth = windowWidth;
    renderSettings.windowHeight = windowHeight;

    int windowPosX = 0;
    int windowPosY = 0;
    try {
        windowPosX = pt.get<int>("Window.Xposition");
        windowPosY = pt.get<int>("Window.Yposition");
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load window position from the .ini-file\n";
        std::cerr << e.what() << "\n";
    }

    window = glfwCreateWindow(windowWidth, windowHeight, pt.get<std::string>("Window.title").c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "ERROR: Failed to create window\n";
        return false;
    }

    auto key = [] (GLFWwindow* w, int key, int scancode, int action, int mods)
    {
        static_cast<Input*>(glfwGetWindowUserPointer(w))->handleKey(w, key, scancode, action, mods);
    };

    glfwMakeContextCurrent(window);    
    glfwSetWindowPos(window, windowPosX, windowPosY);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, &input);
    glfwSetKeyCallback(window, key);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "ERROR: " << glewGetErrorString(err) << "\n";
        return false;
    }
#ifdef DEBUG
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);        
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
        glDebugMessageCallback(debugCallbackFunction, nullptr);
    }
#endif

    printInfo(windowWidth, windowHeight);

    try {
        double sensitivity = pt.get<double>("Input.sensitivity");
        float movementSpeed = pt.get<double>("Input.movementSpeed");
        input.setSensitivity(sensitivity);
        input.setMovementSpeed(movementSpeed);
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load input info from the .ini-file\n";
        std::cerr << e.what() << "\n";
    }

    if (!gui.init(window)) {
        std::cerr << "ERROR: Failed to initialize AntTweakBar\n";
        return false;
    }

    input.setGUI(&gui);

    std::string shaderInfoFile = "";
    try {
        manager.setShaderPath(pt.get<std::string>("Engine.shaderPath"));
        manager.setModelPath(pt.get<std::string>("Engine.modelPath"));
        manager.setTexturePath(pt.get<std::string>("Engine.texturePath"));
        manager.setLevelPath(pt.get<std::string>("Engine.levelPath"));
        shaderInfoFile = pt.get<std::string>("Engine.shaders");
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "ERROR: Could not load resource path info from the .ini-file\n";
        std::cerr << e.what() << "\n";
        return false;
    }

    if (!manager.loadShaders(shaderInfoFile)) {
        return false;
    }

    camera.reset(new Camera());
    Object::view = camera->getViewMatrixPointer();
    Object::projection = camera->getProjectionMatrixPointer();

    if (!renderSettings.loadSettings(pt, manager)) {
        std::cerr << "WARNING: Failed to load render settings\n";
    }

    depthMapDir.setSize(renderSettings.windowWidth, renderSettings.windowHeight);
    if (!depthMapDir.init()) {
        return false;
    }

    depthMapPoint.setSize(1024, 1024);
    if (!depthMapPoint.init()) {
        return false;
    }

    Framebuffer::setSize(renderSettings.windowWidth, renderSettings.windowHeight);
    PostFramebuffer::setSize(renderSettings.windowWidth, renderSettings.windowHeight);
    bool framebuffersInitialized =
            fb.init(2) &&
            postBuffer1.init() &&
            postBuffer2.init() &&
            blitBuffer1.init() &&
            blitBuffer2.init();
    if (!framebuffersInitialized) {
        std::cerr << "ERROR: Framebuffer status is incomplete\n";
        return false;
    }

    passthrough = Postprocess("passthrough", manager.getShader("passthrough")->getProgram(), 0);

    GLuint lightBuffer;
    glGenBuffers(1, &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
    GLsizeiptr lightBufferSize = 16 + 16 + 16;
    glBufferData(GL_UNIFORM_BUFFER, lightBufferSize, 0, GL_DYNAMIC_DRAW);
    Light::lightBlockBuffer = lightBuffer;

    GLuint transformationBuffer;
    glGenBuffers(1, &transformationBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBuffer);
    GLsizeiptr transformationBufferSize = 4 * sizeof(*Object::projection);
    glBufferData(GL_UNIFORM_BUFFER, transformationBufferSize, 0, GL_DYNAMIC_DRAW);
    Object::transformationBlockBuffer = transformationBuffer;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

#ifdef DEBUG
    std::cout << "\nTHIS PROGRAM IS EXECUTED WITH THE DEBUG FLAG\n\n";
#endif

    resetLevel();

    return true;
}

void Engine::execute()
{
    app->start();
    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos(window, &x, &y);
    input.setCursorPosition(x, y);
    input.resetCursorDelta();

    while (app->isRunning()) {
        glfwGetCursorPos(window, &x, &y);
        input.setCursorPosition(x, y);

        time.update();
        app->handleInput(window);
        input.reset();
        app->update();

        updateObjectContainers();
        updateObjects();

        G_DRAW_COUNT = 0;
        render();
        gui.render();

        glfwSwapBuffers(window);
        glfwPollEvents();        

        if (glfwWindowShouldClose(window)) {
            app->quit();
        }
    }
}

ResourceManager* Engine::getResourceManager()
{
    return &manager;
}

Camera* Engine::getCamera()
{
    return camera.get();
}

Input* Engine::getInput()
{
    return &input;
}

RenderSettings* Engine::getRenderSettings()
{
    return &renderSettings;
}

Time*Engine::getTime()
{
    return &time;
}

Object* Engine::createObject(const std::string& name)
{
    std::shared_ptr<Object> obj(new Object(name));
    allObjects.push_back(obj);
    return obj.get();
}

bool Engine::loadLevel(const std::string& level)
{
    std::string lvl = manager.getLevelPath() + level;
    std::ifstream ifs(lvl.c_str());
    if (!ifs) {
        std::cerr << "WARNING: Could not open level file: " << level << "\n";
        return false;
    }

    std::cout << "\nLoading a new level: " << level << "\n\n";
    resetLevel();

    std::string word;

    Object* obj = nullptr;
    try {
        float x, y, z, w;
        bool a, b;
        while (ifs) {
            ifs >> word;
            boost::trim(word);
            if (word.empty()) {
                continue;
            } else if (word[0] == '#') {
                std::getline(ifs, word);
                continue;
            } else if (word == "general") {
                ifs >> x >> y >> z;
                renderSettings.ambientColor = glm::vec3(x, y, z);
                ifs >> word;
                Material* material = manager.createMaterial();
                material->setTexture(manager.getTexture(word), Material::TextureType::DIFFUSE, GL_TEXTURE_2D);
                material->setShaderType(Shader::DIFFUSE);
                Object::setMeshDefaultMaterial(material);
                word.clear();
            } else if (word == "skybox") {
                std::vector<std::string> skyboxTextures(6, "");
                for (int i = 0; i < 6; ++i) {
                    ifs >> skyboxTextures[i];
                }
                renderSettings.skyboxTextures = skyboxTextures;
                if (!createSkybox()) {
                    std::cerr << "WARNING: Failed to create the skybox\n";
                }
            } else if (word == "name") {
                ifs >> word;
                obj = createObject(word);
                ifs >> x >> y >> z;
                obj->setPosition(glm::vec3(x, y, z));
                ifs >> x >> y >> z;
                obj->setRotation(glm::vec3(x, y, z));
                ifs >> x >> y >> z;
                obj->setScale(glm::vec3(x, y, z));
                ifs >> a >> b;
                obj->setShadowCaster(a);
                obj->setShadowReceiver(b);
                word.clear();
            } else if (word == "component") {
                if (!obj) {
                    throw std::runtime_error("Component without an object");
                }
                ifs >> word;
                if (word == "model") {
                    ifs >> word;
                    Model* modelComponent = manager.getModel(word);
                    obj->addComponent<Model>(modelComponent);
                    word.clear();
                } else if (word == "light") {
                    Light* lightComponent = obj->addComponent<Light>();
                    ifs >> x >> y >> z >> w;
                    lightComponent->setColor(glm::vec4(x, y, z, w));
                    ifs >> word;
                    if (word == "point") {
                        lightComponent->setType(Light::Type::POINT);
                    } else if (word == "directional") {
                        lightComponent->setType(Light::Type::DIRECTIONAL);
                    } else {
                        throw std::runtime_error("Unknown light type: " + word);
                    }
                    ifs >> a;
                    lightComponent->setShadowingEnabled(a);
                    word.clear();
                } else {
                    throw std::runtime_error("Component name is " + word);
                }
            } else {
                throw std::runtime_error("Object value is " + word);
            }
        }
    } catch (std::exception& e) {
        std::cerr << "WARNING: Could not parse level: " << level << " - " << e.what() << "\n";
        return false;
    }
    G_COMPONENT_CHANGED = true;
    app->levelLoaded();
    return true;
}

unsigned int Engine::getDrawCount() const
{
    return G_DRAW_COUNT;
}

void Engine::resetLevel()
{
    renderMeshes.clear();
    lights.clear();
    lights.resize(Light::Type::NUM_TYPES);
    allObjects.clear();
    manager.clear();
    skybox.reset();
}

void Engine::render()
{
    fb.bind();
    objectsInFrustum.clear();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    Object::setViewMatrixUniform();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    shader = renderSettings.ambientShader;
    glUseProgram(renderSettings.ambientShader->getProgram());
    glUniform3f(AMBIENT_LOCATION, renderSettings.ambientColor.x, renderSettings.ambientColor.y, renderSettings.ambientColor.z);
    for (auto& shaderMeshMap : renderMeshes) {
        for (auto& meshMap : shaderMeshMap .second) {
            manager.getMaterial(meshMap.first)->setUniforms(shader);
            for (auto& meshObject : meshMap.second) {
                if (!objectInsideFrustum(meshObject)) {
                   continue;
                }
                meshObject.parent->setUniforms(shader);
                meshObject.mesh->render();
                objectsInFrustum.insert(meshObject.mesh->getId());
            }
        }
    }

    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ONE);

    lighting(Light::DIRECTIONAL);
    lighting(Light::POINT);

    glDisable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);

    if (skybox) {
        glCullFace(GL_FRONT);
        shader = renderSettings.skyboxShader;
        glUseProgram(shader->getProgram());
        skybox->setUniforms(shader);
        for (auto& meshObject : skybox->getMeshObjects()) {
            meshObject.material->setUniforms(shader);
            meshObject.mesh->render();
        }
        glCullFace(GL_BACK);
    }

    const std::list<Postprocess>& postprocs = camera->getPostprocesses();
    GLuint renderedTex = blitBuffer1.blit(fb.getFramebuffer(), 0);

    if (camera->getBloomIterations() > 0) {
        GLuint bloomTex = blitBuffer2.blit(fb.getFramebuffer(), 1);
        glUseProgram(manager.getShader("bloom_blur")->getProgram());
        GLboolean horizontal = true;
        for (unsigned int i = 0; i < camera->getBloomIterations(); ++i) {
            glUniform1i(BLOOM_FILTER_HORIZONTAL, horizontal);
            setPostFramebuffer();
            bloomTex = postBuffer->draw(std::vector<GLuint>(1, bloomTex));
            horizontal = !horizontal;
        }
        glUseProgram(manager.getShader("bloom_blend")->getProgram());
        renderedTex = blitBuffer2.draw(std::vector<GLuint>{renderedTex, bloomTex});
    }

    if (camera->isHDREnabled()) {
        glUseProgram(manager.getShader("hdr")->getProgram());
        setPostFramebuffer();
        renderedTex = postBuffer->draw(std::vector<GLuint>{renderedTex});
    }

    for (auto iter = postprocs.begin(); iter != postprocs.end(); ++iter) {
        iter->bind();
        setPostFramebuffer();
        renderedTex = postBuffer->draw(std::vector<GLuint>(1, renderedTex));
    }

    passthrough.bind();
    setPostFramebuffer();
    postBuffer->setInputTextures(std::vector<GLuint>(1, renderedTex));
    postBuffer->activate();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Engine::lighting(Light::Type lightType)
{
    DepthMap* depthMap = nullptr;
    if (lightType == Light::DIRECTIONAL) {
        depthMap = &depthMapDir;
    } else {
        depthMap = &depthMapPoint;
    }

    for (auto& light : lights[lightType]) {
        shader = manager.getShader(Shader::DEPTH, lightType);
        glUseProgram(shader->getProgram());
        Light* lightComp = light->getComponent<Light>();
        bool shadowingEnabled = lightComp->isShadowingEnabled();
        lightComp->setUniforms(light->getPosition(), light->getForward());
        depthMap->bind(light->getPosition(), light->getForward());
        if (shadowingEnabled) {
            for (auto& shaderMeshMap : renderMeshes) {
                for (auto& meshMap : shaderMeshMap.second) {
                    for (auto& meshObject : meshMap.second) {
                        if (meshObject.parent->isShadowCaster()) {
                            meshObject.parent->setUniforms(shader);
                            meshObject.mesh->render();
                        }
                    }
                }
            }
        }

        fb.bind();

        for (auto& shaderMeshMap : renderMeshes) {
            shader = manager.getShader(shaderMeshMap.first, lightType);
            glUseProgram(shader->getProgram());
            if (shader->hasUniform(FAR_CLIP_DISTANCE_LOCATION)) {
                glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
            }
            depthMap->activate();
            for (auto& meshMap : shaderMeshMap.second) {
                manager.getMaterial(meshMap.first)->setUniforms(shader);
                for (auto& meshObject : meshMap.second) {
                    if (objectsInFrustum.find(meshObject.mesh->getId()) == objectsInFrustum.end()) {
                        continue;
                    }
                    meshObject.parent->setUniforms(shader);
                    meshObject.mesh->render();
                }
            }
        }
    }
}

void Engine::updateObjectContainers()
{
    if (!G_COMPONENT_CHANGED) {
        return;
    }

    for (const auto obj : allObjects) {
        for (auto& meshObject : obj->getMeshObjects()) {
            int shaderType = meshObject.material->getShaderType();
            MaterialId materialId = meshObject.material->getId();
            std::vector<Object::MeshObject>& meshes = renderMeshes[shaderType][materialId];
            if (std::find(meshes.begin(), meshes.end(), meshObject) == meshes.end()) {
                meshes.push_back(meshObject);
            }
        }
        if (obj->hasComponent<Light>()) {
            Light::Type type = obj->getComponent<Light>()->getLightType();
            std::vector<Object*>& objs = lights[type];
            if (std::find(objs.begin(), objs.end(), obj.get()) == objs.end()) {
                lights[type].push_back(obj.get());
            }
        }
    }
    for (auto& objs : lights) {
        objs.erase(std::remove_if(objs.begin(), objs.end(), [] (Object* obj) { return !obj->hasComponent<Light>(); }),
                objs.end());
    }

    for (auto& shaderMeshMap : renderMeshes) {
        for (auto& meshMap : shaderMeshMap.second) {
            std::vector<Object::MeshObject>& meshes = meshMap.second;
            meshes.erase(std::remove_if(meshes.begin(), meshes.end(), [] (Object::MeshObject mo) { return mo.parent == nullptr; }),
                    meshes.end());
        }
    }

#if DEBUG
    manager.checkMissingTextures();
#endif
    G_COMPONENT_CHANGED = false;
}

void Engine::updateObjects()
{
    camera->updateViewMatrix();
    Object::updateViewProjectionMatrix();
    for (const std::shared_ptr<Object>& obj : allObjects) {
        obj->updateModelMatrix();
    }
    if (skybox) {
        skybox->setPosition(camera->getPosition());
        skybox->updateModelMatrix();
    }
}

void Engine::printInfo(int windowWidth, int windowHeight)
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "GFX: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL: "  << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    std::cout << "Resolution: " << mode->width << " x " << mode->height << "\n";
    std::cout << "Refresh rate: " << mode->refreshRate << " Hz\n";
    int monitorWidth, monitorHeight;
    glfwGetMonitorPhysicalSize(primaryMonitor, &monitorWidth, &monitorHeight);
    std::cout << "Monitor size: " << monitorWidth << "mm x " << monitorHeight << "mm\n\n";

    std::cout << "Window resolution: " << windowWidth << " x " << windowHeight << "\n\n";
}

bool Engine::createSkybox()
{
    if (!renderSettings.isLoaded()) {
        return false;
    }

    skybox.reset(new Object());
    skybox->addComponent<Model>(manager.getModel("cube.3ds"));

    Material* material = manager.createMaterial();
    GLuint texture = manager.getCubeTexture(renderSettings.skyboxTextures);
    material->setTexture(texture, Material::TextureType::DIFFUSE, GL_TEXTURE_CUBE_MAP);
    for (auto& meshObject : skybox->getMeshObjects()) {
        meshObject.material = material;
    }

    skybox->setShadowCaster(false);
    skybox->setShadowReceiver(false);

    return true;
}

bool Engine::objectInsideFrustum(const Object::MeshObject& mo) const
{
    glm::vec3 point = mo.mesh->getCenterPoint();
    point = glm::vec3((*Object::view) * mo.parent->getModelMatrix() * glm::vec4(point.x, point.y, point.z, 1.0f));
    glm::vec3 scale = mo.parent->getScale();
    float scaleMultiplier = std::max(std::max(scale.x, scale.y), scale.z);
    float radius = mo.mesh->getBoundingRadius() * scaleMultiplier ;
    return camera->sphereInsideFrustum(point, radius);
}

void Engine::setPostFramebuffer()
{
    postBuffer = postBuffer != &postBuffer1 ? &postBuffer1 : &postBuffer2;
}

} // moar
