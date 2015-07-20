#ifndef _CAPTURE_H
#define _CAPTURE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <vector>

class triangle
{
public:
	glm::vec4 vertices[3];
};

class pixel
{
public:
	glm::vec4 position;
	unsigned char RGB[3];
	pixel()
	{
		for (int i = 0; i < 3; i++)
		{
			RGB[i] = 0;
		}
	}
};

void capture(const glm::mat4 &MVP, const GLfloat g_vertex_buffer_data[], const int g_vertex_buffer_data_size, const GLfloat g_color_buffer_data[]);

#endif