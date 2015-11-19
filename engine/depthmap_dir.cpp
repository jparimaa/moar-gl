#include "depthmap_dir.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace moar
{

DepthMapDirectional::DepthMapDirectional() :
    // Todo: Proper frustum calculation / CSM.
    frustum(-10.0f, 10.0f, -10.0f, 10.0f),
    projectionMatrix(glm::ortho(frustum.x, frustum.y, frustum.z, frustum.w, nearClipDistance, farClipDistance))
{    
}

DepthMapDirectional::~DepthMapDirectional()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &depthTexture);
}

bool DepthMapDirectional::init(GLuint shader)
{
    if (width == 0 || height == 0) {
        std::cerr << "ERROR: Shadow map width or height not initilized." << std::endl;
    }

    this->shader = shader;

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    bool status = createFramebuffer(framebuffer, depthTexture, false);
    return status;
}

void DepthMapDirectional::bind(const glm::vec3& lightPos, const glm::vec3& lightDir)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader);
    glm::mat4 viewMatrix = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(LIGHT_SPACE_PROJ_LOCATION, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void DepthMapDirectional::activate()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(DEPTH_TEX_LOCATION, 0);
    glUniformMatrix4fv(LIGHT_SPACE_PROJ_LOCATION, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

} // moar
