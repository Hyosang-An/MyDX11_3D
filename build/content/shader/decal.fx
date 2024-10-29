#ifndef _DECAL
#define _DECAL

#include "value.fx"


// ================================
// Decal Shader
// MRT  : Decal ( Albedo, Emissive )
// Mesh : CubeMesh
// Rasterizer   : CULL_FRONT
// DepthStencil : NO_TEST_NO_WRITE
// BlendState   : Albedo - AlphaBlend, Emissive - One_One
// Parameter
#define POS_TARGET      g_tex_0
// 뷰 역행렬
// 월드 역행렬
// ================================

struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

VS_OUT VS_Decal(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    
    return output;
}

struct PS_OUT
{
    float4 vAlbedo : SV_Target0;
    float4 vEmissive : SV_Target1;
};

PS_OUT PS_Decal(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // 픽셀쉐이더랑 동일한 지점에서 Position 값을 확인한다.    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution;
    float4 vViewPos = POS_TARGET.Sample(g_sam_0, vScreenUV);
    
    // 광원이 부딪힐 물체가 존재하지 않는다.
    if (0.f == vViewPos.w)
    {
        discard;
    }
    
    // 해당 물체가 볼륨메쉬 내부인지 판정
    // 추출한 물체의 ViewPos 를 WorldPos 로 변경한다.
    float3 vWorldPos = mul(float4(vViewPos.xyz, 1.f), matViewInv).xyz;
    
    // World 상에 있는 물체의 좌표를, Volume Mesh 의 월드 역행렬을 곱해서 Local 공간으로 데려간다.
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), matWorldInv).xyz;
    
    // 물체가 볼륨메쉬(Cube) 영역 밖이라면 Decal 중단
    if (0.5f < abs(vLocalPos.x) || 0.5f < abs(vLocalPos.y) || 0.5f < abs(vLocalPos.z))
    {
        discard;
    }
    
    float4 vColor = float4(0.f, 1.f, 0.f, 1.f);
    float4 vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    float2 vUV = (float2) 0.f;
    vUV.x = vLocalPos.x + 0.5f;
    vUV.y = 1.f - (vLocalPos.z + 0.5f);
    
    if (g_btex_1)
    {
        vColor = g_tex_1.Sample(g_sam_0, vUV);
    }
    
    if (g_btex_2)
    {
        vEmissive = g_tex_2.Sample(g_sam_0, vUV);
    }
    
    //vColor.rgb *= vColor.a;
    
    //output.vAlbedo = float4(0.f, 0.f, 0.f, 0.f);    
    //output.vEmissive = vColor;
    
    output.vAlbedo = vColor;
    output.vEmissive = float4(vColor.rgb, 1.f) * vColor.a; // emissive는 기존의 값에 누적을 시켜야 하기 때문에 알파블렌드를 쓰지 않지만, 그러면 알파값이 1보다 작은 컬러도 그대로 더해지기 때문에 알파값을 곱해서 더해준다.
   
    return output;
}




#endif