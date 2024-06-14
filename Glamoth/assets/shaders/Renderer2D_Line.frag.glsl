#version 450 core

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	vec4 color;
};

layout(location = 0) in VertexOutput vInput;
layout(location = 1) in flat int vEntityID;

void main()
{
	oColor = vInput.color;
	oEntityID = vEntityID;
}
