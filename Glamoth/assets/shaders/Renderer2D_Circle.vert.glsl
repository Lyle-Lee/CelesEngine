#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in float aThickness;
layout(location = 2) in vec3 aLocalPosition;
layout(location = 3) in float aFade;
layout(location = 4) in vec4 aColor;
layout(location = 5) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProj;
};
//uniform mat4 uModel;

struct VertexOutput
{
	vec3 localPos;
	float thickness;
	vec4 color;
	float fade;
};

layout(location = 0) out VertexOutput vOutput;
layout(location = 4) out flat int vEntityID;

void main()
{
	vOutput.localPos = aLocalPosition;
	vOutput.thickness = aThickness;
	vOutput.color = aColor;
	vOutput.fade = aFade;

	vEntityID = aEntityID;

	//gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
	gl_Position = uViewProj * vec4(aPosition, 1.0);
}
