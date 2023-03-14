#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexID;
layout(location = 4) in float aTilingFactor;

uniform mat4 uViewProj;
//uniform mat4 uModel;

out vec3 vPosition;
out vec4 vColor;
out vec2 vTexCoord;
out float vTexID;
out float vTilingFactor;

void main()
{
	vPosition = aPosition;
	vColor = aColor;
	vTexCoord = aTexCoord;
	vTexID = aTexID;
	vTilingFactor = aTilingFactor;
	//gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
	gl_Position = uViewProj * vec4(aPosition, 1.0);
}
