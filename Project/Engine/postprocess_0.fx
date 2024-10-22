#ifndef _POSTPROCESS
#define POSTPROCESS

#include "value.fx"

// ===========================
// GrayFilterShader
// Mesh     : RectMesh
// DSTYPE   : NO_TEST_NO_WRITE
// g_tex_0  : TargetCopyTex
// g_tex_1  : NoiseTexture 1
// g_tex_2  : NoiseTexture 2
// g_tex_3  : NoiseTexture 3
// ===========================

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

VS_OUT VS_Screen(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // Proj 행렬을 곱한 결과는 각 xyz 에 자신의 ViewZ 가 곱혀져있는 형태이다.
    // W 자리에 자신의 ViewZ 가 출력되기 때문에 이것으로 각 xyz 를 나누어야 실제 Proj 좌표가 나온다.
    // 따라서 Rasterizer State 에 투영행렬을 곱한 결과를 전달하면 이후 렌더링 파이프라인에서 각 xyz 를 w 로 나누어서 NDC좌표로 만들어 사용한다.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_GrayFilter(VS_OUT _in) : SV_Target
{
    // GrayFilter
    //float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);    
    //float Average = (vColor.x + vColor.y + vColor.z) / 3.f;
    //vColor = float4(Average, Average, Average, 1.f); 
    
    //===========================================================================
    // Cos Distortion
    //float2 vUV = _in.vUV;    
    //vUV.y += cos( (vUV.x + g_EngineTime * 0.1f) * PI * 12.f) * 0.01;    
    //float4 vColor = g_tex_0.Sample(g_sam_0, vUV);
    
    //===========================================================================
    //Noise

    float2 vUV = _in.vUV;
    vUV.x += g_EngineTime * 0.1;
    float4 vNoise = g_tex_1.Sample(g_sam_0, vUV);
    vNoise = (vNoise * 2.f - 1.f) * 0.01f;
    vUV = _in.vUV + vNoise.xy;
    float4 vColor = g_tex_0.Sample(g_sam_0, vUV);
    vColor.b *= 1.5f;
    
    return vColor;
    
    //===========================================================================
  
}


// ==============================================================================
// Convex Shader
// Mesh     : RectMesh
// DSTYPE   : NO_TEST_NO_WRITE
// g_tex_0  : PostProcessRTTex_0
// g_tex_1  : PostProcessDSTex
// ===============================================================================

VS_OUT VS_ScreenZoom(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // Proj 행렬을 곱한 결과는 각 xyz 에 자신의 ViewZ 가 곱혀져있는 형태이다.
    // W 자리에 자신의 ViewZ 가 출력되기 때문에 이것으로 각 xyz 를 나누어야 실제 Proj 좌표가 나온다.
    // 따라서 Rasterizer State 에 투영행렬을 곱한 결과를 전달하면 이후 렌더링 파이프라인에서 각 xyz 를 w 로 나누어서 NDC좌표로 만들어 사용한다.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);

    // Normalized UV with zoom
    float2 nUV = _in.vUV - 0.5;
    nUV *= 0.9;
    nUV += 0.5;
    
    output.vUV = nUV;
    
    return output;
}

float4 PS_RippleEffect(VS_OUT _in) : SV_Target
{
    // Normalize coordinates to screen height
    float2 cPos = _in.vPosition.xy / g_Resolution.y;
    cPos = cPos * 2.0 - float2(g_Resolution.x / g_Resolution.y, 1);
    
    // Calculate distance from center
    float cLength = length(cPos);
    
    if (cLength < 0.001)
        return g_tex_0.Sample(g_sam_0, _in.vUV);
    
    // Ripple effect
    float2 ripple = (cPos / cLength) * cos(cLength * 12.0 - g_EngineTime * 4.0) * 0.012;

    // Adjust UV coordinates and clamp
    float2 uv = clamp(_in.vUV + ripple, 0.0, 1.0);

    // Check for edge cases
    if (uv.x == 0.0 || uv.y == 0.0 || uv.x == 1.0 || uv.y == 1.0)
        return float4(0, 0, 0, 1);
    
    // Sample texture
    float4 col = g_tex_0.Sample(g_sam_0, uv);

    return col;
}


// ==============================================================================
// Distortion Shader
// Mesh     : RectMesh
// DSTYPE   : NO_TEST_NO_WRITE
// g_tex_0  : TargetCopyTex
// g_tex_1  : NoiseTexture 1
// g_tex_2  : NoiseTexture 2
// g_tex_3  : NoiseTexture 3
// ===============================================================================
VS_OUT VS_Distortion(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Distortion(VS_OUT _in) : SV_Target
{
    // 1. 렌더타겟 해상도 정보 (전역 상수버퍼)    
    // 2. 픽셀쉐이더의 픽셀 좌표
    
    //버텍스 쉐이더에서 출력된 클립 좌표는 렌더링 파이프라인을 통해 다음 단계인 뷰포트 변환으로 전달됩니다. 
    //이 과정에서 클립 좌표는 정규 장치 좌표(NDC)로 변환되고, 다시 뷰포트 좌표로 변환됩니다. 
    //뷰포트 좌표는 렌더 타겟의 해상도에 기반한 좌표입니다.

    float2 vScreenUV = _in.vPosition.xy / g_Resolution;
        
    float2 vNoiseUV = vScreenUV;
    vNoiseUV.x += g_EngineTime * 0.1;
    float4 vNoise = g_tex_3.Sample(g_sam_0, vNoiseUV);
    
    vNoise = (vNoise * 2.f - 1.f) * 0.01f;
    vScreenUV = vScreenUV + vNoise.xy;
    float4 vColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    
    vColor.b *= 2;
    
    return vColor;
}


// ==============================================================================
// Convex Shader
// Mesh     : RectMesh
// DSTYPE   : NO_TEST_NO_WRITE
// g_tex_0  : PostProcessRTTex_0
// g_tex_1  : PostProcessDSTex
// ===============================================================================
VS_OUT VS_ConvexLens(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    return output;
}

float4 PS_ConvexLens(VS_OUT _in) : SV_Target
{
    // 1. 렌더타겟 해상도 정보 (전역 상수버퍼)    
    // 2. 픽셀쉐이더의 픽셀 좌표

    // 초점거리
    float FOCAL_LENGTH = 100;
    
    // 카메라의 depth 범위
    float near = 1.f;
    float far = 100000.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution;
        
    // 렌즈의 world위치 및 방향
    float3 lensWorldPos = float3(matWorld[3][0], matWorld[3][1], matWorld[3][2]);
    float3 lensFrontDir = normalize(frontDir);
    
    // 렌즈의 NDC 좌표
    float4 lensViewPos = mul(float4(lensWorldPos, 1.f), matView);
    float4 lensClipPos = mul(lensViewPos, matProj);
    float3 lensNDC = lensClipPos.xyz / lensClipPos.w;
    
    // 렌즈의 스크린 좌표
    float2 lensScreenPos;
    lensScreenPos.x = (lensNDC.x * 0.5 + 0.5) * g_Resolution.x;
    lensScreenPos.y = (lensNDC.y * -0.5 + 0.5) * g_Resolution.y; // Y축은 뒤집어야 합니다
    
    // 렌즈의 스크린 UV 좌표
    float2 lensScreenUV = lensScreenPos / g_Resolution;
    
    if (lensScreenUV.x > 1)
        return float4(1.f, 0.f, 0.f, 1.f);
    
    // 현재 지점의 Depth
    float depth = g_tex_1.Sample(g_sam_0, vScreenUV).r;
    // 깊이 값을 월드 스케일 깊이 값으로 변환
    float worldDepth = near + (far - near) * depth;
    
    
    // 렌즈로부터 현재 지점의 렌즈 front방향 수직거리 : 오브젝트 거리 p
    float objectDistance = worldDepth;
    
    //렌즈 배율 계산 ( 일단 objectDistance 50으로 설정)
    float lensMagnification = FOCAL_LENGTH / (FOCAL_LENGTH - 50);
    
    // 현재 위치를 렌즈 중심 기준 좌표계로 변환
    vScreenUV -= lensScreenUV;
    
    // 렌즈 배율 적용
    vScreenUV /= lensMagnification;
    
    // UV를 다시 화면 좌상단 기준 좌표계로 변환
    vScreenUV += lensScreenUV;
    
    
    float2 voriginalScreenUV = _in.vPosition.xy / g_Resolution;
    float4 vColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    
    return vColor;
}


//======================================================================================================
//ExtractBright

float4 PS_ExtractBright(VS_OUT _in) : SV_Target
{
    float2 vScreenUV = _in.vPosition.xy / g_Resolution;
    
    float threshold = 0.5;
    
    float4 originalColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    float brightness = dot(originalColor.rgb, float3(0.2126, 0.7152, 0.0722)); //https://en.wikipedia.org/wiki/Relative_luminance
    
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    if (brightness > threshold)
    {
        vColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    }

    return vColor;
}


// ==========================
// Blur Shader
// Mesh     : RectMesh
// DSTYPE   : NO_TEST_NO_WRITE
// g_tex_0  : Target
// ===========================
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

static float CrossFilter[13] = { 0.0561f, 0.1353f, 0.278f, 0.4868f, 0.7261f, 0.9231f, 1.f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f };
static float Total = 6.2108f;

struct VS_OUT_BLUR
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float InstID : FOG;
};

VS_OUT_BLUR VS_Blur(VS_IN _in, uint _InstID : SV_InstanceID)
{
    VS_OUT_BLUR output = (VS_OUT_BLUR) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    output.InstID = _InstID;
    
    return output;
}

float4 PS_Blur(VS_OUT_BLUR _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
        
    float2 vUVStep = 1.f / g_Resolution;
    vUVStep *= 3.4f;
    
    if (_in.InstID == 0)
    {
        for (int i = 0; i < 13; ++i)
        {
            float2 vUV = _in.vUV + float2(vUVStep.x * (-6 + i), 0.f);
            vColor += g_tex_0.Sample(g_sam_2, vUV) * CrossFilter[i];
        }
    }
    else if (_in.InstID == 1)
    {
        for (int j = 0; j < 13; ++j)
        {
            float2 vUV = _in.vUV + float2(0.f, vUVStep.y * (-6 + j));
            vColor += g_tex_0.Sample(g_sam_2, vUV) * CrossFilter[j];
        }
    }
    vColor /= Total;
    
    return vColor;
}


VS_OUT VS_EffectMerge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_EffectMerge(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    float4 vEffect = g_tex_0.Sample(g_sam_0, _in.vUV);
    float4 vEffectBlur = g_tex_1.Sample(g_sam_0, _in.vUV);
    
    float4 vBloom = pow(abs(pow(vEffect, 2.2f)) + abs(pow(vEffectBlur, 2.2f)), 1.f / 2.2f);
    vBloom = saturate(vBloom);
    return vBloom;
}

#endif