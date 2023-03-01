#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uViewProj;
uniform mat4 uModel;

out vec3 vPosition;
out vec2 vTexCoord;

void main()
{
	vPosition = aPosition;
	vTexCoord = aTexCoord;
	gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
}
