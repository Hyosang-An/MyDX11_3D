#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

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
struct VS_DIR_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_DIR_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


VS_DIR_OUT VS_DirLight(VS_DIR_IN _in)
{
    VS_DIR_OUT output = (VS_DIR_OUT) 0.f;
        
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET;
    float4 vSpecular : SV_TARGET1;
};


PS_OUT PS_DirLight(VS_DIR_OUT _in)
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





#endif