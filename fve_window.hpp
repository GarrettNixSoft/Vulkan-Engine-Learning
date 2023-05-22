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

		bool shouldClose() { return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS; }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		GLFWwindow* window;
		int width;
		int height;
		bool framebufferResized;
		std::string windowName;

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		void initWindow();

	};

}