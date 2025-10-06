#include <glfw3.h>
#include <iostream>

#include "AABB.h"
#include "structs.h"
#include "cameraController.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CameraController::CameraController():
    deltaTime(0.0f),
    lastFrame(0.0f),
    firstMouse(true),
    yaw(-90.0f),
    pitch(0.0f),
    lastX(0.0),
    lastY(0.0),
    cameraPos(glm::vec3(0.0f, 1.71f, 3.0f)),
    cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
    cameraUp(glm::vec3(0.0f, 1.0f, 0.0f))
{}

void CameraController::mouse_callback_s(GLFWwindow* window, double x, double y) {

    CameraController* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (controller) controller->mouse_callback(x, y);
}

void CameraController::mouse_callback(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // ע�⣺y�Ƿ���
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // �����ӽǽǶ�
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // ����yaw pitch���㷽������
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

bool CameraController::playerModelChecker(const AABB& playerAABB, const std::vector<SceneObject>& sceneObjects) {
    // ������AABBȥ�ͳ�������objects��AABB���ص��жϣ��Դﵽ�޷���ģ��Ч��
    // return true:  ��ģ��
    // return false: û��

    AABB_class AABBClass;

    for (const auto& object : sceneObjects) {

        // ������������������е�AABB
        AABB objectAABB = AABBClass.calculateWorldAABB(object.resource->localAABB, object.modelMatrix);


        if (AABBClass.checkAABBCollision(playerAABB, objectAABB)) return true;
    }

    return false;
}

void CameraController::processInput(GLFWwindow* window, const std::vector<SceneObject>& sceneObjects) {
    // camera controller core code

    float cameraSpeed = 4.5f * deltaTime;
    float cameraSpeed_running = 8.5f * deltaTime;

    glm::vec3 moveForward = glm::vec3(cameraFront.x , 0.0f, cameraFront.z);
    moveForward = glm::normalize(moveForward);

    glm::vec3 moveRight = glm::normalize(glm::cross(moveForward, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 moveDirection(0.0f);   // ���Դ洢�´��ƶ����ݣ����AABB���ص���������ټ�ֵ��cameraPos���ƶ�

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed = cameraSpeed_running;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDirection += cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDirection -= cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDirection -= glm::normalize(glm::cross(cameraFront, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDirection += glm::normalize(glm::cross(cameraFront, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glm::length(moveDirection) != 0.0f) {

        moveDirection = glm::normalize(moveDirection) * cameraSpeed; // (������֡��֮��)����normalize�ٶȵ�����������б���ƶ��ٶ���ȻΪ1.0

        AABB playerAABB;
        playerAABB.min = glm::vec3(-0.3f, -1.7f, -0.2f); // player ��   y = -1.7
        playerAABB.max = glm::vec3(0.3f, 0.1f, 0.2f);    // player ͷ�� y = 0.1

        //x
        cameraPos.x += moveDirection.x;
        AABB playerAABB_X = { cameraPos + playerAABB.min, cameraPos + playerAABB.max };

        if (playerModelChecker(playerAABB_X, sceneObjects)) {
            cameraPos.x -= moveDirection.x;
        }

        //y
        /*cameraPos.y += moveDirection.y;
        AABB playerAABB_Y = { cameraPos + playerAABB.min, cameraPos + playerAABB.max };

        if (playerModelChecker(playerAABB_Y, sceneObjects)) {
            cameraPos.y -= moveDirection.y;
        }*/

        //z
        cameraPos.z += moveDirection.z;
        AABB playerAABB_Z = { cameraPos + playerAABB.min, cameraPos + playerAABB.max };

        if (playerModelChecker(playerAABB_Z, sceneObjects)) {
            cameraPos.z -= moveDirection.z;
        }
    }

    // std::cout << "Player Pos: (" << cameraPos.x << ", " << cameraPos.y << ")" << std::endl;
}

void CameraController::updateTime(float currentFrame) {

    set_deltaTime(currentFrame - get_lastFrame());
    set_lastFrame(currentFrame);

}

glm::mat4 CameraController::get_view_lookAt() { return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); }