#ifndef _CAPTURE_H
#define _CAPTURE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <IL/il.h>
#include <IL/ilu.h>

void capture(const glm::mat4 &MVP, const GLfloat g_vertex_buffer_data[], const GLfloat g_color_buffer_data[]);

#endif