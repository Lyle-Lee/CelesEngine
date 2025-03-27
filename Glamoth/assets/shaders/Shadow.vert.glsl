#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(std140, binding = 0) uniform GeometryData
{
    mat4 uModelToWorld;
};
layout(std140, binding = 2) uniform LightData
{
    mat4 uLightVP;
    vec3 uLightPos;
    vec3 uLightIntensity;
};

layout(location = 0) out highp vec3 vNormal;
layout(location = 1) out highp vec2 vTexCoord;

void main()
{
	vNormal = (uModelToWorld * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;

	gl_Position = uLightVP * uModelToWorld * vec4(aPosition, 1.0);
}
