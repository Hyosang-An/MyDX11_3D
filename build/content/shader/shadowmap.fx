#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vProjPos : POSITION;
};

VS_OUT VS_DirLightShadowMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    
    // output.vPosition == (Px * Vz, Py * Vz, Pz * Vz, Vz)
    output.vProjPos = output.vPosition.xyz / output.vPosition.w;
        
    return output;
}


float PS_DirLightShadowMap(VS_OUT _in) : SV_Target
{
    return _in.vProjPos.z;
}






#endif