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
    
    float LightPow : LightPow;
    float3 vWorldNormal : NORMAL;
};


static float3 g_LightDir = normalize(float3(1.f, -1.f, 1.f));
static float3 g_LightColor = float3(1.f, 1.f, 1.f);
static float3 g_Ambient = float3(0.15f, 0.15f, 0.15f);


// Gouraud Shading VS 에서 광원의 세기를 구한 뒤, PS 로 넘기는 방식
// Phong Shading  VS 에서 Normal 을 넘기고, PS 에서 광원의 세기를 계산하는 방식


VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    
    // 월드스페이스에서의 광원의 진행 방향
    float3 vLightDir = normalize(g_LightDir);
    
    // 월드에서 정점에서의 표면의 수직방향
    output.vWorldNormal = normalize(mul(float4(_in.vNormal, 0.f), matWorld));
        
    // 정점에 도달한 빛의 세기
    output.LightPow = saturate(dot(-vLightDir, output.vWorldNormal));
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    

    // VS 에서 받은 노말값으로, 빛의 세기를 PS 에서 직접 구한다음 빛의 세기를 적용
    float3 vLightDir = normalize(g_LightDir);
    float LightPow = saturate(dot(-vLightDir, _in.vWorldNormal));
    vOutColor.xyz *= (LightPow + g_Ambient);
    
    // 반사 벡터
    // vR = vL + 2 * dot(-vL, vN) * vN;
    
        
    return vOutColor;
}










#endif