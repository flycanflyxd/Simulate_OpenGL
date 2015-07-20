#include <cstdio>
#include <iostream>
#include <vector>
#include "capture.h"

using namespace std;

void capture(const glm::mat4 &MVP, const GLfloat g_vertex_buffer_data[], const GLfloat g_color_buffer_data[])
{
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	vector<glm::vec4> vertexPosition;
	glm::vec4 tmp;
	for (int i = 0; i < 36; i++)
	{
		tmp = MVP * glm::vec4(g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2], 1);
		for (int j = 0; j < 4; j++)
		{
			tmp[j] /= tmp[3];
			tmp[j] += 1;
			tmp[j] /= 2;
			//cout << tmp[j] << " ";
		}
		//cout << endl;
		vertexPosition.push_back(tmp);
	}
	ilInit();
	int width = viewportSize[2] - viewportSize[0], height = viewportSize[3] - viewportSize[1];
	int bytesToUsePerPixel = 3;
	int sizeOfByte = sizeof(unsigned char);
	int theSize = width * height * sizeOfByte * bytesToUsePerPixel;
	unsigned char* imData = new unsigned char [theSize];
	for (int i = 0; i < vertexPosition.size(); i++)
	{
		int screenX = vertexPosition[i].x * width, screenY = vertexPosition[i].y * height;
		int index = ((screenY - 1) * width + screenX) * 3;
		if (index < 0)
		{
			index = 0;
		}
		for (int k = 0; k < 21; k += 3)
		{
			for (int j = 0; j < 3; j++)
			{
				imData[index + j + k] = 0;
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