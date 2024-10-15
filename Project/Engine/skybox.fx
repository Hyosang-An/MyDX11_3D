#ifndef _SKYBOX
#define _SKYBOX

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
    float3 vLocalDir : TEXCOORD1;
};

VS_OUT VS_SkyBox(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    float4 vPos = mul(mul(float4(_in.vPos, 0.f), matView), matProj);
    
    // 스카이박스는 카메라에서 무한히 멀리 있는 배경처럼 보이기 때문에, 카메라의 z 축에서 일정한 깊이를 가지도록 z 값을 w 값으로 설정하여 
    // z 버퍼 깊이에 영향을 주지 않도록 합니다. 이를 통해 스카이박스가 씬의 다른 물체보다 항상 멀리 있는 것처럼 렌더링됩니다.
    vPos.z = vPos.w;
    
    output.vPosition = vPos;
    output.vUV = _in.vUV;                   // Sphere 매핑 시, UV 좌표가 필요
    output.vLocalDir = normalize(_in.vPos); //큐브맵 샘플링 시, 정점의 방향 벡터가 필요
    
    return output;
}

float4 PS_SkyBox(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(0.4f, 0.4f, 0.4f, 1.f);
        
    // Sphere
    if (0 == g_int_0)
    {
        if (g_btex_0)
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    // Cube
    else if (1 == g_int_0)
    {
        if (g_btexcube_0)
            vColor = g_texcube_0.Sample(g_sam_0, _in.vLocalDir);
    }
    
    return vColor;
}

#endif