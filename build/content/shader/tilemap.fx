#ifndef _TILEMAP
#define _TILEMAP

#include "value.fx"
#include "func.fx"

struct tTileInfo
{
    int ImgIdx;
    int3 padding;
};


// ===============================
// TileMapShader
// parameter
#define AtlasTex            g_tex_0
#define IsAtlasBind         g_btex_0

#define AtlasMaxRow         g_int_1
#define AtlasMaxCol         g_int_2
#define isTileGridShow      g_int_3
#define TileSliceUV         g_vec2_0
#define TileMapColRow       g_vec2_1
StructuredBuffer<tTileInfo> g_TileInfoBuffer : register(t15);
// ===============================



struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vWorldPos : POSITION;
};

VS_OUT VS_TileMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // TileMap 이 우측 하단으로 확장되도록 하기 위해서 
    // Local RectMesh 의 좌표를 수정한 후 상태행렬을 곱하게 한다.
    _in.vPos.x += 0.5f;
    _in.vPos.y -= 0.5f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV * TileMapColRow;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    return output;
}


float4 PS_TileMap(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
          
    if (IsAtlasBind)
    {
        // 픽셀쉐이더에서 본인의 타일이 몇번째 타일인지 알아내야한다.
        float2 CurColRow = floor(_in.vUV);
        int Idx = TileMapColRow.x * CurColRow.y + CurColRow.x;
        
        // 그 정보로 g_TileInfoBuffer 에 전달된 각 타일정보중 본인의 정보에 접근해서 ImgIdx 를 알아낸다.
        // 알아낸 ImgIdx 로 LeftTopUV 값을 계산한다.        
        // ImgIdx가 atlas의 전체 타일 개수보다 많아도 uv wrap이 되어서 0~1로 계산된다.
        
        if (g_TileInfoBuffer[Idx].ImgIdx != -1)
        {
            int row = g_TileInfoBuffer[Idx].ImgIdx / AtlasMaxCol;
            int col = g_TileInfoBuffer[Idx].ImgIdx % AtlasMaxCol;
            float2 vLeftTopUV = float2(col, row) * TileSliceUV;
            
            if (isTileGridShow)
            {
                // frac(_in.vUV)가 타일 가장자리이면, 그리드 렌더링
                if (frac(_in.vUV).x < 0.025f || frac(_in.vUV).x > 0.975f ||
                frac(_in.vUV).y < 0.025f || frac(_in.vUV).y > 0.975f)
                {
                // 그리드 색상은 회색
                    vOutColor = float4(0.8f, 0.8f, 0.8f, 1.f);
                }
            
            // 타일 내부이면, 타일 렌더링
                else
                {
                    float2 vUV = vLeftTopUV + frac(_in.vUV) * TileSliceUV;
                    vOutColor = AtlasTex.Sample(g_sam_1, vUV);
                }
            }
            
            else
            {
                float2 vUV = vLeftTopUV + frac(_in.vUV) * TileSliceUV;
                vOutColor = AtlasTex.Sample(g_sam_1, vUV);
            }
            
        }
        else
        {
            if (isTileGridShow)
            {
                // frac(_in.vUV)가 타일 가장자리이면, 그리드 렌더링
                if (frac(_in.vUV).x < 0.025f || frac(_in.vUV).x > 0.975f ||
                frac(_in.vUV).y < 0.025f || frac(_in.vUV).y > 0.975f)
                {
                // 그리드 색상은 회색
                    vOutColor = float4(0.8f, 0.8f, 0.8f, 1.f);
                }
                else
                    discard;
            }
            else
                discard;
        }

    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    // 광원 적용
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light); // 광원들만 독립적으로 누적
    }
    
    vOutColor.rgb = vOutColor.rgb * Light.Color.rgb 
                  + vOutColor.rgb * Light.Ambient.rgb;
    
    return vOutColor;
}




#endif