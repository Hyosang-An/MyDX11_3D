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
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
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


// Gouraud Shading VS 에서 광원의 세기를 구한 뒤, PS 로 넘기는 방식
// Phong Shading  VS 에서 Normal 을 넘기고, PS 에서 광원의 세기를 계산하는 방식


VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    
    // 정점의 ViewSpace 좌표
    output.vViewPos = mul(float4(_in.vPos, 1.f), matWV).xyz;
    
    // 뷰 Space 에서 정점에서의 표면의 수직방향
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), matWV).xyz);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), matWV).xyz);
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), matWV).xyz);
    
    
    return output;
}

// 노말맵핑 특징
// 1. 추출한 값의 범위 0 ~ 1 를 -1 ~ 1 로 변환해서 사용해야 한다.
// 2. Normal 텍스처안에 들어있는 노말값의 좌표 축 기준이 다르다.
//  - xyz != xzy(rgb)

// 3. 좌표계
//  - 노말맵의 색감이 전체적으로 다양한 색상
//  - LocalSpace 에서 바로 사용할 수 있는 Normal 값이 저장되어있음, 절대값 개념
//  - 장점 - 연산이 빠르다.
//  - 단점 - 특정 메쉬에만 적용 가능
//         - 로컬 스페이스에서 모양이 정해지지 않은 메쉬에는 적용이 추가적인 조치가 필요하다. 
//           EX) Animation 적용 시, 노말벡터도 같이 애니메이션 변환을 적용 시켜주어야 함

//  - 노말맵의 색감이 전체적으로 푸른색 계열
//  - Tangent(접선) Space 좌표계 기준의 노말 값이 저장되어있음, Relative 개념
//  - 단점 - 계산이 위 방식보다 좀더 걸린다.
//           TagentSpace 에 있는 노말벡터를, 적용 시킬 좌표계로 변환하는 과정이 필요하다.
//    장점 - 범용적으로 사용 가능하다.


float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    float3 vViewNormal = _in.vViewNormal;
    
    if (g_btex_0)
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    tLight light = (tLight) 0.f;
    
    if (g_btex_1)
    {
        vViewNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
              
        // 추출한 값의 범위를 0 ~ 1 에서 -1 ~ 1 로 변경한다.
        vViewNormal = vViewNormal * 2.f - 1.f;
                
        // 표면 좌표계(Tagent Space) 기준 방향을 ViewSpace 적용시키기 위해서
        // 적용시킬 곳의 표면 정보(T, B, N) 이 필요하다.
        // 표면정보로 회전 행렬을 구성한다.
        
        // 정점의 Tangent는 uv 좌표계에서의 u, Binormal은 v 방향이다.
        // 노말맵의 좌표계는 정점에서의 접평면(Tangent Space) 기준으로, x방향은 Tangent, y방향은 Binormal, z방향은 Normal 이다.
        
        // 1 0 0         [T
        // 0 1 0 *  R =   B
        // 0 0 1          N]
        
        // R은 Tangent Space 에서 ViewSpace 로 변환시킬 회전 행렬이다.
        
        float3x3 matRot =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal    
        };
        
        // TangentSpace에서의 노말벡터를 ViewSpace로 변환한다.
        vViewNormal = mul(vViewNormal, matRot);
    }
    
    
    tPhongShadingLight accLight = (tPhongShadingLight) 0.f;
    
    for (int i = 0; i < g_Light3DCount; ++i)
    {
        tPhongShadingLight Light = CalculateLight3D(i, vViewNormal, _in.vViewPos);
        
        accLight.Diffuse += Light.Diffuse;
        accLight.Ambient += Light.Ambient;
        accLight.Specular += Light.Specular;
    }
    
    // 계산한 빛을 물체의 색상에 적용
    
    vOutColor.xyz = vOutColor.rgb * accLight.Diffuse +
                    vOutColor.rgb * accLight.Ambient +
                    accLight.Specular;
    vOutColor.w = 1.f;
        
    return vOutColor;
}

#endif