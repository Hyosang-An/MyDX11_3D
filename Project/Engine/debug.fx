#ifndef _DEBUG_SHADER
#define _DEBUG_SHADER

#include "value.fx"


struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
};

VS_OUT VS_DebugShape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    output.vUV = _in.vUV;
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), matWV).xyz);
    
    return output;
    
    
    
    //====================================================================================================
    // GS_DebugShape 사용할 때 
    //VS_OUT output = (VS_OUT) 0.f;
    
    //output.vPosition = mul(float4(_in.vPos, 1.f), matWorld);
    //return output;
    // ====================================================================================================
}

//struct GS_OUT
//{
//    float4 vPosition : SV_Position;
//    float2 vUV : TEXCOORD;
//};

//[maxvertexcount(6)]
//void GS_DebugShape(line VS_OUT input[2], inout TriangleStream<GS_OUT> _OutStream)
//{
//    float thickness = 2; // 원하는 라인의 기본 두께 설정
    
//    // world 좌표계 상 방향 벡터 계산
//    float3 dir = normalize(input[1].vPosition - input[0].vPosition);
    
//    // dir 방향으로 두께만큼 이동한 지점 계산
//    input[0].vPosition -= float4(dir * thickness, 0);
//    input[1].vPosition += float4(dir * thickness, 0);

    
//    // view 좌표계로 변환
//    input[0].vPosition = mul(float4(input[0].vPosition), matView);
//    input[1].vPosition = mul(float4(input[1].vPosition), matView);
    
//    // view좌표계 상 방향 벡터 계산
//    float2 dirInViewSpace = normalize(input[1].vPosition.xy - input[0].vPosition.xy);
    
//    float2 perp = float2(-dirInViewSpace.y, dirInViewSpace.x) * thickness;
    
//    // * ----- * (0 --- 1)
    
//    //  0 -- 1
//    //  | \  |
//    //  3 -- 2    
    
//    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
//    output[0].vPosition = input[0].vPosition + float4(perp, 0, 0);
//    output[1].vPosition = input[1].vPosition + float4(perp, 0, 0);
//    output[2].vPosition = input[1].vPosition - float4(perp, 0, 0);
//    output[3].vPosition = input[0].vPosition - float4(perp, 0, 0);
    
//    for (int i = 0; i < 4; ++i)
//    {
//        output[i].vPosition.w = 1.f;
//        //output[i].vPosition = mul(output[i].vPosition, matView);
//        output[i].vPosition = mul(output[i].vPosition, matProj);
//    }

  
    
//    output[0].vUV = float2(0.f, 0.f);
//    output[1].vUV = float2(1.f, 0.f);
//    output[2].vUV = float2(1.f, 1.f);
//    output[3].vUV = float2(0.f, 1.f);
    
    
//    // Stream 출력 (삼각형마다)
//    _OutStream.Append(output[0]);
//    _OutStream.Append(output[2]);
//    _OutStream.Append(output[3]);
//    _OutStream.RestartStrip();
    
//    _OutStream.Append(output[0]);
//    _OutStream.Append(output[1]);
//    _OutStream.Append(output[2]);
//    _OutStream.RestartStrip();
    
//}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
    float4 vColor = g_vec4_0;
    
    if (4 == g_int_0) // sphere인 경우
    {
        float3 vEye = normalize(_in.vViewPos);
        float Alpha = pow(1.f - saturate(dot(vEye, _in.vViewNormal)), 3);
        vColor.a = Alpha;
    }
    
    return vColor;
}


// ====================================================================================================
// DebugLine
// ====================================================================================================
struct VS_LINE_IN
{
    float3 vPos : POSITION;
};

struct VS_LINE_OUT
{
    float3 vPos : POSITION;
};

struct GS_LINE_OUT
{
    float4 vPosition : SV_Position;
};


VS_LINE_OUT VS_DebugLine(VS_LINE_IN _in)
{
    VS_LINE_OUT output = (VS_LINE_OUT) 0.f;
        
    output.vPos = _in.vPos;
    
    return output;
}

[maxvertexcount(12)]
void GS_DebugLine(point VS_LINE_OUT _in[1], inout LineStream<GS_LINE_OUT> _OutStream)
{
    GS_LINE_OUT output[3] = { (GS_LINE_OUT) 0.f, (GS_LINE_OUT) 0.f, (GS_LINE_OUT) 0.f };
    
    // g_vec4_1 : 시작점 위치, g_vec4_2 : 끝점 위치
    output[0].vPosition = mul(mul(g_vec4_1, matView), matProj);
    output[1].vPosition = mul(mul(g_vec4_2, matView), matProj);
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
   
    _OutStream.RestartStrip();
}

float4 PS_DebugLine(GS_LINE_OUT _in) : SV_Target
{
    return g_vec4_0;
}

#endif