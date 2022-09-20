#pragma once

#include "glm.hpp"

class Object
{
public:
	Object()
		: translation(1.0f), rotation(1.0f), scale(1.0f)
	{}
	virtual ~Object() {}

	void setTranslation(const glm::mat4&);
	void setRotation(const glm::mat4&);
	void setScale(const glm::mat4&);

	glm::mat4 translation, rotation, scale;
};

class Cube : public Object
{
public:
	Cube() {}
	~Cube() {}

	float positions[24] = {
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f
	};
	unsigned int indices[36] = {
		4, 0, 5, 0, 1, 5, // front
		6, 2, 7, 2, 3, 7, // back
		7, 4, 6, 4, 5, 6, // top
		3, 0, 2, 0, 1, 2, // bottom
		5, 1, 6, 1, 2, 6, // right
		7, 3, 4, 3, 0, 4  // left
	};
};

class Quad : public Object
{
public:
	Quad() {}
	~Quad() {}

	float vertices[24] = {
		-1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // position, normal
		 1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};
	unsigned int indices[6] = {
		3, 0, 2, 0, 1, 2
	};
};