#ifndef _DEBUG_SHADER
#define _DEBUG_SHADER

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
};

VS_OUT VS_DebugShape(VS_IN _in)
{
    //VS_OUT output = (VS_OUT) 0.f;
        
    //output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    //output.vUV = _in.vUV;
    
    //return output;
    
    
    
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWorld);
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[maxvertexcount(6)]
void GS_DebugShape(line VS_OUT input[2], inout TriangleStream<GS_OUT> _OutStream)
{
    float thickness = 2; // 원하는 라인의 기본 두께 설정
    
    // world 좌표계 상 방향 벡터 계산
    float3 dir = normalize(input[1].vPosition - input[0].vPosition);
    
    // dir 방향으로 두께만큼 이동한 지점 계산
    input[0].vPosition -= float4(dir * thickness, 0);
    input[1].vPosition += float4(dir * thickness, 0);

    
    // view 좌표계로 변환
    input[0].vPosition = mul(float4(input[0].vPosition), matView);
    input[1].vPosition = mul(float4(input[1].vPosition), matView);
    
    // view좌표계 상 방향 벡터 계산
    float2 dirInViewSpace = normalize(input[1].vPosition.xy - input[0].vPosition.xy);
    
    float2 perp = float2(-dirInViewSpace.y, dirInViewSpace.x) * thickness;
    
    // * ----- * (0 --- 1)
    
    //  0 -- 1
    //  | \  |
    //  3 -- 2    
    
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    output[0].vPosition = input[0].vPosition + float4(perp, 0, 0);
    output[1].vPosition = input[1].vPosition + float4(perp, 0, 0);
    output[2].vPosition = input[1].vPosition - float4(perp, 0, 0);
    output[3].vPosition = input[0].vPosition - float4(perp, 0, 0);
    
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.w = 1.f;
        //output[i].vPosition = mul(output[i].vPosition, matView);
        output[i].vPosition = mul(output[i].vPosition, matProj);
    }

  
    
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    
    // Stream 출력 (삼각형마다)
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
    
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
    return g_vec4_0;
}

#endif