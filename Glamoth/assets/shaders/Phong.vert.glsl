#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(std140, binding = 0) uniform GeometryData
{
    mat4 uModelToWorld;
};
layout(std140, binding = 1) uniform CameraData
{
    mat4 uViewProj;
    vec3 uCameraW;
};
layout(std140, binding = 2) uniform LightData
{
    mat4 uLightVP;
    vec3 uLightPos;
    vec3 uLightIntensity;
};

layout(location = 0) out highp vec3 vWorldPos;
layout(location = 1) out highp vec3 vNormal;
layout(location = 2) out highp vec2 vTexCoord;
layout(location = 3) out vec4 vPositionFromLight;

void main()
{
    vWorldPos = (uModelToWorld * vec4(aPosition, 1.0)).xyz;
    vNormal = (uModelToWorld * vec4(normalize(aNormal), 0.0)).xyz;

    gl_Position = uViewProj * uModelToWorld * vec4(aPosition, 1.0);

    vTexCoord = aTexCoord;
    vPositionFromLight = uLightVP * uModelToWorld * vec4(aPosition, 1.0);
}
