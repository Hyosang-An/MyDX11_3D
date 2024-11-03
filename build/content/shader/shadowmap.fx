#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "value.fx"


struct VS_IN
{
    float3 vPos : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vProjPos : POSITION;
};

VS_OUT VS_DirLightShadowMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
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