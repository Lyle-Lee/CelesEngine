#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uViewProj;
uniform mat4 uModel;

out vec3 vPosition;

void main()
{
	vPosition = aPosition;
	gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
}
