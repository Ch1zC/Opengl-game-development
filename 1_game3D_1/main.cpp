#include <glad/glad.h>
#include <glfw3.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map> 

//texture loader
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//model loader
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tinyobjloader/tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "modelsLoader.h"
#include "cameraController.h"

#include "structs.h"

const unsigned int WINDOW_WIDTH  = 1024;
const unsigned int WINDOW_HEIGHT = 768;

void framebuffer_size_callback(GLFWwindow*, int, int);

std::vector<ModelsNeedToShow> loadLevelData(const std::string&);

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D texture_diffuse;

    void main()
    {
        FragColor = texture(texture_diffuse, TexCoord);
    }
)glsl";


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    CameraController cameraController;

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, &cameraController);
    glfwSetCursorPosCallback(window, CameraController::mouse_callback_s);
    stbi_set_flip_vertically_on_load(true);

    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader Program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<ModelsNeedToShow> ModelsNeedToShow_data = loadLevelData("levelData/test_level.txt");

    std::unordered_map<std::string, ModelResource> resourceManager;
    std::vector<SceneObject> sceneObjects;

    ModelsLoader modelsLoader;

    for (const auto& obj : ModelsNeedToShow_data) {

        glm::mat4 model = obj.transform;

        if (resourceManager.find(obj.modelPath) == resourceManager.end()) {

            modelsLoader_struct model_data = modelsLoader.load(obj.modelPath);
            unsigned int VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, model_data.vertices.size() * sizeof(float), model_data.vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            resourceManager[obj.modelPath] = { VAO, VBO, model_data.diffuseMap, (unsigned int)model_data.vertices.size() / 5, model_data.localAABB };
        }

        sceneObjects.push_back({&resourceManager[obj.modelPath], model});
    }


    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();

        cameraController.updateTime(currentFrame);
        cameraController.processInput(window, sceneObjects);

        glm::mat4 view = cameraController.get_view_lookAt();

        glUseProgram(shaderProgram);

        for (const auto& object : sceneObjects) {
        
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object.resource->textureID);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);

            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
            unsigned int projLoc  = glGetUniformLocation(shaderProgram, "projection");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(object.modelMatrix));
            glUniformMatrix4fv(viewLoc , 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc , 1, GL_FALSE, glm::value_ptr(projection));

            glBindVertexArray(object.resource->VAO);
            glDrawArrays(GL_TRIANGLES, 0, object.resource->vertexCount);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto const& x : resourceManager) {
        glDeleteVertexArrays(1, &x.second.VAO);
        glDeleteBuffers(1, &x.second.VBO);
    }
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::vector<ModelsNeedToShow> loadLevelData(const std::string& filePath) {
    // 读取场景txt，将各个模型数据转换存储到后台

    std::vector<ModelsNeedToShow> dataVector;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "-FAILED to load-[Txt] path: " << filePath << std::endl;
        return dataVector;
    }

    std::string line;
    std::string currentPath;
    glm::mat4 blenderMatrix(1.0f); // 用于存储从文件读取的原始Blender矩阵

    while (std::getline(file, line)) {

        if (line.rfind("MODEL:", 0) == 0) {
            currentPath = line.substr(6);
        }

        else if (line.rfind("MATRIX:", 0) == 0) {
            std::string matrixData = line.substr(7);
            std::stringstream ss(matrixData);
            std::string value;

            // 将16个浮点数读入一个临时数组
            float matrixElements[16];
            int i = 0;
            while (std::getline(ss, value, ',') && i < 16) {
                matrixElements[i++] = std::stof(value);
            }

            // 创建vec4矩阵，并将行主序转到opengl的列主序
            blenderMatrix = glm::transpose(glm::make_mat4(matrixElements));
        }
        else if (line == "---") {

            // 转换坐标，从Z头顶转成Y头顶（尝试导出直接Y头顶，省去这行）
            glm::mat4 coordSystemFix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            // 二者相乘，得出最终在世界中的位置
            glm::mat4 finalModelMatrix = coordSystemFix * blenderMatrix;


            dataVector.push_back({ currentPath, finalModelMatrix });

            blenderMatrix = glm::mat4(1.0f);
        }
    }
    return dataVector;
}

