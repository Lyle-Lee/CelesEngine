#version 450 core

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	vec3 localPos;
	float thickness;
	vec4 color;
	float fade;
};

layout(location = 0) in VertexOutput vInput;
layout(location = 4) in flat int vEntityID;

void main()
{
	// -1 -> 1 local space
	vec2 uv = vInput.localPos.xy;

	// Calculate distance and fill circle with color
	float distance = 1.0 - length(uv);
	float alpha = smoothstep(0.0, vInput.fade, distance);
	alpha *= smoothstep(vInput.thickness + vInput.fade, vInput.thickness, distance);
	if (alpha == 0.0)
		discard;

	// Set outputs
	oColor = vec4(vInput.color.rgb, vInput.color.a * alpha);
	oEntityID = vEntityID;
}
