#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

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
    
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};


VS_OUT VS_Std3D_Deferred(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
   
    // 정점의 ViewSpace 좌표
    output.vViewPos = mul(float4(_in.vPos, 1.f), matWV).xyz;
    
    // 뷰 Space 에서 정점에서의 표면의 방향정보를 계산
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), matWV).xyz);
    
    return output;
}



struct PS_OUT
{
    float4 vAlbedo : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vEmissive : SV_Target3;
    float4 vData : SV_Target4;
};


PS_OUT PS_Std3D_Deferred(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
        
    output.vAlbedo = float4(1.f, 0.f, 1.f, 1.f);
    output.vNormal = float4(_in.vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    output.vData = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_btex_0)
        output.vAlbedo = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    if (g_btex_1)
    {
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
              
        // 추출한 값의 범위를 0 ~ 1 에서 -1 ~ 1 로 변경한다.
        vNormal = vNormal * 2.f - 1.f;
               
        // 표면 좌표계(Tagent Space) 기준 방향을 ViewSpace 적용시키기 위해서
        // 적용시킬 곳의 표면 정보(T, B, N) 이 필요하다.
        // 표면정보로 회전 행렬을 구성한다.
        float3x3 matRot =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal    
        };
        
        // TangentSpace 방향 정보를 적용시킬 표면의 좌표계로 가져온다.
        output.vNormal = float4(mul(vNormal, matRot), 1.f);
    }
        
    return output;
}

#endif