#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D uTexture;

in vec4 vColor;
in vec2 vTexCoord;
in float vMaterialID;

void main()
{
    if (int(vMaterialID) == 0)
        color = vColor;
    else
    {
        vec4 texColor = texture(uTexture, vTexCoord);
        color = texColor;
    }
}