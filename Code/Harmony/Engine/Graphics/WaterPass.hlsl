static const uint NUM_WAVES = 3;
static const float PI = 3.14159265358979323826264;
static const float G = 9.81f;

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

struct WaterParams
{
    float amplitude;
    float2 windDirection;
    float steepness;
    float wavelength;
    float _waterParamsPadding0;
    float _waterParamsPadding1;
    float _waterParamsPadding2;
};

cbuffer cbWaterParams : register(b1)
{
    WaterParams gWaterParams[NUM_WAVES];
};

cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;

    float3 gEyePosW;
    uint cbPassPadding0;

    float4x4 gProj;
    float4x4 gInvProj;

    float gNearZ;
    float gFarZ;
    uint cbPassPadding1;
    uint cbPassPadding2;

    float4x4 gViewProj;
    float4x4 gInvViewProj;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    uint _cbPassPaddingFinal[20];
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangentU : TANGENT;
    float2 textureCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;

    float normalizedHeight : TEXCOORD0;
};

float calculatePhase(float3 vertex, float waveLength, float2 direction, float t)
{
    float k = (2.0f * PI) / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * (direction.x * vertex.x + direction.y * vertex.z)) - (angularFreq * t);

    return phase;
}

float3 calculateOffsets(float3 vertex, float amplitude, float2 direction, float steepness, float waveLength, float t)
{
    float phase = calculatePhase(vertex, waveLength, direction, t);
    float dx = (steepness * amplitude * direction.x * cos(phase));
    float dy = amplitude * sin(phase);
    float dz = (steepness * amplitude * direction.y * cos(phase));

    return float3(dx, dy, dz);
}

float3 calculateOffsetsRadial(float2 center, float3 vertex, float amplitude, float2 direction, float steepness, float waveLength, float t)
{
    // Compute the radial direction for the vertex
    float2 radial = vertex.xz - center;
    float distance = length(radial);
    if (distance > 0.0001f)
    {
        radial /= distance; // Normalize
    }
    else
    {
        radial = radial/ max(distance, 0.0001f); // float2(1, 0); // Fallback if exactly at center
    }

    // newPhase = k * radius - angularFreq * t
    float k = (2.0f * PI) / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * distance) - (angularFreq * t);

    // the offsets move in radial direction instead
    float dx = steepness * radial.x * cos(phase);
    float dy = amplitude * sin(phase);
    float dz = (steepness * radial.y * cos(phase));

    return float3(dx, dy, dz);
}

VSOutput vsMain(VSInput input)
{
    VSOutput result;
    float4x4 mvp = mul(gView, gProj);

    const float2 center = float2(0.0f, 0.0f);
    const float swirlSpeed = 3.0f;
    const float swirlFallOff = 10.0f;

    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    [unroll]
    for(uint i = 0; i < NUM_WAVES; ++i)
    {
        waveOffset += calculateOffsets(
                        input.position,
                        gWaterParams[i].amplitude,
                        gWaterParams[i].windDirection,
                        gWaterParams[i].steepness,
                        gWaterParams[i].wavelength, gTotalTime);
    }

    float3 displaced = input.position;
    displaced.x += waveOffset.x;
    displaced.y = waveOffset.y;
    displaced.z += waveOffset.z;

    float2 centeredXZ = displaced.xz - center;
    float r = length(centeredXZ);

    float factor = saturate(1.0f - (r / swirlFallOff));
    float rawAngle = swirlSpeed * gTotalTime;
    float wrapped = fmod(rawAngle, 4.0f * PI);
    float swirlAngle = wrapped * factor;

    float s = sin(swirlAngle);
    float c = cos(swirlAngle);
    float2 v = centeredXZ;
    float2 rotatedXZ = float2(
        v.x * c - v.y * s,
        v.x * s + v.y * c
    );

    float3 finalPosition = float3(
        center.x + rotatedXZ.x,
        displaced.y - (swirlSpeed * factor),
        center.y + rotatedXZ.y
    );
    result.position = mul(float4(finalPosition, 1.0f), mvp);

    // compute wave min max
    float maxAmplitude = 0.0f;
    for (int i = 0; i < NUM_WAVES; ++i)
    {
        maxAmplitude += gWaterParams[i].amplitude;
    }
    result.normalizedHeight = saturate(displaced.y / maxAmplitude);

    result.normal = input.normal;
    return result;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    float3 deepColor = float3(0.0f, 0.3f, 0.5f);
    float3 peakColor = float3(1.0f, 1.0f, 1.0f);

    float3 finalColor = lerp(deepColor, peakColor, input.normalizedHeight);

    return float4(finalColor, 1.0f);
}


----


static const uint NUM_WAVES = 3;
static const float PI = 3.14159265358979323826264;
static const float G = 9.81f;

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

struct WaterParams
{
    float amplitude;
    float2 windDirection;
    float steepness;
    float wavelength;
    float _waterParamsPadding0;
    float _waterParamsPadding1;
    float _waterParamsPadding2;
};

cbuffer cbWaterParams : register(b1)
{
    WaterParams gWaterParams[NUM_WAVES];
};

cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;

    float3 gEyePosW;
    uint cbPassPadding0;

    float4x4 gProj;
    float4x4 gInvProj;

    float gNearZ;
    float gFarZ;
    uint cbPassPadding1;
    uint cbPassPadding2;

    float4x4 gViewProj;
    float4x4 gInvViewProj;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    uint _cbPassPaddingFinal[20];
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangentU : TANGENT;
    float2 textureCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;

    float normalizedHeight : TEXCOORD0;
};

float calculatePhase(float3 vertex, float waveLength, float2 direction, float t)
{
    float k = (2 * PI) / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * (direction.x * vertex.x + direction.y * vertex.z)) - (angularFreq * t);

    return phase;
}

float3 calculateOffsets(float3 vertex, float amplitude, float2 direction, float steepness, float waveLength, float t)
{
    float phase = calculatePhase(vertex, waveLength, direction, t);
    float dx = (steepness * amplitude * direction.x * cos(phase));
    float dy = amplitude * sin(phase);
    float dz = (steepness * amplitude * direction.y * cos(phase));

    return float3(dx, dy, dz);
}

VSOutput vsMain(VSInput input)
{
    VSOutput result;
    float4x4 mvp = mul(gView, gProj);

    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    [unroll]
    for(uint i = 0; i < NUM_WAVES; ++i)
    {
        waveOffset += calculateOffsets(
                        input.position,
                        gWaterParams[i].amplitude,
                        gWaterParams[i].windDirection,
                        gWaterParams[i].steepness,
                        gWaterParams[i].wavelength, gTotalTime);
    }

    float3 wave = input.position;
    wave.x += waveOffset.x;
    wave.y = waveOffset.y;
    wave.z += waveOffset.z;
    result.position = mul(float4(wave, 1.0f), mvp);

    // compute wave min max
    float maxAmplitude = 0.0f;
    for (int i = 0; i < NUM_WAVES; ++i)
    {
        maxAmplitude += gWaterParams[i].amplitude;
    }
    result.normalizedHeight = saturate(wave.y / maxAmplitude);

    result.normal = input.normal;
    return result;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    float3 deepColor = float3(0.0f, 0.3f, 0.5f);
    float3 peakColor = float3(1.0f, 1.0f, 1.0f);

    float3 finalColor = lerp(deepColor, peakColor, input.normalizedHeight);

    return float4(finalColor, 1.0f);
}


---------


static const uint NUM_WAVES = 3;
static const float PI = 3.14159265358979323826264;
static const float G = 9.81f;

cbuffer cbFrame : register(b0)
{
    float gTotalTime;
    float gDeltaTime;
    uint _framePadding0;
    uint _framePadding1;
};

struct WaterParams
{
    float amplitude;
    float2 windDirection;
    float steepness;
    float wavelength;
    float _waterParamsPadding0;
    float _waterParamsPadding1;
    float _waterParamsPadding2;
};

cbuffer cbWaterParams : register(b1)
{
    WaterParams gWaterParams[NUM_WAVES];
};

cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;

    float3 gEyePosW;
    uint cbPassPadding0;

    float4x4 gProj;
    float4x4 gInvProj;

    float gNearZ;
    float gFarZ;
    uint cbPassPadding1;
    uint cbPassPadding2;

    float4x4 gViewProj;
    float4x4 gInvViewProj;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    uint _cbPassPaddingFinal[20];
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangentU : TANGENT;
    float2 textureCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;

    float normalizedHeight : TEXCOORD0;
};

float calculatePhase(float3 vertex, float waveLength, float2 direction, float t)
{
    float k = (2.0f * PI) / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * (direction.x * vertex.x + direction.y * vertex.z)) - (angularFreq * t);

    return phase;
}

float3 calculateOffsets(float3 vertex, float amplitude, float2 direction, float steepness, float waveLength, float t)
{
    float phase = calculatePhase(vertex, waveLength, direction, t);
    float dx = (steepness * amplitude * direction.x * cos(phase));
    float dy = amplitude * sin(phase);
    float dz = (steepness * amplitude * direction.y * cos(phase));

    return float3(dx, dy, dz);
}

float3 calculateOffsetsRadial(float2 center, float3 vertex, float amplitude, float2 direction, float steepness, float waveLength, float t)
{
    // Compute the radial direction for the vertex
    float2 radial = vertex.xz - center;
    float distance = length(radial);
    if (distance > 0.0001f)
    {
        radial /= distance; // Normalize
    }
    else
    {
        radial = radial/ max(distance, 0.0001f); // float2(1, 0); // Fallback if exactly at center
    }

    // newPhase = k * radius - angularFreq * t
    float k = (2.0f * PI) / waveLength;
    float angularFreq = sqrt(G * k);
    float phase = (k * distance) - (angularFreq * t);

    // the offsets move in radial direction instead
    float dx = steepness * radial.x * cos(phase);
    float dy = amplitude * sin(phase);
    float dz = (steepness * radial.y * cos(phase));

    return float3(dx, dy, dz);
}

VSOutput vsMain(VSInput input)
{
    VSOutput result;
    float4x4 mvp = mul(gView, gProj);

    const float2 center = float2(0.0f, 0.0f);
    const float swirlSpeed = 3.0f;
    const float swirlFallOff = 15.0f;

    float3 waveOffset = float3(0.0f, 0.0f, 0.0f);
    [unroll]
    for(uint i = 0; i < NUM_WAVES; ++i)
    {
        waveOffset += calculateOffsets(
                        input.position,
                        gWaterParams[i].amplitude,
                        gWaterParams[i].windDirection,
                        gWaterParams[i].steepness,
                        gWaterParams[i].wavelength, gTotalTime);
    }

    float3 displaced = input.position;
    displaced.x += waveOffset.x;
    displaced.y = waveOffset.y;
    displaced.z += waveOffset.z;

    float2 centeredXZ = displaced.xz - center;
    float r = length(centeredXZ);

    float factor = saturate(1.0f - (r / swirlFallOff));
    float swirlAngle = swirlSpeed * gTotalTime * factor;

    float s = sin(swirlAngle);
    float c = cos(swirlAngle);
    float2 v = centeredXZ;
    float2 rotatedXZ = float2(
        v.x * c - v.y * s,
        v.x * s + v.y * c
    );

    float3 finalPosition = float3(
        center.x + rotatedXZ.x,
        displaced.y,
        center.y + rotatedXZ.y
    );
    result.position = mul(float4(finalPosition, 1.0f), mvp);

    // compute wave min max
    float maxAmplitude = 0.0f;
    for (int i = 0; i < NUM_WAVES; ++i)
    {
        maxAmplitude += gWaterParams[i].amplitude;
    }
    result.normalizedHeight = saturate(displaced.y / maxAmplitude);

    result.normal = input.normal;
    return result;
}

float4 psMain(VSOutput input) : SV_TARGET
{
    float3 deepColor = float3(0.0f, 0.3f, 0.5f);
    float3 peakColor = float3(1.0f, 1.0f, 1.0f);

    float3 finalColor = lerp(deepColor, peakColor, input.normalizedHeight);

    return float4(finalColor, 1.0f);
}
