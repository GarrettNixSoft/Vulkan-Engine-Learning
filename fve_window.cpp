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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		// create the window
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		//glfwSetWindowUserPointer(window, this);
		//glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	/*void fvewindow::framebufferresizecallback(glfwwindow* window, int width, int height) {
		auto fvewindow = reinterpret_cast<fvewindow*>(glfwgetwindowuserpointer(window));
		fvewindow->framebufferresized = true;
		fvewindow->width = width;
		fvewindow->height = height;
		std::cout << "window resized to (" << width << " x " << height << ")" << std::endl;
	}*/

	void FveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

}