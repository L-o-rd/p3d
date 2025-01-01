#include <GL/glew.h>
#include "Skybox.hpp"
#include <iostream>
#include "SOIL.h"

constexpr static const char * Faces[6] = {
	"./Assets/right.png",
	"./Assets/left.png",
	"./Assets/up.png",
	"./Assets/down.png",
	"./Assets/front.png",
	"./Assets/back.png",
};

constexpr static float Vertices[] = {
	-1.f, -1.f,  1.f,
	 1.f, -1.f,  1.f,
	 1.f, -1.f, -1.f,
	-1.f, -1.f, -1.f,

	-1.f,  1.f,  1.f,
	 1.f,  1.f,  1.f,
	 1.f,  1.f, -1.f,
	-1.f,  1.f, -1.f,
};

constexpr static GLuint Indices[] = {
	1, 2, 6,
	6, 5, 1,

	0, 4, 7,
	7, 3, 0,

	4, 5, 6,
	6, 7, 4,

	0, 3, 2,
	2, 1, 0,

	0, 1, 5,
	5, 4, 0,

	3, 7, 6,
	6, 2, 3,
};

auto Skybox::Destroy(void) -> void {
	glDisableVertexAttribArray(0);
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &this->vao);

	std::cout << "Skybox destroyed." << std::endl;
}

auto Skybox::Create(void) -> void {
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ebo);

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &this->tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width, height, channels;
	for (int i = 0; i < 6; ++i) {
		GLubyte* data = SOIL_load_image(Faces[i], &width, &height, &channels, SOIL_LOAD_RGB);
		if (data) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);

			SOIL_free_image_data(data);
		} else {
			std::cerr << "Skybox::Create couldn't load `" << Faces[i] << "`." << std::endl;
		}
	}
}