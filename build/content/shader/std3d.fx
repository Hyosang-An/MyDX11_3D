#ifndef _STD3D
#define _STD3D

#include "value.fx"


struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
};


// Gouraud Shading VS 에서 광원의 세기를 구한 뒤, PS 로 넘기는 방식
// Phong Shading  VS 에서 Normal 을 넘기고, PS 에서 광원의 세기를 계산하는 방식


VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    
    // 정점의 ViewSpace 좌표
    output.vViewPos = mul(float4(_in.vPos, 1.f), matWV).xyz;
    
    // 뷰 Space 에서 정점에서의 표면의 수직방향
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), matWV).xyz);
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    
    if (g_btex_0)
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    tLightInfo Light = g_Light3D[0];

    // VS 에서 받은 노말값으로, 빛의 세기를 PS 에서 직접 구한다음 빛의 세기를 적용
    float3 vLightDir = normalize(mul(float4(Light.WorldDir, 0.f), matView).xyz);
    float LightPow = saturate(dot(-vLightDir, _in.vViewNormal));
    
    // 반사광 계산
    // 반사 벡터
    // vR = vL + 2 * dot(-vL, vN) * vN;
    
    float3 vReflect = vLightDir + 2 * dot(-vLightDir, _in.vViewNormal) * _in.vViewNormal;
    
    // eyeDir
    float3 vEyeDir = normalize(-_in.vViewPos);
    
    // 반사광 계산
    float powCoef = 25;
    float SpecularPow = pow(saturate(dot(vReflect, vEyeDir)), powCoef);
    
    if (0 == g_int_0)
    {
        vOutColor.xyz = vOutColor.xyz * LightPow * Light.light.Color.xyz
                        + vOutColor.xyz * Light.light.Ambient.xyz;
    }
    else
    {
        vOutColor.xyz = vOutColor.xyz * LightPow * Light.light.Color.xyz
                        + vOutColor.xyz * Light.light.Ambient.xyz
                        + SpecularPow * Light.light.Color.xyz * Light.light.SpecularCoef;
    }
    
        
    return vOutColor;
}

#endif