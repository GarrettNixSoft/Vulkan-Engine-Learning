#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace fve {

	class FveWindow {
	public:
		FveWindow(int w, int h, std::string name);
		~FveWindow();

		FveWindow(const FveWindow&) = delete;
		FveWindow& operator=(const FveWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		GLFWwindow* window;
		const int width;
		const int height;
		std::string windowName;

		void initWindow();

	};

}