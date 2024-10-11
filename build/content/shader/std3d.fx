#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"

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
    
    if (vOutColor.a == 0.f)
        discard;
    
    tLight light = (tLight) 0.f;
    
    //for (int i = 0; i < g_Light3DCount; ++i)
    //{
    //    CalculateLight3D(i, _in.vViewNormal, _in.vViewPos, light);
    //}
    
    //// 계산한 빛을 물체의 색상에 적용
    //vOutColor.xyz = vOutColor.xyz * light.Color.rgb
    //                + vOutColor.xyz * light.Ambient.rgb
    //                + g_Light3D[0].light.Color.rgb * light.SpecularCoef;
    
    
    float3 accLight = float3 (0.f, 0.f, 0.f);
    
    for (int i = 0; i < g_Light3DCount; ++i)
    {
        accLight += CalculateLight3D(vOutColor.xyz, i, _in.vViewNormal, _in.vViewPos);
    }
    
    vOutColor.xyz = accLight;
    
        
    return vOutColor;
}

#endif