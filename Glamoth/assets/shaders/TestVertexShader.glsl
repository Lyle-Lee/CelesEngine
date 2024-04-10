#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in int aTexID;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProj;
};
//uniform mat4 uModel;

struct VertexOutput
{
	//vec3 position;
	vec4 color;
	vec2 texCoord;
	float tilingFactor;
};

layout(location = 0) out VertexOutput vOutput;
layout(location = 3) out flat int vTexID;
layout(location = 4) out flat int vEntityID;

void main()
{
	//vOutput.position = aPosition;
	vOutput.color = aColor;
	vOutput.texCoord = aTexCoord;
	vOutput.tilingFactor = aTilingFactor;
	vTexID = aTexID;
	vEntityID = aEntityID;
	//gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
	gl_Position = uViewProj * vec4(aPosition, 1.0);
}
