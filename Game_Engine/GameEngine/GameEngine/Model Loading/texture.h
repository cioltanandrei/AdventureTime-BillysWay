#pragma once
#include <glew.h>
#include <glfw3.h>

GLuint loadBMP(const char * imagepath);
unsigned int loadCubemap(std::vector<std::string> faces);