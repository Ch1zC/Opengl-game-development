#pragma once

#include <iostream>
#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

struct ModelsNeedToShow {
    std::string modelPath;
    glm::mat4 transform;
};

struct ModelResource {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int textureID;
    unsigned int vertexCount;
    AABB localAABB;
};

struct SceneObject {
    ModelResource* resource;
    glm::mat4 modelMatrix;
};