#include "Object.h"

void Object::setTranslation(const glm::mat4& matrix)
{
	translation = matrix;
}

void Object::setRotation(const glm::mat4& matrix)
{
	rotation = matrix;
}

void Object::setScale(const glm::mat4& matrix)
{
	scale = matrix;
}


