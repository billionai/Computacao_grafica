#ifndef DISPLAY_MANAGER
#define DISPLAY_MANAGER

//standard libraries
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <vector>
#include <set>

//graphic libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class DisplayManager{
private:
	//members used for GLFW library
	GLFWwindow* window;

	//members used for Vulkan library
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	//General Purpose Members
	const int width, height;

	//private methods
	void initWindow();
	void initVulkan();
	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();

public:
	DisplayManager(int = 800, int = 600);
	~DisplayManager();
};

#endif
