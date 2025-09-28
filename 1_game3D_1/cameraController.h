#pragma once
#include <glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "structs.h"

class CameraController {
public:
	CameraController();
	void mouse_callback(double, double);
	void processInput(GLFWwindow*, const std::vector<SceneObject>&);
	bool playerModelChecker(const AABB&, const std::vector<SceneObject>&);
	void updateTime(float);
	void set_deltaTime(float d) { deltaTime = d; }
	void set_lastFrame(float l) { lastFrame = l; }
	float get_deltaTime() { return deltaTime; }
	float get_lastFrame() { return lastFrame; }
	glm::mat4 get_view_lookAt();

	static void mouse_callback_s(GLFWwindow*, double, double);

private:
	float deltaTime;
	float lastFrame;
	bool firstMouse;
	double lastX;
	double lastY;
	float yaw;
	float pitch;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
};