#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aMaterialID;

uniform mat4 uMVP;

out vec4 vColor;
out vec2 vTexCoord;
out float vMaterialID;

void main()
{
    gl_Position = uMVP * vec4(aPosition, 1.0);
    vColor = aColor;
    vTexCoord = aTexCoord;
    vMaterialID = aMaterialID;
}