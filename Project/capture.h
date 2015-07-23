#ifndef _CAPTURE_H
#define _CAPTURE_H

#define GLM_SWIZZLE

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glfw3.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <ctime>

class triangle
{
public:
	glm::vec4 vertices[3];
};

class pixel
{
public:
	glm::vec4 position;
	unsigned char RGB[4];
	pixel()
	{
		GLfloat clearColor[4];
		glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
		for (int i = 0; i < 3; i++)
		{
			RGB[i] = static_cast<unsigned char>(255 * clearColor[i]);
		}
	}
};

void capture(GLFWwindow* &window, const glm::mat4 &MVP, const GLfloat g_vertex_buffer_data[], const int g_vertex_buffer_data_size, const GLfloat g_color_buffer_data[]);

#endif