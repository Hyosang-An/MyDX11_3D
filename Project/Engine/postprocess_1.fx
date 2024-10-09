#ifndef GAUSSIAN_BLUR
#define GAUSSIAN_BLUR

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vWorldPos : POSITION;
};


// 가우시안 블러
#define KERNEL_SIZE 5
static const float weights[KERNEL_SIZE][KERNEL_SIZE] =
{
    { 0.0030, 0.0133, 0.0219, 0.0133, 0.0030 },
    { 0.0133, 0.0596, 0.0983, 0.0596, 0.0133 },
    { 0.0219, 0.0983, 0.1621, 0.0983, 0.0219 },
    { 0.0133, 0.0596, 0.0983, 0.0596, 0.0133 },
    { 0.0030, 0.0133, 0.0219, 0.0133, 0.0030 }
};
        
static const float offsets[KERNEL_SIZE] =
{
    -2.0, -1.0, 0.0, 1.0, 2.0
};

float4 PS_GaussianBlur(VS_OUT _in) : SV_Target
{
    
    float2 texelSize = float2(1 / g_Resolution.x, 1 / g_Resolution.y);
    
    float4 blurColor = float4(0.f, 0.f, 0.f, 1.f);
        
    // blur
    for (int i = 0; i < KERNEL_SIZE; i++)
        for (int j = 0; j < KERNEL_SIZE; j++)
        {
            float2 offset = float2(offsets[i] * texelSize.x, offsets[j] * texelSize.y);
            float2 samplingUV = clamp(_in.vUV + offset, 0.0, 1.0);
            blurColor += g_tex_1.Sample(g_sam_0, samplingUV) * weights[i][j];
        }

    return blurColor;
}

float4 PS_Bloom(VS_OUT _in) : SV_Target
{
    const float bloomIntensity = 1;
    float2 texelSize = float2(1 / g_Resolution.x, 1 / g_Resolution.y);
    
    float4 blurColor = float4(0.f, 0.f, 0.f, 1.f);
        
    // blur
    for (int i = 0; i < KERNEL_SIZE; i++)
        for (int j = 0; j < KERNEL_SIZE; j++)
        {
            float2 offset = float2(offsets[i] * texelSize.x, offsets[j] * texelSize.y);
            float2 samplingUV = clamp(_in.vUV + offset, 0.0, 1.0);
            blurColor += g_tex_1.Sample(g_sam_0, samplingUV) * weights[i][j];
        }

    return blurColor + g_tex_0.Sample(g_sam_0, _in.vUV) * bloomIntensity;
}
        
#endif