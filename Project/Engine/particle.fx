#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"
#include "struct.fx"

StructuredBuffer<tParticle> ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule> ModuleBuffer : register(t21);

#define Module      ModuleBuffer[0]




struct VS_IN
{
    float3 vPos : POSITION;
    uint InstID : SV_InstanceID;
};


struct VS_OUT
{
    float3 vLocalPos : POSITION;
    uint InstID : INSTANCE_ID;
};


VS_OUT VS_Particle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
       
    output.vLocalPos = _in.vPos;
    output.InstID = _in.InstID;
    
    return output;
}


// 1. 비활성화 파티클 처리
// GeometryShader 에서 정점을 생성 및 삭제
// 비활성화된 파티클을 렌더링할 차례면, 출력스트림을 비워서, 중간에 파이프라인 종료시킴

// 2. 파티클 빌보드 처리

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint InstID : INSTANCE_ID;
};

[maxvertexcount(12)]
void GS_Particle(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    GS_OUT output_cross[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    tParticle Particle = ParticleBuffer[_in[0].InstID];
    
    if (false == Particle.Active)
    {
        return;
    }
    
    //  0 -- 1
    //  | \  |
    //  3 -- 2
    float3 scale = Particle.vWorldCurrentScale;
    float3 vViewPos = mul(float4(Particle.vWorldPos, 1.f), matView).xyz;
        
    output[0].vPosition = float4(-scale.x / 2.f, scale.y / 2.f, 0.f, 0.f);
    output[1].vPosition = float4(scale.x / 2.f, scale.y / 2.f, 0.f, 0.f);
    output[2].vPosition = float4(scale.x / 2.f, -scale.y / 2.f, 0.f, 0.f);
    output[3].vPosition = float4(-scale.x / 2.f, -scale.y / 2.f, 0.f, 0.f);
    
    
    if (Module.VelocityAlignment)
    {
    // 빌보드 정점을 초기화합니다. (카메라 뷰 공간 기준)
        output_cross[0].vPosition = float4(-scale.x * 0.5f, 0.f, scale.y * 0.5f, 0.f);
        output_cross[1].vPosition = float4(scale.x * 0.5f, 0.f, scale.y * 0.5f, 0.f);
        output_cross[2].vPosition = float4(scale.x * 0.5f, 0.f, -scale.y * 0.5f, 0.f);
        output_cross[3].vPosition = float4(-scale.x * 0.5f, 0.f, -scale.y * 0.5f, 0.f);

        // 뷰공간에서의 xy평면상(output), xz평면상(output_cross)의 정점들의 x방향(뷰 공간의 right)이 파티클 진행방향과 일치하도록 하기 위해 파티클 진행방향을 right로 설정
        float3 vR = normalize(mul(float4(Particle.vVelocity, 0.f), matView).xyz);
        float3 vF = normalize(cross(vR, float3(0.f, 1.f, 0.f))); // 파티클 진행방향의 수직 벡터를 얻기 위해 그냥 y축을 사용것일 뿐 (x축이나 z축을 사용해도 무방)
        float3 vU = normalize(cross(vF, vR));

        // 뷰 공간에서의 파티클 회전 행렬
        float3x3 vRot =
        {
            vR,
            vU,
            vF,
        };
    
    // 각 정점에 회전 행렬 적용 -> 뷰공간에서의 xy평면상(output), xz평면상(output_cross)의 정점을 파티클 방향으로 회전
        for (int i = 0; i < 4; ++i)
        {
            output[i].vPosition.xyz = mul(output[i].vPosition.xyz, vRot);
            output_cross[i].vPosition.xyz = mul(output_cross[i].vPosition.xyz, vRot);
        }
    }
        
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.xyz += vViewPos;
        output[i].vPosition.w = 1.f;
        output[i].vPosition = mul(output[i].vPosition, matProj);
        output[i].InstID = _in[0].InstID;
    }
    
    
    // 생성시킨 정점의 UV 세팅
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    
    // Stream 출력
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
    
    
    if (Module.VelocityAlignment)
    {
        for (int i = 0; i < 4; ++i)
        {
            output_cross[i].vPosition.xyz += vViewPos;
            output_cross[i].vPosition.w = 1.f;
            output_cross[i].vPosition = mul(output_cross[i].vPosition, matProj);
            output_cross[i].InstID = _in[0].InstID;
        }
    
        // 생성시킨 정점의 UV 세팅
        output_cross[0].vUV = float2(0.f, 0.f);
        output_cross[1].vUV = float2(1.f, 0.f);
        output_cross[2].vUV = float2(1.f, 1.f);
        output_cross[3].vUV = float2(0.f, 1.f);
           
        // OutStream 에 RectMesh 를 만들기 위해서 총 6개의 출력을 지정한다.
        _OutStream.Append(output_cross[0]);
        _OutStream.Append(output_cross[1]);
        _OutStream.Append(output_cross[2]);
        _OutStream.RestartStrip();
    
        _OutStream.Append(output_cross[0]);
        _OutStream.Append(output_cross[2]);
        _OutStream.Append(output_cross[3]);
        _OutStream.RestartStrip();
    }
}

float4 PS_Particle(GS_OUT _in) : SV_Target
{
    float4 vParticleColor = float4(1.f, 0.f, 0.f, 1.f);
    
    if (g_btex_0)
    {
        vParticleColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        vParticleColor *= ParticleBuffer[_in.InstID].vColor;
    }
        
    return vParticleColor;
}




#endif