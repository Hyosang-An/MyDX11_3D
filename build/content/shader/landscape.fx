#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"

// ================
// 테셀레이션 테스트
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
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float2 vUV : TEXCOORD;
};

VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vLocalPos = _in.vPos;
    
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
    
    output.arrEdge[0] = 1;
    output.arrEdge[1] = 1;
    output.arrEdge[2] = 1;
    output.Inside = 1;
    
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
    for (int i = 0; i < 3; ++i)
    {
        input.vLocalPos += _in[i].vLocalPos * _Weight[i];
        
        input.vNormal += _in[i].vNormal * _Weight[i];
        input.vTangent += _in[i].vTangent * _Weight[i];
        input.vBinormal += _in[i].vBinormal * _Weight[i];
        
        input.vUV += _in[i].vUV * _Weight[i];
    }
        
    DS_OUT output = (DS_OUT) 0.f;
    
    output.Position = mul(float4(input.vLocalPos, 1.f), matWVP);
    
    output.vViewPos = mul(float4(input.vLocalPos, 1.f), matWV);
    output.vViewNormal = normalize(mul(float4(input.vNormal, 0.f), matWV));
    output.vViewTangent = normalize(mul(float4(input.vTangent, 0.f), matWV));
    output.vViewBinormal = normalize(mul(float4(input.vBinormal, 0.f), matWV));
        
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
    
    output.vAlbedo = float4(1.f, 0.f, 1.f, 1.f);
    output.vNormal = float4(_in.vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    
    return output;
}


#endif