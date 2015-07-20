#include <cstdio>
#include <iostream>
#include "capture.h"

using namespace std;

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
	/*for (int i = 0; i < triangles.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				cout << triangles[i].vertices[j][k] << " ";
			}
			cout << endl;
		}
	}*/

	ilInit();
	int width = viewportSize[2] - viewportSize[0], height = viewportSize[3] - viewportSize[1];
	int bytesToUsePerPixel = 3;
	int sizeOfByte = sizeof(unsigned char);
	int theSize = width * height * sizeOfByte * bytesToUsePerPixel;
	unsigned char* imData = new unsigned char [theSize];
	
	for (int i = 0; i < triangles.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int screenX = triangles[i].vertices[j].x * width, screenY = triangles[i].vertices[j].y * height;
			int index = ((screenY - 1) * width + screenX) * 3;
			if (index < 0)
			{
				index = 0;
			}
			for (int l = 0; l < 21; l += 3)
			{
				for (int k = 0; k < 3; k++)
				{
					imData[index + k + l] = 0;
				}
			}
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