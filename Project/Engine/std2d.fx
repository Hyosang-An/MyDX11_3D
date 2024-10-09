#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"

// Vertex Shader
struct VTX_IN
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    
    float3 vWorldPos : POSITION;
};

VTX_OUT VS_Std2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
  
    // LocalSpace -> WorldSpace
    // float3 x float4x4(matrix)
    // float3 를 float4 로 차수를 맞추어준다.
    // 동차좌표를 1 로 설정, 상태행렬 4행에 들어있는 이동을 적용받겠다는 뜻
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    return output;
}

float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    // FlipBook 을 사용한다.
    if (UseFlipbook)
    {
        // _in.vUV : 스프라이를 참조할 위치를 비율로 환산한 값        
        
        // 아틀라스 내에서 백그라운드의 왼쪽 위 UV 좌표를 계산합니다.
        float2 BackGroundLeftTopInAtlasUV = LeftTopInAtlasUV - (BackGroundSizeInAtlasUV - SliceSizeInAtlasUV) / 2.f;
        
        // 스프라이트가 아틀라스 내에서 참조될 위치를 계산합니다.
        float2 vSpriteInAtlasUV = BackGroundLeftTopInAtlasUV + (_in.vUV * BackGroundSizeInAtlasUV);
        vSpriteInAtlasUV -= OffsetUV;
                
        // 스프라이트의 UV 좌표가 유효한 범위 내에 있는지 검사합니다.
        if (LeftTopInAtlasUV.x <= vSpriteInAtlasUV.x && vSpriteInAtlasUV.x <= LeftTopInAtlasUV.x + SliceSizeInAtlasUV.x
            && LeftTopInAtlasUV.y <= vSpriteInAtlasUV.y && vSpriteInAtlasUV.y <= LeftTopInAtlasUV.y + SliceSizeInAtlasUV.y)
        {
            // 유효한 범위 내에 있으면, 텍스처를 샘플링하여 색상을 가져옵니다.
            vColor = g_AtlasTex.Sample(g_sam_1, vSpriteInAtlasUV);
        }
        else
        {
            //vColor = float4(1.f, 1.f, 0.f, 1.f);
            discard;
        }
    }
    
    // FlipBook 을 사용하지 않는다.
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
        else
        {
            vColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }
    
    if (vColor.a == 0.f)
    {
        discard;
    }
    

    // 광원 적용      
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light); // 광원들만 독립적으로 누적
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb 
               + vColor.rgb * Light.Ambient.rgb;
    
    return vColor;
}



float4 PS_Std2D_Alphablend(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
     
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else
    {
        vColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vColor;
}

#endif