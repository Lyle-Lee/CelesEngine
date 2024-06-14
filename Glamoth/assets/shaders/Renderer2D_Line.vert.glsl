#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProj;
};
//uniform mat4 uModel;

struct VertexOutput
{
	vec4 color;
};

layout(location = 0) out VertexOutput vOutput;
layout(location = 1) out flat int vEntityID;

void main()
{
	vOutput.color = aColor;
	vEntityID = aEntityID;
	//gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
	gl_Position = uViewProj * vec4(aPosition, 1.0);
}
