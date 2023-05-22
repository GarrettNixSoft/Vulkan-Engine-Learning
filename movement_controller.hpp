#pragma once

#include "fve_game_object.hpp"
#include "fve_window.hpp"

namespace fve {
	class MovementController {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_SPACE;
			int moveDown = GLFW_KEY_LEFT_CONTROL;
			int sprint = GLFW_KEY_LEFT_SHIFT;
		};

		struct MouseMappings {
			int select = GLFW_MOUSE_BUTTON_LEFT;
			int attack = GLFW_MOUSE_BUTTON_LEFT;
			int interact = GLFW_MOUSE_BUTTON_RIGHT;
		};

		KeyMappings keys;
		float moveSpeed{ 3.0f };

		bool firstMouse = true;
		double lastX{ 0.0 };
		double lastY{ 0.0 };

		float fov = 50.0f;

		void init(GLFWwindow* window, int width, int height);

		void update(GLFWwindow* window);

		void moveInPlaneXZ(GLFWwindow* window, float dt, FveGameObject& gameObject);

	};
}