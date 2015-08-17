#include "capture.h"

using namespace std;

bool check = true;

void capture(GLFWwindow* &window, const glm::mat4 &MVP, const GLfloat g_vertex_buffer_data[], const int g_vertex_buffer_data_size, const GLfloat g_color_buffer_data[], const int g_color_buffer_data_size)
{
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
		{
			clock_t start = clock();
			vector<triangle> triangles(g_vertex_buffer_data_size / 3 / 3);
			vector<triangle> trianglesOnViewport;
			GLint viewportSize[4];
			glGetIntegerv(GL_VIEWPORT, viewportSize);
			vector<triangle> vertexPositions;

			// Get the texture from shader
			GLint texWidth, texHeight, internalFormat;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &internalFormat); // get internal format type of GL texture
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth); // get width of GL texture
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight); // get height of GL texture
			// GL_TEXTURE_COMPONENTS and GL_INTERNAL_FORMAT are the same.
			// just work with RGB8 and RGBA8
			GLint numBytes = 0;
			switch (internalFormat) // determine what type GL texture has...
			{
			case GL_RGB:
				numBytes = texWidth * texHeight * 3;
				break;
			case GL_RGBA:
				numBytes = texWidth * texHeight * 4;
				break;
			default: // unsupported type (or you can put some code to support more formats if you need)
				break;
			}
			unsigned char *pixels = new unsigned char[numBytes]; // allocate image data into RAM
			glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_UNSIGNED_BYTE, pixels);
			//delete [] pixels; // when you don't need 'pixels' anymore clean a memory page to avoid memory leak.

			// Modify the coordinate
			for (int i = 0; i < g_vertex_buffer_data_size / 3; i++)
			{
				triangles[i / 3].vertices[i % 3].position = MVP * glm::vec4(g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2], 1);
				//triangles[i / 3].vertices[i % 3].color = glm::vec3(g_color_buffer_data[i * 3], g_color_buffer_data[i * 3 + 1], g_color_buffer_data[i * 3 + 2]);
				int positionX = g_color_buffer_data[i * 2] * texWidth;
				int positionY = g_color_buffer_data[i * 2 + 1] * texHeight;
				GLfloat R = static_cast<float>(pixels[(positionY * texHeight + positionX) * 3]) / 255;
				GLfloat G = static_cast<float>(pixels[(positionY * texHeight + positionX) * 3 + 1]) / 255;
				GLfloat B = static_cast<float>(pixels[(positionY * texHeight + positionX) * 3 + 2]) / 255;
				//cout << R << " " << G << " " << B << endl;
				//system("pause");
				triangles[i / 3].vertices[i % 3].color = glm::vec3(R, G, B);
				for (int j = 0; j < 4; j++)
				{
					triangles[i / 3].vertices[i % 3].position[j] /= triangles[i / 3].vertices[i % 3].position.w; // normalize the frustrum
					triangles[i / 3].vertices[i % 3].position[j] += 1; // change the range from -1~1 to 0~2
					triangles[i / 3].vertices[i % 3].position[j] /= 2; // compress the range from 0~2 to 0~1
				}
			}

			int width = viewportSize[2] - viewportSize[0], height = viewportSize[3] - viewportSize[1];
			int bytesToUsePerPixel = 3;
			int sizeOfByte = sizeof(unsigned char);
			int theSize = width * height * sizeOfByte * bytesToUsePerPixel;
			unsigned char* imData = new unsigned char[theSize];
			vector< vector<pixel> > viewport(height, vector<pixel>(width));

			/* This is no use now
			for (int i = 0; i < height; i++)
			{
			for (int j = 0; j < width; j++)
			{
			viewport[i][j].position = glm::vec4(1.0 / width * j, 1.0 / height * i, 1.0, 1.0);
			}
			}*/

			// Place the vertices
			for (int i = 0; i < triangles.size(); i++)
			{
				triangle triangleOnViewport;
				int screenX, screenY;
				for (int j = 0; j < 3; j++)
				{
					screenX = triangles[i].vertices[j].position.x * width, screenY = triangles[i].vertices[j].position.y * height;
					triangleOnViewport.vertices[j].position = glm::vec4(screenX, screenY, 0.0, 1.0);
					for (int k = 0; k < 3; k++)
					{
						viewport[screenY][screenX].RGB[k] = static_cast<unsigned char>(255 * triangles[i].vertices[j].color[k]);
					}
				}
				trianglesOnViewport.push_back(triangleOnViewport);
				/* Draw sides. Maybe it is not necessary.
				for (int j = 0; j < 3; j++)
				{
					glm::vec4 vector = triangleOnViewport.vertices[(j + 1) % 3] - triangleOnViewport.vertices[j % 3];
					float len = glm::length(vector);
					glm::vec4 normalizedVec = glm::normalize(vector);
					int scale = 1;
					vector = static_cast<float>(scale)* normalizedVec;
					while (glm::length(vector) < len)
					{
						for (int k = 0; k < 3; k++)
						{
							viewport[triangleOnViewport.vertices[j].y + static_cast<int>(vector.y)][triangleOnViewport.vertices[j].x + static_cast<int>(vector.x)].RGB[k] = 0;
						}
						++scale;
						vector = static_cast<float>(scale)* normalizedVec;
					}
				}*/
			}
			
			// Do rasterization and give the color information from viewport to imData
			vector<glm::vec3> vertices(3);
			glm::vec3 point;
			glm::vec2 v1, v2, uv;
			float area[3];
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					point = glm::vec3(j, i, 0.0);
					for (int k = 0; k < trianglesOnViewport.size(); k++)
					{
						for (int l = 0; l < 3; l++)
						{
							vertices[l] = trianglesOnViewport[k].vertices[l].position.xyz();
						}
						if (glm::dot(glm::cross(vertices[1] - vertices[0], point - vertices[0]), glm::vec3(0.0, 0.0, 1.0)) >= 0.0
							&& glm::dot(glm::cross(vertices[2] - vertices[1], point - vertices[1]), glm::vec3(0.0, 0.0, 1.0)) >= 0.0
							&& glm::dot(glm::cross(vertices[0] - vertices[2], point - vertices[2]), glm::vec3(0.0, 0.0, 1.0)) >= 0.0)
						{
							v1 = (vertices[0] - point).xy();
							v2 = (vertices[1] - point).xy();
							area[0] = (v1.x * v2.y - v2.x * v1.y) / 2;
							v1 = (vertices[1] - point).xy();
							v2 = (vertices[2] - point).xy();
							area[1] = (v1.x * v2.y - v2.x * v1.y) / 2;
							v1 = (vertices[2] - point).xy();
							v2 = (vertices[0] - point).xy();
							area[2] = (v1.x * v2.y - v2.x * v1.y) / 2;
							/*viewport[i][j].RGB[0] = static_cast<unsigned char>(255.0 * (triangles[k].vertices[0].color[0] * area[1] + triangles[k].vertices[1].color[0] * area[2] + triangles[k].vertices[2].color[0] * area[0]) / (area[0] + area[1] + area[2]));
							viewport[i][j].RGB[1] = static_cast<unsigned char>(255.0 * (triangles[k].vertices[0].color[1] * area[1] + triangles[k].vertices[1].color[1] * area[2] + triangles[k].vertices[2].color[1] * area[0]) / (area[0] + area[1] + area[2]));
							viewport[i][j].RGB[2] = static_cast<unsigned char>(255.0 * (triangles[k].vertices[0].color[2] * area[1] + triangles[k].vertices[1].color[2] * area[2] + triangles[k].vertices[2].color[2] * area[0]) / (area[0] + area[1] + area[2]));*/
							uv = glm::vec2((g_color_buffer_data[6 * k] * area[1] + g_color_buffer_data[6 * k + 2] * area[2] + g_color_buffer_data[6 * k + 4] * area[0]) / (area[0] + area[1] + area[2]),
								(g_color_buffer_data[6 * k + 1] * area[1] + g_color_buffer_data[6 * k + 3] * area[2] + g_color_buffer_data[6 * k + 5] * area[0]) / (area[0] + area[1] + area[2]));
							int positionX = uv.x * texWidth;
							int positionY = uv.y * texHeight;
							viewport[i][j].RGB[0] = static_cast<unsigned char>(pixels[(positionY * texHeight + positionX) * 3]);
							viewport[i][j].RGB[1] = static_cast<unsigned char>(pixels[(positionY * texHeight + positionX) * 3 + 1]);
							viewport[i][j].RGB[2] = static_cast<unsigned char>(pixels[(positionY * texHeight + positionX) * 3 + 2]);
						}
					}

					imData[3 * (i * width + j)] = viewport[i][j].RGB[0];
					imData[3 * (i * width + j) + 1] = viewport[i][j].RGB[1];
					imData[3 * (i * width + j) + 2] = viewport[i][j].RGB[2];
				}
			}

			ilInit();
			ILuint imageID = ilGenImage();
			ilBindImage(imageID);
			ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, imData);
			printf("Now width=%d, height=%d, bpp=%d\n", ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP));
			ilSetData(imData);
			ilEnable(IL_FILE_OVERWRITE);
			ilSave(IL_PNG, "output.png");
			cout << "Time used: " << static_cast<double>(clock() - start) / CLK_TCK << " (sec)" << endl;
		}
	}
}