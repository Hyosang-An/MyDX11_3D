#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"
#include "func.fx"
#include "struct.fx"

// ================
// LandScapeShader
// MRT          : Deferred
// RS_TYPE      : CULL_BACK
// BS_TYPE      : Default
// DS_TYPE      : Less

// ================
// Parameter
#define FaceX               g_int_0
#define FaceZ               g_int_1
#define MODE                g_int_2

#define MIN_LEVEL           g_vec4_0.x
#define MAX_LEVEL           g_vec4_0.y
#define MIN_RANGE           g_vec4_0.z
#define MAX_RANGE           g_vec4_0.w

#define CAM_POS             g_vec4_1.xyz

#define COLOR_TEX           g_texarr_0
#define NORMAL_TEX          g_texarr_1
#define HasColorTex         g_btexarr_0
#define HasNormalTex        g_btexarr_1
#define TEXTURE_ARRSIZE     g_int_3

#define HeightMap           g_tex_0

#define IsShowBrush         g_btex_1 && g_float_0 // float0 는 raycast success 변수
#define BRUSH_TEX           g_tex_1
#define BrushScale          g_vec2_0
#define BrushPos            g_vec2_1

StructuredBuffer<tWeight8> WEIGHT_MAP : register(t20);
#define WEIGHT_RESOLUTION   g_vec2_2


// ================
struct VS_IN
{
    float3 vPos : POSITION;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    float3 vWorldPos : POSITION1;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
};

VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vLocalPos = _in.vPos;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    output.vNormal = _in.vNormal;
    output.vTangent = _in.vTangent;
    output.vBinormal = _in.vBinormal;
    
    output.vUV = _in.vUV;
    
    return output;
}

// Hull Shader
// - Patch Constant Function (패치 상수 함수)
//   - 패치당 호출되는 함수
// - Hull Shader Main 함수
//   - 각 정점마다 호출되는 함수
struct TessFactor
{
    float arrEdge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

TessFactor PatchConstantFunc(InputPatch<VS_OUT, 3> _in, uint _PatchIdx : SV_PrimitiveID)
{
    TessFactor output = (TessFactor) 0.f;
    
    // 0
	// | \
	// 2--1
    
    // OR
    
	// 1--2 
	//  \ |
	//    0
    
    // 위 또는 아래
    output.arrEdge[0] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_in[1].vWorldPos + _in[2].vWorldPos) * 0.5f);
    
    // 좌 또는 우
    output.arrEdge[1] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_in[0].vWorldPos + _in[2].vWorldPos) * 0.5f);
    
    // 빗변
    output.arrEdge[2] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_in[0].vWorldPos + _in[1].vWorldPos) * 0.5f);
    
    // 삼각형 중심
    output.Inside = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                , MIN_RANGE, MAX_RANGE, CAM_POS
                                , (_in[0].vWorldPos + _in[1].vWorldPos + _in[2].vWorldPos) / 3.f);
    
    return output;
}

struct HS_OUT
{
    float3 vLocalPos : POSITION;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
};

[domain("tri")]
[partitioning("integer")]
//[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunc")]
[maxtessfactor(64)]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _in
           , uint _PatchIdx : SV_PrimitiveID
           , uint _ControlPointID : SV_OutputControlPointID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vLocalPos = _in[_ControlPointID].vLocalPos;
    
    output.vNormal = _in[_ControlPointID].vNormal;
    output.vTangent = _in[_ControlPointID].vTangent;
    output.vBinormal = _in[_ControlPointID].vBinormal;
    
    output.vUV = _in[_ControlPointID].vUV;
    
    return output;
}


// Domain Shader

struct DS_OUT
{
    float4 Position : SV_Position;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
};

[domain("tri")]
DS_OUT DS_LandScape(OutputPatch<HS_OUT, 3> _in, float3 _Weight : SV_DomainLocation
                , TessFactor _PatchTessFactor)
{
    HS_OUT input = (HS_OUT) 0.f;
    DS_OUT output = (DS_OUT) 0.f;
    
    for (int i = 0; i < 3; ++i)
    {
        input.vLocalPos += _in[i].vLocalPos * _Weight[i];
        
        input.vNormal += _in[i].vNormal * _Weight[i];
        input.vTangent += _in[i].vTangent * _Weight[i];
        input.vBinormal += _in[i].vBinormal * _Weight[i];
        
        input.vUV += _in[i].vUV * _Weight[i];
    }
        
    
// 높이맵이 있다면
    if (g_btex_0)
    {
        float2 vHeightMapUV = saturate(float2(input.vLocalPos.x / (float) FaceX
                                    , 1.f - (input.vLocalPos.z / (float) FaceZ)));
        
        if (0.f == vHeightMapUV.x || 1.f == vHeightMapUV.x ||
            0.f == vHeightMapUV.y || 1.f == vHeightMapUV.y)
        {

        }
        else
            input.vLocalPos.y = HeightMap.SampleLevel(g_sam_0, vHeightMapUV, 0).x;
        
        // 패치 분할레벨을 정점 간격으로 잡는다.
        float fLocalStep = 1.f / _PatchTessFactor.Inside;
                
        float3 arrUDLR[4] =
        {
            float3(input.vLocalPos.x, 0.f, input.vLocalPos.z + fLocalStep),
            float3(input.vLocalPos.x, 0.f, input.vLocalPos.z - fLocalStep),
            float3(input.vLocalPos.x - fLocalStep, 0.f, input.vLocalPos.z),
            float3(input.vLocalPos.x + fLocalStep, 0.f, input.vLocalPos.z)
        };
        
        for (int i = 0; i < 4; ++i)
        {
            float2 vUV = saturate(float2(arrUDLR[i].x / (float) FaceX, 1.f - (arrUDLR[i].z / (float) FaceZ)));
            arrUDLR[i].y = HeightMap.SampleLevel(g_sam_0, vUV, 0).x;
            
            // 로컬 좌표들을 월드 좌표계로 변환 (월드에서 방향벡터 계산을 해야 되므로)
            arrUDLR[i] = mul(float4(arrUDLR[i], 1.f), matWorld).xyz;
        }
        
        float3 vTangent = normalize(arrUDLR[3] - arrUDLR[2]);
        float3 vBinormal = normalize(arrUDLR[1] - arrUDLR[0]);
        float3 vNormal = cross(vTangent, vBinormal);
                
        output.vViewTangent = normalize(mul(float4(vTangent, 0.f), matView));
        output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), matView));
        output.vViewNormal = normalize(mul(float4(vNormal, 0.f), matView));
    }
    else
    {
        output.vViewNormal = normalize(mul(float4(input.vNormal, 0.f), matWV));
        output.vViewTangent = normalize(mul(float4(input.vTangent, 0.f), matWV));
        output.vViewBinormal = normalize(mul(float4(input.vBinormal, 0.f), matWV));
    }
    
    output.Position = mul(float4(input.vLocalPos, 1.f), matWVP);
    output.vViewPos = mul(float4(input.vLocalPos, 1.f), matWV);
        
    output.vUV = input.vUV;
    
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

PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float4 vBrush = (float4) 0.f;
    
    if (IsShowBrush && MODE)
    {
        // Brush LeftTop 좌표
        float2 BrushLT = BrushPos - (BrushScale * 0.5f);
        
        // 지형 기준, 픽셀의 위치 구하기
        float2 vBrusUV = _in.vUV / float2(FaceX, FaceZ);
        vBrusUV = (vBrusUV - BrushLT) / BrushScale;
        
        if (0.f <= vBrusUV.x && vBrusUV.x <= 1.f
            && 0.f <= vBrusUV.y && vBrusUV.y <= 1.f)
        {
            float BrushAlpha = BRUSH_TEX.Sample(g_sam_2, vBrusUV).a;
            float3 BrushColor = float3(0.8f, 0.8f, 0.f);
            
            //vBrush.rgb = (vBrush.rgb * (1 - BrushAlpha)) + (BrushColor * BrushAlpha);
            vBrush.rgb = (BrushColor * BrushAlpha);
        }
    }
       
    
    float4 vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    float3 vViewNormal = _in.vViewNormal;
    
    if (HasColorTex)
    {
        // 편미분
        float2 derivX = ddx(_in.vUV);
        float2 derivY = ddy(_in.vUV);
        
        float2 vFullUV = _in.vUV / float2(FaceX, FaceZ); // _in.vUV는 0~FaceX, 0~FaceZ 범위
        int2 vColRow = vFullUV * WEIGHT_RESOLUTION;
        int WeightMapIdx = WEIGHT_RESOLUTION.x * vColRow.y + vColRow.x;

        vColor = (float4) 0.f;
        
        int MaxIdx = -1;
        float WeightMax = 0.f;
        
        for (int i = 0; i < TEXTURE_ARRSIZE; ++i)
        {
            float Weight = WEIGHT_MAP[WeightMapIdx].arrWeight[i];
            
            if (0.f != Weight)
            {
                //vColor += COLOR_TEX.SampleLevel(g_sam_0, float3(_in.vUV, i), 5) * Weight; //  texture_array.Sample(g_sam_0, float3(uv, layerIndex), lod);
                vColor += COLOR_TEX.SampleGrad(g_sam_0, float3(_in.vUV, i), derivX * 0.35f, derivY * 0.35f) * Weight;
            }
                        
            // 제일 높았던 가중치를 기록
            if (WeightMax < Weight)
            {
                WeightMax = Weight;
                MaxIdx = i;
            }
        }
        
        if (MaxIdx != -1)
        {
            //float3 vNormal = NORMAL_TEX.SampleLevel(g_sam_0, float3(_in.vUV, MaxIdx), 5);
            float3 vNormal = NORMAL_TEX.SampleGrad(g_sam_0, float3(_in.vUV, MaxIdx), derivX * 0.25f, derivY * 0.25f);
            // 추출한 값의 범위를 0 ~ 1 에서 -1 ~ 1 로 변경한다.
            vNormal = vNormal * 2.f - 1.f;
        
            // 표면 좌표계(Tagent Space) 기준 방향을 ViewSpace 적용시키기 위해서
            // 적용시킬 곳의 표면 정보(T, B, N) 이 필요하다.
            // 표면정보로 회전 행렬을 구성한다.
            float3x3 Rot =
            {
                _in.vViewTangent,
                _in.vViewBinormal,
                _in.vViewNormal
            };
            
            // TangentSpace 방향 정보를 적용시킬 좌표계(viewSpace)로 가져온다.
            vViewNormal = normalize(mul(vNormal, Rot));
        }
    }
    
    output.vAlbedo = float4(vColor.xyz, 1.f);
    output.vEmissive = float4(vBrush.rgb, 1.f);
    output.vNormal = float4(vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    
    return output;
}


#endif