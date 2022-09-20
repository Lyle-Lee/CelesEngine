#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uViewProj;
//uniform mat4 uLightVP;

out mat4 vWorldToScreen;
out highp vec4 vPosWorld;

void main()
{
	vec4 posWorld = uModel * vec4(aPosition, 1.0);
	vPosWorld = posWorld / posWorld.w;
	vWorldToScreen = uViewProj;

	gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
}