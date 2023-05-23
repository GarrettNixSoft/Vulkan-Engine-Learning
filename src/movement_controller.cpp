#include "movement_controller.hpp"

#include <stdexcept>

// print debugging
#include <iostream>

namespace fve {

	void MovementController::init(GLFWwindow* window, int width, int height) {
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			//glfwSetCursorPosCallback(window, mouseCallback);
			//glfwSetScrollCallback(window, scrollCallback);
		}
		else
			throw std::runtime_error("Raw mouse motion unsupported");
	}

	void MovementController::update(GLFWwindow* window) {
		/*double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		mouseDeltaX = xpos - lastMouseX;
		mouseDeltaY = ypos - lastMouseY;
		lastMouseX = xpos;
		lastMouseY = ypos;*/
	}

	void MovementController::moveInPlaneXZ(GLFWwindow* window, float dt, FveGameObject& gameObject) {

		double xpos{}, ypos{};
		glfwGetCursorPos(window, &xpos, &ypos);

		if (firstMouse) // initially set to true
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		//if (abs(mouseDeltaX) > std::numeric_limits<float>::epsilon()) rotate.y += mouseDeltaX * lookSensitivityX;
		//if (abs(mouseDeltaY) > std::numeric_limits<float>::epsilon()) rotate.x -= mouseDeltaY * lookSensitivityY;

		float xOffset = xpos - lastX;
		float yOffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;

		const float sensitivity = 0.45f;

		glm::vec3 rotate{ 0 };
		if (abs(xOffset) > std::numeric_limits<float>::epsilon()) {
			rotate.y += xOffset;
		}
		if (abs(yOffset) > std::numeric_limits<float>::epsilon()) {
			rotate.x -= yOffset;
		}

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			//gameObject.transform.rotation += sensitivity * dt * glm::normalize(rotate);
			gameObject.transform.rotation += sensitivity * dt * rotate;
		}

		// clamp pitch
		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		// mod yaw
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		float speedModifier = 1.0f;
		if (glfwGetKey(window, keys.sprint)) speedModifier = 5.0f;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.translation += moveSpeed * speedModifier * dt * glm::normalize(moveDir);
		}

		// FOV changes?
		//

	}

	void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		//
	}

	void scrollCallback(GLFWwindow* window, double xoffset, double yOffset) {
		//
	}

}