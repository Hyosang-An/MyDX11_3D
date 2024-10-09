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
    
    // Proj ����� ���� ����� �� xyz �� �ڽ��� ViewZ �� �������ִ� �����̴�.
    // W �ڸ��� �ڽ��� ViewZ �� ��µǱ� ������ �̰����� �� xyz �� ������� ���� Proj ��ǥ�� ���´�.
    // ���� Rasterizer State �� ��������� ���� ����� �����ϸ� ���� ������ ���������ο��� �� xyz �� w �� ����� NDC��ǥ�� ����� ����Ѵ�.
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
    
    // Proj ����� ���� ����� �� xyz �� �ڽ��� ViewZ �� �������ִ� �����̴�.
    // W �ڸ��� �ڽ��� ViewZ �� ��µǱ� ������ �̰����� �� xyz �� ������� ���� Proj ��ǥ�� ���´�.
    // ���� Rasterizer State �� ��������� ���� ����� �����ϸ� ���� ������ ���������ο��� �� xyz �� w �� ����� NDC��ǥ�� ����� ����Ѵ�.
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
    // 1. ����Ÿ�� �ػ� ���� (���� �������)    
    // 2. �ȼ����̴��� �ȼ� ��ǥ
    
    //���ؽ� ���̴����� ��µ� Ŭ�� ��ǥ�� ������ ������������ ���� ���� �ܰ��� ����Ʈ ��ȯ���� ���޵˴ϴ�. 
    //�� �������� Ŭ�� ��ǥ�� ���� ��ġ ��ǥ(NDC)�� ��ȯ�ǰ�, �ٽ� ����Ʈ ��ǥ�� ��ȯ�˴ϴ�. 
    //����Ʈ ��ǥ�� ���� Ÿ���� �ػ󵵿� ����� ��ǥ�Դϴ�.

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
    // 1. ����Ÿ�� �ػ� ���� (���� �������)    
    // 2. �ȼ����̴��� �ȼ� ��ǥ

    // �����Ÿ�
    float FOCAL_LENGTH = 100;
    
    // ī�޶��� depth ����
    float near = 1.f;
    float far = 100000.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution;
        
    // ������ world��ġ �� ����
    float3 lensWorldPos = float3(matWorld[3][0], matWorld[3][1], matWorld[3][2]);
    float3 lensFrontDir = normalize(frontDir);
    
    // ������ NDC ��ǥ
    float4 lensViewPos = mul(float4(lensWorldPos, 1.f), matView);
    float4 lensClipPos = mul(lensViewPos, matProj);
    float3 lensNDC = lensClipPos.xyz / lensClipPos.w;
    
    // ������ ��ũ�� ��ǥ
    float2 lensScreenPos;
    lensScreenPos.x = (lensNDC.x * 0.5 + 0.5) * g_Resolution.x;
    lensScreenPos.y = (lensNDC.y * -0.5 + 0.5) * g_Resolution.y; // Y���� ������� �մϴ�
    
    // ������ ��ũ�� UV ��ǥ
    float2 lensScreenUV = lensScreenPos / g_Resolution;
    
    if (lensScreenUV.x > 1)
        return float4(1.f, 0.f, 0.f, 1.f);
    
    // ���� ������ Depth
    float depth = g_tex_1.Sample(g_sam_0, vScreenUV).r;
    // ���� ���� ���� ������ ���� ������ ��ȯ
    float worldDepth = near + (far - near) * depth;
    
    
    // ����κ��� ���� ������ ���� front���� �����Ÿ� : ������Ʈ �Ÿ� p
    float objectDistance = worldDepth;
    
    //���� ���� ��� ( �ϴ� objectDistance 50���� ����)
    float lensMagnification = FOCAL_LENGTH / (FOCAL_LENGTH - 50);
    
    // ���� ��ġ�� ���� �߽� ���� ��ǥ��� ��ȯ
    vScreenUV -= lensScreenUV;
    
    // ���� ���� ����
    vScreenUV /= lensMagnification;
    
    // UV�� �ٽ� ȭ�� �»�� ���� ��ǥ��� ��ȯ
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

#endif