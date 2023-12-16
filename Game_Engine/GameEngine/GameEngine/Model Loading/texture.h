#pragma once
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <string>

using namespace std;

GLuint loadBMP(const char * imagepath);
GLuint loadCubemap(vector<string> faces);