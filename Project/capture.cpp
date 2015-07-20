#include <cstdio>
#include <iostream>
#include "capture.h"

using namespace std;

bool check = true;

void capture(const glm::mat4 &MVP, const GLfloat g_vertex_buffer_data[], const int g_vertex_buffer_data_size, const GLfloat g_color_buffer_data[])
{
	vector<triangle> triangles(g_vertex_buffer_data_size / 3 / 3);
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	vector<glm::vec4> vertexPosition;
	glm::vec4 tmp;

	for (int i = 0; i < g_vertex_buffer_data_size / 3; i++)
	{
		triangles[i / 3].vertices[i % 3] = MVP * glm::vec4(g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2], 1);
		for (int j = 0; j < 4; j++)
		{
			triangles[i / 3].vertices[i % 3][j] /= triangles[i / 3].vertices[i % 3].w; // normalize the frustrum
			triangles[i / 3].vertices[i % 3][j] += 1; // change the range from -1~1 to 0~2
			triangles[i / 3].vertices[i % 3][j] /= 2; // compress the range from 0~2 to 0~1
		}
	}

	ilInit();
	int width = viewportSize[2] - viewportSize[0], height = viewportSize[3] - viewportSize[1];
	int bytesToUsePerPixel = 3;
	int sizeOfByte = sizeof(unsigned char);
	int theSize = width * height * sizeOfByte * bytesToUsePerPixel;
	unsigned char* imData = new unsigned char [theSize];
	vector< vector<pixel> > viewport(height, vector<pixel>(width));
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			viewport[i][j].position = glm::vec4(1.0 / width * j, 1.0 / height * i, 1.0, 1.0);
		}
	}

	for (int i = 0; i < triangles.size(); i++)
	{
		triangle triangleOnPlane;
		int screenX, screenY;
		for (int j = 0; j < 3; j++)
		{
			screenX = triangles[i].vertices[j].x * width, screenY = triangles[i].vertices[j].y * height;
			triangleOnPlane.vertices[j] = glm::vec4(screenX, screenY, 0.0, 1.0);
			for (int k = 0; k < 3; k++)
			{
				viewport[screenY][screenX].RGB[k] = 255;
			}
		}
		for (int j = 0; j < 3; j++)
		{
			glm::vec4 vector = triangleOnPlane.vertices[(j + 1) % 3] - triangleOnPlane.vertices[j % 3];
			float len = glm::length(vector);
			glm::vec4 normalizedVec = glm::normalize(vector);
			int scale = 1;
			vector = (float)scale * normalizedVec;
			while (glm::length(vector) < len)
			{
				for (int k = 0; k < 3; k++)
				{
					viewport[triangleOnPlane.vertices[j].y + (int)vector.y][triangleOnPlane.vertices[j].x + (int)vector.x].RGB[k] = 255;
				}
				++scale;
				vector = (float)scale * normalizedVec;
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			imData[3 * (i * width + j)] = viewport[i][j].RGB[0];
			imData[3 * (i * width + j) + 1] = viewport[i][j].RGB[1];
			imData[3 * (i * width + j) + 2] = viewport[i][j].RGB[2];
		}
	}

	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	printf("New image! width=%d, height=%d, bpp=%d\n", ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP));
	printf("About to tex up your image\n");
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, imData);
	printf("Your image was texed\n");
	printf("Now width=%d, height=%d, bpp=%d\n", ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP));
	ilSetData(imData);
	ilEnable(IL_FILE_OVERWRITE);
	ilSave(IL_PNG, "output.png");
}