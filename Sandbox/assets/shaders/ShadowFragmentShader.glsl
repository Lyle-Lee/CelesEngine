#version 330 core

layout(location = 0) out vec4 depthColor;

//uniform vec3 uLightPos;

in highp vec3 vNormal;
in highp vec2 vTexCoord;

vec4 pack(float depth)
{
    // Store z value in 4 bytes, precision for one byte is 1/256
    const vec4 bitShift = vec4(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0);
    const vec4 bitMask = vec4(1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0, 0.0);
    // gl_FragCoord: fragment coordinate in WindowCoord, fract(): returns the fractional part of a value
    vec4 rgbaDepth = fract(depth * bitShift);
    rgbaDepth -= rgbaDepth.gbaa * bitMask; // Cut off the value which do not fit in 8 bits
    return rgbaDepth;
}

void main()
{
    //gl_FragColor = vec4( 1.0, 0.0, 0.0, gl_FragCoord.z);
    depthColor = pack(gl_FragCoord.z);
}