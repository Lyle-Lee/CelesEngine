#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 uColor;

in vec3 vPosition;

void main()
{
	//color = vec4(vPosition * 0.5 + 0.5, 1.0);
	color = uColor;
}
