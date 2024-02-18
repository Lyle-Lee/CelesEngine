#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexID;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

uniform mat4 uViewProj;
//uniform mat4 uModel;

out vec3 vPosition;
out vec4 vColor;
out vec2 vTexCoord;
out float vTexID;
out float vTilingFactor;
out flat int vEntityID;

void main()
{
	vPosition = aPosition;
	vColor = aColor;
	vTexCoord = aTexCoord;
	vTexID = aTexID;
	vTilingFactor = aTilingFactor;
	vEntityID = aEntityID;
	//gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
	gl_Position = uViewProj * vec4(aPosition, 1.0);
}
