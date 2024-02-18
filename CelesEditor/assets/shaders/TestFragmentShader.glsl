#version 450

layout(location = 0) out vec4 color;
layout(location = 1) out int color1;

uniform sampler2D uTexture[32];

in vec3 vPosition;
in vec4 vColor;
in vec2 vTexCoord;
in float vTexID;
in float vTilingFactor;
in flat int vEntityID;

void main()
{
	//color = vec4(vPosition * 0.5 + 0.5, 1.0);
	//color = vColor;
	color = texture(uTexture[int(vTexID)], vTexCoord * vTilingFactor) * vColor;
	color1 = vEntityID;
}
