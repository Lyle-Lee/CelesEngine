#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
//uniform mat4 uViewProj;
uniform mat4 uLightVP;

out highp vec3 vNormal;
out highp vec2 vTexCoord;

void main()
{
	vNormal = (uModel * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;

	gl_Position = uLightVP * uModel * vec4(aPosition, 1.0);
}