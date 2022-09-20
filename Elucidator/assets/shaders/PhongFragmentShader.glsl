#version 330 core

layout(location = 0) out vec4 color;

//uniform sampler2D uTexture;
uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform vec3 uLightIntensity;
uniform vec3 uLightPos;
uniform vec3 uCameraPos;
uniform sampler2D uShadowMap;

in highp vec3 vWorldPos;
in highp vec3 vNormal;
in highp vec2 vTexCoord;
in vec4 vPositionFromLight;

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

float unpack(vec4 rgbaDepth)
{
    const vec4 bitShift = vec4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
    return dot(rgbaDepth, bitShift);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples(const in vec2 randomSeed)
{
    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(randomSeed) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int i = 0; i < NUM_SAMPLES; i++)
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

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        poissonDisk[i] = vec2(radius * cos(angle), radius * sin(angle));

        sampleX = rand_1to1(sampleY);
        sampleY = rand_1to1(sampleX);

        angle = sampleX * PI2;
        radius = sqrt(sampleY);
    }
}

float findBlocker(sampler2D shadowMap, vec2 uv, float zReceiver)
{
    poissonDiskSamples(uv);
    float filterRange = (zReceiver - SM_NEAR) / zReceiver * LIGHT_WIDTH;

    int cnt = 0;
    float sumBlockerDepth = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        vec2 xy = uv + poissonDisk[i] * filterRange;
        float lightDepth = unpack(texture2D(shadowMap, xy));

        if (lightDepth + EPS < zReceiver) {
            ++cnt;
            sumBlockerDepth += lightDepth;
        }
    }

    return cnt > 0 ? sumBlockerDepth / float(cnt) : zReceiver;
}

float PCF(sampler2D shadowMap, vec4 coords)
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
        float lightDepth = unpack(texture2D(shadowMap, uv));

        res += lightDepth + EPS < coords.z ? 0.0 : 1.0;
    }

    return res / float(NUM_SAMPLES);
}

float PCSS(sampler2D shadowMap, vec4 coords)
{
    // STEP 1: avgblocker depth
    float avgBlockerDepth = findBlocker(shadowMap, coords.xy, coords.z);
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
        float lightDepth = unpack(texture2D(shadowMap, uv));

        res += lightDepth + EPS < coords.z ? 0.0 : 1.0;
    }

    return res / float(NUM_SAMPLES);
}

float useShadowMap(sampler2D shadowMap, vec4 shadowCoord)
{
    float lightDepth = unpack(texture2D(shadowMap, shadowCoord.xy));
    return lightDepth + EPS < shadowCoord.z ? 0.0 : 1.0;
}

vec3 BlinnPhong()
{
    //vec3 color = texture2D(uTexture, vTexCoord).rgb;
    //color = pow(color, vec3(2.2));
    vec3 lightDir = normalize(uLightPos - vWorldPos);

    vec3 ambient = 0.05 * uKa;

    float cosTheta = max(dot(lightDir, vNormal), 0.0);
    vec3 light_atten_coff = uLightIntensity / pow(length(uLightPos - vWorldPos), 2.0);
    vec3 diffuse = cosTheta * light_atten_coff * uKd;

    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfDir, vNormal), 0.0), 32.0);
    vec3 specular = uKs * light_atten_coff * spec;

    vec3 radiance = ambient + diffuse + specular;
    return pow(radiance, vec3(1.0 / 2.2));
}

void main()
{
    // transform to NDC ([0, 1])
    vec3 shadowCoord = vPositionFromLight.xyz / vPositionFromLight.w;
    shadowCoord.xyz = (shadowCoord.xyz + 1.0) / 2.0;

    float visibility = 1.0;
    if (shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0)
        //visibility = useShadowMap(uShadowMap, vec4(shadowCoord, 1.0));
        visibility = PCF(uShadowMap, vec4(shadowCoord, 1.0));
        //visibility = PCSS(uShadowMap, vec4(shadowCoord, 1.0));

    vec3 PhongColor = BlinnPhong();

    color = vec4(PhongColor * visibility, 1.0);
    //color = vec4(PhongColor, 1.0);
}