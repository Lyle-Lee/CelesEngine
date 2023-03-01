#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;
uniform mat4 uLightVP;

//out highp vec4 vPosWorld;
out highp vec3 vNormalWorld;
out highp vec2 vTexCoord;
out highp float vDepth;
out vec4 vPosFromLight;

void main()
{
	//vec4 posWorld = uModel * vec4(aPosition, 1.0);
	//vPosWorld = posWorld.xyzw / posWorld.w;
	vec4 normalWorld = uModel * vec4(aNormal, 0.0);
	vNormalWorld = normalize(normalWorld.xyz);
	vTexCoord = aTexCoord;

	gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
	vDepth = gl_Position.w;

	vPosFromLight = uLightVP * uModel * vec4(aPosition, 1.0);
}