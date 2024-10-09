#ifndef _VALUE
#define _VALUE

#include "struct.fx"

#define PI 3.1415926535f

// 4096
cbuffer OBJECT_POS : register(b0)
{
    row_major matrix matWorld;
    row_major matrix matView;
    row_major matrix matProj;
    
    row_major matrix matWV;
    row_major matrix matWVP;
    
    float3 rightDir;
    float3 upDir;
    float3 frontDir;
    
    int3 padding;
};

cbuffer MATERIAL : register(b1)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    row_major matrix g_mat_0;
    row_major matrix g_mat_1;
    row_major matrix g_mat_2;
    row_major matrix g_mat_3;
    
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;
    int g_btex_4;
    int g_btex_5;
    int g_btexcube_0;
    int g_btexcube_1;
    int g_btexarr_0;
    int g_btexarr_1;
    
    int2 mtrlpadding;
}

cbuffer SPRITE_INFO : register(b2)
{
    float2 LeftTopInAtlasUV;
    float2 SliceSizeInAtlasUV;
    float2 BackGroundSizeInAtlasUV;
    float2 OffsetUV;
    int UseFlipbook;
    int3 SpriteInfoPadding;
}

cbuffer GLOBAL_DATA : register(b3)
{
    // 시간 관련 정보
    float g_DT;
    float g_EngineDT;
    float g_Time;
    float g_EngineTime;
    
    // 렌더타겟의 해상도 정보
    float2 g_Resolution;
    
    // 바인딩 된 구조화버퍼에 광원이 몇개 들어있는지
    int g_Light2DCount;
    int g_Light3DCount;
}

SamplerState g_sam_0 : register(s0); // 이방성 필터 샘플러
SamplerState g_sam_1 : register(s1); // 포인트 필터 샘플러


Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
TextureCube g_texcube_0 : register(t6);
TextureCube g_texcube_1 : register(t7);
Texture2DArray g_texarr_0 : register(t8);
Texture2DArray g_texarr_1 : register(t9);

Texture2D g_AtlasTex : register(t10);

StructuredBuffer<tLightInfo> g_Light2D : register(t11);
StructuredBuffer<tLightInfo> g_Light3D : register(t12);

#endif

