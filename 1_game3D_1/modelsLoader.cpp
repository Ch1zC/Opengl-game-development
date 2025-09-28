#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <stb_image.h>
#include "modelsLoader.h"
#include "tinyobjloader/tiny_obj_loader.h"

modelsLoader_struct ModelsLoader::load(std::string objPath) {

    std::string warn, err;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string parentDir = "";
    size_t last_slash_idx = objPath.rfind('/');
    if (std::string::npos != last_slash_idx) {
        parentDir = objPath.substr(0, last_slash_idx + 1);
    }
    
    //                                               models obj file's path-.                  .- models mtl file's path
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), parentDir.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::vector<float> vertices;

    AABB localAABB;
    if (!attrib.vertices.empty()) {
        localAABB.min = glm::vec3(attrib.vertices[0], attrib.vertices[1], attrib.vertices[2]);
        localAABB.max = localAABB.min;

        // 遍历所有原始顶点来确定边界
        for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
            float x = attrib.vertices[i + 0];
            float y = attrib.vertices[i + 1];
            float z = attrib.vertices[i + 2];

            localAABB.min.x = std::min(localAABB.min.x, x);
            localAABB.min.y = std::min(localAABB.min.y, y);
            localAABB.min.z = std::min(localAABB.min.z, z);

            localAABB.max.x = std::max(localAABB.max.x, x);
            localAABB.max.y = std::max(localAABB.max.y, y);
            localAABB.max.z = std::max(localAABB.max.z, z);
        }
    }

    for (const auto& shape : shapes) {

        for (const auto& index : shape.mesh.indices) {

            // xyz
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            //uv
            if (index.texcoord_index >= 0) {
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            }
            else {
                // def uv value
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }
    }

    if (!warn.empty()) std::cout << "TINYOBJLOADER WARNING: " << warn << std::endl;
    if (!err.empty() ) std::cerr << "TINYOBJLOADER ERROR: "   << err  << std::endl;

    unsigned int diffuseMap = 0;
    bool textureLoaded = false;

    if (!materials.empty()) {

        for (const auto& material : materials) {
            if (!material.diffuse_texname.empty()) {
                std::string texture_path = material.diffuse_texname;
                std::string texPath = parentDir + "/" + texture_path;
                diffuseMap = loadTexture(texPath.c_str());

                textureLoaded = true;
                break;
            }
        }
    }
    if (!textureLoaded) {
        std::cout << "Could not find any material with a texture." << std::endl;
    }

    return modelsLoader_struct{ vertices, diffuseMap, localAABB };
}

unsigned int ModelsLoader::loadTexture(const char* path) {

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {

        GLenum format;

        if      (nrComponents == 1) format = GL_RED ;
        else if (nrComponents == 3) format = GL_RGB ;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture loaded at path: " << path << std::endl;
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    return textureID;
}