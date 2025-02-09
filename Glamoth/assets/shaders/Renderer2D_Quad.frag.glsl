#version 450 core

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	//vec3 position;
	vec4 color;
	vec2 texCoord;
	float tilingFactor;
};

layout(location = 0) in VertexOutput vInput;
layout(location = 3) in flat int vTexID;
layout(location = 4) in flat int vEntityID;

layout(binding = 0) uniform sampler2D uTextures[32];

void main()
{
	//oColor = vec4(vInput.position * 0.5 + 0.5, 1.0);
	vec4 texColor = texture(uTextures[vTexID], vInput.texCoord * vInput.tilingFactor) * vInput.color;
	if (texColor.a == 0.0)
		discard;

	oColor = texColor;
	oEntityID = vEntityID;
}
