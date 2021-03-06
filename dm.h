#ifndef DISPLAY_MANAGER
#define DISPLAY_MANAGER

//#define DEBUG
#define MAX_VERTICES 4

//standard libraries
#include <iostream>
#include <fstream>
#include <exception>
#include <cstring>
#include <string>
#include <vector>

#include "VAO.h"

class DisplayManager{
private:
	//members used for GLFW library
	GLFWwindow* window;
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;


	//General Purpose Members
	int dimensions[2];// width, height

	std::vector<VAO_INFO> VAO_array;

public:
	DisplayManager();
	~DisplayManager();

	void init(int = 800, int = 600, const char* = nullptr);
	void run();

	void register_VAO(VAO_INFO);
	void deregister_VAO(VAO_INFO);
	void update_VAO(VAO_INFO);
	void registerMouseButtonCallback(void (*mouseFunc)(GLFWwindow*,int,int,int));

	GLFWwindow* getWindow(){return window;}
	unsigned int getShader(){return shaderProgram;}

	void setClearColor(float,float,float);
};

#endif
