#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET;
    float4 vSpecular : SV_TARGET1;
};

// ================================
// Directional Light Shader 
// MRT  : LIGHT (Diffuse, Specular)
// Mesh : RectMesh
// Rasterizer   : CULL_BACK
// DepthStencil : NO_TEST_NO_WRITE
// BlendState   : ONE_ONE  (기존의 렌더타겟에 픽셀쉐이더에서 계산된 값을 1:1로 더한다. light shader에서는 빛을 누적시키는 효과)
// Parameter
#define LIGHT_IDX       g_int_0
#define POS_TARGET      g_tex_0
#define NORMAL_TARGET   g_tex_1
// ================================


VS_OUT VS_DirLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;    
    
    return output;
}




PS_OUT PS_DirLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // 픽셀쉐이더랑 동일한 지점에서 Position 값을 확인한다.
    float4 vViewPos = POS_TARGET.Sample(g_sam_0, _in.vUV);
    
    // 광원이 부딪힐 물체가 존재하지 않는다.
    if (0.f == vViewPos.w)
        discard;
    
    float3 vViewNormal = NORMAL_TARGET.Sample(g_sam_0, _in.vUV).xyz;
    
    // 해당 지점이 받을 빛의 세기를 계산한다.
    tPhongShadingLight light = (tPhongShadingLight) 0.f;
    light = CalculateLight3D(LIGHT_IDX, vViewNormal, vViewPos.xyz);
        
    output.vDiffuse.rgb = light.Diffuse + light.Ambient; //diffuse + ambient
    output.vSpecular.rgb = light.Specular;
    
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}


// ================================
// Point Light Shader
// MRT  : LIGHT (Diffuse, Specular)
// Mesh : SphereMesh
// Rasterizer   : CULL_FRONT, (CULL_NONE 하면 광원처리 2중첩 되는 문제가 있다)
// DepthStencil : NO_TEST_NO_WRITE
// BlendState   : ONE_ONE
// Parameter
//#define LIGHT_IDX       g_int_0
//#define POS_TARGET      g_tex_0
//#define NORMAL_TARGET   g_tex_1 
// 뷰 역행렬
// 월드 역행렬
// ================================

VS_OUT VS_PointLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_PointLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // 픽셀쉐이더랑 동일한 지점에서 Position 값을 확인한다.
    float2 vScreenUV = _in.vPosition.xy/*픽셀 좌표*/ / g_Resolution;
    float4 vViewPos = POS_TARGET.Sample(g_sam_0, vScreenUV);
    
    // 광원이 부딪힐 물체가 존재하지 않는다.
    if (0.f == vViewPos.w)
        discard;
    
     // 해당 물체가 볼륨메쉬 내부인지 판정
    // 추출한 물체의 ViewPos 를 WorldPos 로 변경한다.
    float3 vWorldPos = mul(float4(vViewPos.xyz, 1.f), matViewInv).xyz;
    
    // World 상에 있는 물체의 좌표를, Volume Mesh 의 월드 역행렬을 곱해서 Local 공간으로 데려간다.
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), matWorldInv).xyz;
    
    // 물체가 볼륨메쉬 영역 밖이라면 광원계산 중단
    if (0.5f < length(vLocalPos))
    {
        discard;
    }
    
    float3 vViewNormal = NORMAL_TARGET.Sample(g_sam_0, vScreenUV).xyz;
    
    // 해당 지점이 받을 빛의 세기를 계산한다.
    tPhongShadingLight light = (tPhongShadingLight) 0.f;
    light = CalculateLight3D(LIGHT_IDX, vViewNormal, vViewPos.xyz);
        
    output.vDiffuse.rgb = light.Diffuse + light.Ambient;
    output.vSpecular.rgb = light.Specular;
    
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}



#endif