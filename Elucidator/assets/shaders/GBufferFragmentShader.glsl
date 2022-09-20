#version 330 core

layout(location = 0) out vec4 gKd;
layout(location = 1) out vec4 gDepth;
layout(location = 2) out vec4 gNormal;
layout(location = 3) out vec4 gVisibility;
//layout(location = 4) out vec4 gPosWorld;

uniform sampler2D uKd;
uniform sampler2D uNt;
uniform sampler2D uShadowMap;
uniform vec3 uKd_vec;
uniform int materialID;

//in highp vec4 vPosWorld;
in highp vec3 vNormalWorld;
in highp vec2 vTexCoord;
in highp float vDepth;
in vec4 vPosFromLight;

// Shadow map related variables
#define NUM_SAMPLES 60
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define LIGHT_WIDTH 0.6
#define SM_NEAR 1e-2

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

float unpack(vec4 rgbaDepth)
{
    const vec4 bitShift = vec4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
    return dot(rgbaDepth, bitShift);
}

float useShadowMap(vec4 shadowCoord)
{
    float lightDepth = unpack(texture2D(uShadowMap, shadowCoord.xy));
    return lightDepth + EPS < shadowCoord.z ? 0.0 : 1.0;
}

highp float rand_1to1(highp float x)
{
    // -1 ~ 1
    return fract(sin(x) * 10000.0);
}

highp float rand_2to1(vec2 uv)
{
    // 0 ~ 1
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples(const in vec2 randomSeed)
{
    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(randomSeed) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

void uniformDiskSamples(const in vec2 randomSeed)
{
    float randNum = rand_2to1(randomSeed);
    float sampleX = rand_1to1(randNum);
    float sampleY = rand_1to1(sampleX);

    float angle = sampleX * PI2;
    float radius = sqrt(sampleY);

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        poissonDisk[i] = vec2(radius * cos(angle), radius * sin(angle));

        sampleX = rand_1to1(sampleY);
        sampleY = rand_1to1(sampleX);

        angle = sampleX * PI2;
        radius = sqrt(sampleY);
    }
}

float findBlocker(vec2 uv, float zReceiver)
{
    poissonDiskSamples(uv);
    float filterRange = (zReceiver - SM_NEAR) / zReceiver * LIGHT_WIDTH;

    int cnt = 0;
    float sumBlockerDepth = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        vec2 xy = uv + poissonDisk[i] * filterRange;
        float lightDepth = texture2D(uShadowMap, xy).x;

        if (lightDepth + 1e-2 < zReceiver)
        {
            ++cnt;
            sumBlockerDepth += lightDepth;
        }
    }

    return cnt > 0 ? sumBlockerDepth / float(cnt) : zReceiver;
}

float PCF(vec4 coords)
{
    // use coords.xy for seed
    // uniformDiskSamples(coords.xy);
    poissonDiskSamples(coords.xy);

    float textureSize = 2048.0;
    float filterStride = 4.0; // kernel size
    float filterRange = filterStride / textureSize;

    float res = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        vec2 uv = coords.xy + poissonDisk[i] * filterRange;
        float lightDepth = unpack(texture2D(uShadowMap, uv));

        res += lightDepth + EPS < coords.z ? 0.0 : 1.0;
    }

    return res / float(NUM_SAMPLES);
}

float PCSS(vec4 coords)
{
    // STEP 1: avgblocker depth
    float avgBlockerDepth = findBlocker(coords.xy, coords.z);
    if (avgBlockerDepth == coords.z) return 1.0;

    // STEP 2: penumbra size
    float penumbraWidth = (coords.z - avgBlockerDepth) / avgBlockerDepth * LIGHT_WIDTH;
    float filterRange = penumbraWidth * SM_NEAR / coords.z;

    // STEP 3: filtering
    poissonDiskSamples(coords.xy);
    float res = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        vec2 uv = coords.xy + poissonDisk[i] * filterRange;
        float lightDepth = unpack(texture2D(uShadowMap, uv));

        res += lightDepth + EPS < coords.z ? 0.0 : 1.0;
    }

    return res / float(NUM_SAMPLES);
}

void LocalBasis(vec3 n, out vec3 t, out vec3 b)
{
    float sign_ = sign(n.z);
    if (n.z == 0.0)
        sign_ = 1.0;
    float u = -1.0 / (sign_ + n.z);
    float v = n.x * n.y * u;
    t = vec3(1.0 + sign_ * n.x * n.x * u, sign_ * v, -sign_ * n.x);
    b = vec3(v, sign_ + n.y * n.y * u, -n.y);
}

vec3 ApplyTangentNormalMap()
{
    vec3 t, b;
    LocalBasis(vNormalWorld, t, b);
    vec3 nt = texture2D(uNt, vTexCoord).xyz * 2.0 - 1.0;
    nt = normalize(nt.x * t + nt.y * b + nt.z * vNormalWorld);
    return nt;
}

void main()
{
    vec3 kd = materialID == 0 ? uKd_vec : texture2D(uKd, vTexCoord).rgb;
    vec4 shadowCoord = vPosFromLight / vPosFromLight.w * 0.5 + 0.5;
    float visibility = 1.0;
    if (shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0)
        //visibility = useShadowMap(shadowCoord);
        visibility = PCSS(shadowCoord);

    gKd = vec4(kd, 1.0);
    gDepth = vec4(vec3(vDepth), 1.0);
    gNormal = vec4(materialID == 0 ? vNormalWorld : ApplyTangentNormalMap(), 1.0);
    gVisibility = vec4(vec3(visibility), 1.0);
    //gPosWorld = vec4(vec3(vPosWorld.xyz), 1.0);
}