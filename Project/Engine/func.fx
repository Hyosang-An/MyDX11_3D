#ifndef _FUNC
#define _FUNC

#include "value.fx"
#include "struct.fx"

int IsBinding(Texture2D _tex)
{
    int width = 0, height = 0;
    _tex.GetDimensions(width, height);
    
    if (0 == width || height == 0)
        return false;
    
    return true;
}

void CalculateLight2D(int _LightIdx, float3 _WorldPos, inout tLight _Light)
{
    tLightInfo Info = g_Light2D[_LightIdx];
    
    // DirectionalLight 인 경우
    if (0 == Info.Type)
    {
        _Light.Color.rgb += Info.light.Color.rgb;
        _Light.Ambient.rgb += Info.light.Ambient.rgb;
    }
    
    // PointLight 인 경우
    else if (1 == Info.Type)
    {
         // 점광원과 픽셀까지의 거리
        float fDist = distance(Info.WorldPos.xy, _WorldPos.xy);
        
        // 거리값을 각도로 치환해서 거리에 따른 빛의 세기를 코사인 그래프 형태로 사용한다.
        float fPow = 0.f;
        if (fDist <= Info.Radius)
            fPow = saturate(cos((fDist / Info.Radius) * (PI / 2.f)));
        
        // 광원으로부터 떨어진 거리에 따른 빛의 세기
        //float fPow2 = saturate(1.f - fDist / Info.Radius);
        
        // 최종 색상 계산 = 빛의 색 * 거리에따른 세기
        _Light.Color.rgb += Info.light.Color.rgb * fPow;
        _Light.Ambient.rgb += Info.light.Ambient.rgb;
    }
    
    // SpotLight 인 경우
    else
    {
        // TODO
    }
}

float3 CalculateLight3D(float3 _ObjectColor, int _LightIdx, float3 _ViewNormal, float3 _ViewPos)
{
    tLightInfo LightInfo = g_Light3D[_LightIdx];
    
    float LightPow = 0.f;
    float SpecularPow = 0.f;
    float Ratio = 1.f;
    //float SpecularRatio = 1.f;
    
    // DirectionalLight 인 경우
    if (0 == LightInfo.Type)
    {
        float3 vLightDir = normalize(mul(float4(LightInfo.WorldDir, 0), matView).xyz);
        LightPow = saturate(dot(-vLightDir, _ViewNormal));
        
        // 반사광 계산
        // vR = vL + 2 * dot(-vL, vN) * vN;
        float3 vReflect = normalize(vLightDir + 2 * dot(-vLightDir, _ViewNormal) * _ViewNormal);
        
        // eyeDir
        float3 vEyeDir = normalize(-_ViewPos);
        
        // 반사광 계산
        float powCoef = 15;
        SpecularPow = pow(saturate(dot(vReflect, vEyeDir)), powCoef);        
    }
    
    
    // Point Light인 경우
    else if (1 == LightInfo.Type)
    {
        float3 vLightViewPos = mul(float4(LightInfo.WorldPos, 1.f), matView).xyz;
        float3 vLightDir = normalize(- vLightViewPos + _ViewPos);
        LightPow = saturate(dot(-vLightDir, _ViewNormal));
        
        // 반사광 계산
        // vR = vL + 2 * dot(-vL, vN) * vN;
        float3 vReflect = normalize(vLightDir + 2 * dot(-vLightDir, _ViewNormal) * _ViewNormal);
        
        // eyeDir
        float3 vEyeDir = normalize(-_ViewPos);
        
        // 반사광 계산
        float powCoef = 15;
        SpecularPow = pow(saturate(dot(vReflect, vEyeDir)), powCoef);
        
        // 거리에 따른 빛의 세기
        float dist = distance(vLightViewPos, _ViewPos);
        float cameraDist = length(_ViewPos);
        //Ratio = saturate(1.f - (Distance / LightInfo.Radius));
        Ratio = saturate(cos((PI / 2.f) * saturate(dist / LightInfo.Radius)));
        //SpecularRatio = saturate(cos((PI / 2.f) * saturate(cameraDist / LightInfo.Radius)));
    }
    
    // Spot Light인 경우
    else if (2 == LightInfo.Type)
    {
        
    }
    
    //// 최종 색상 계산
    //_Light.Color.rgb += LightInfo.light.Color.rgb * LightPow * Ratio;
    //_Light.Ambient.rgb += LightInfo.light.Ambient.rgb * Ratio;
    //_Light.SpecularCoef += LightInfo.light.SpecularCoef * SpecularPow * SpecularRatio;
    
    
    // diffuse + ambient + specular
    
    // diffuse
    float3 diffuse = _ObjectColor * LightInfo.light.Color.rgb * LightPow * Ratio;
    
    // ambient
    float3 ambient = _ObjectColor * LightInfo.light.Ambient.rgb * Ratio;
    
    // specular
    float3 specular = LightInfo.light.Color.rgb * LightInfo.light.SpecularCoef * SpecularPow * Ratio;
    
    return diffuse + ambient + specular;
    
}


float3 GetRandom(in Texture2D _NoiseTexture, uint _ThreadID, uint _maxThreadId)
{
    float2 vUV = (float2) 0.f;
    vUV.x = ((float) _ThreadID / (float) (_maxThreadId - 1)) + g_EngineTime * 0.5f;
    vUV.y = sin(vUV.x * 20 * PI) * 0.5f + g_EngineTime * 0.1f;
    float3 vRandom = _NoiseTexture.SampleLevel(g_sam_1, vUV, 0).xyz; // 0 : MipLevel 원본 해상도
    
    return vRandom;
}

float2 GetRandomFloat2(uint _ThreadID)
{
    // 1. 해싱 기법을 이용한 단일 랜덤 값 생성
    float p = dot(float2((float) _ThreadID, g_EngineTime), float2(127.1f, 311.7f));
    float hashValue = frac(sin(p) * 43758.5453f);

    // 2. Sine 함수 기반 랜덤 값 생성
    float sineValue = frac(sin(_ThreadID * 12.9898f + g_EngineTime * 78.233f) * 43758.5453f);

    return float2(hashValue, sineValue);
}

float3 GetRandomFloat3(uint _ThreadID)
{
    // 1. 해싱 기법을 이용한 랜덤 값 생성
    float3 p = float3((float) _ThreadID, g_EngineTime, (float) (_ThreadID * 31));
    p = float3(dot(p, float3(127.1f, 311.7f, 74.7f)),
               dot(p, float3(269.5f, 183.3f, 246.1f)),
               dot(p, float3(113.5f, 271.9f, 124.6f)));
    
    float3 hashResult = frac(sin(p) * 43758.5453f);

    return hashResult; // 0~1 사이의 float3 값을 반환
}




#endif