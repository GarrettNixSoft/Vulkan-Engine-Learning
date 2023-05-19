#include "fve_window.h"

#include <iostream>
#include <stdexcept>

namespace fve{

	FveWindow::FveWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	FveWindow::~FveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void FveWindow::initWindow() {
		// init GLFW
		if (glfwInit() != GLFW_TRUE) {
			throw std::runtime_error("Failed to initialize GLFW");
		}
		// tell GLFW not to create an OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// disable resizing for now
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		// create the window
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void FveWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto fveWindow = reinterpret_cast<FveWindow*>(glfwGetWindowUserPointer(window));
		fveWindow->framebufferResized = true;
		fveWindow->width = width;
		fveWindow->height = height;
	}

	void FveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

}