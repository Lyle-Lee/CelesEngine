#version 330 core

layout(location = 0) out vec4 color;

uniform vec3 uColor;
uniform sampler2D uTexture;

in vec3 vPosition;
in vec2 vTexCoord;

void main()
{
	//color = vec4(vPosition * 0.5 + 0.5, 1.0);
	//color = vec4(uColor, 1.0);
	color = texture(uTexture, vTexCoord);
}
