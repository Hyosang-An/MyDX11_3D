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

tPhongShadingLight CalculateLight3D(int _LightIdx, float3 _ViewNormal, float3 _ViewPos)
{
    tLightInfo LightInfo = g_Light3D[_LightIdx];
    
    float LightPow = 0.f;
    float SpecularPow = 0.f;
    float Ratio = 1.f;
    //float SpecularRatio = 1.f;
    
    tPhongShadingLight light = (tPhongShadingLight) 0.f;
    
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
        // Spot Light 각도와 방향
        float angle = LightInfo.Angle;
        // view 공간에서 광원의 위치
        float3 vLightViewPos = mul(float4(LightInfo.WorldPos, 1.f), matView).xyz;
        // view 공간에서 픽셀이 spot light 내부에 있는지 확인
        float3 spotlightDir_View = normalize(mul(float4(LightInfo.WorldDir, 0.f), matView).xyz);
        
         // spot light 범위 밖인 경우 
        if (dot(normalize(_ViewPos - vLightViewPos), spotlightDir_View) < cos(angle))
            return light;
        
        // diffuse
        float3 vLightDir = normalize(-vLightViewPos + _ViewPos);
        LightPow = saturate(dot(-vLightDir, _ViewNormal));
        
        // specular
        // vR = vL + 2 * dot(-vL, vN) * vN;
        float3 vReflect = normalize(vLightDir + 2 * dot(-vLightDir, _ViewNormal) * _ViewNormal);
        float3 vEyeDir = normalize(-_ViewPos);
        float powCoef = 25;
        SpecularPow = pow(saturate(dot(vReflect, vEyeDir)), powCoef);
        
        // 거리에 따른 빛의 세기
        float dist = distance(vLightViewPos, _ViewPos);
        Ratio = saturate(cos((PI / 2.f) * saturate(dist / LightInfo.Radius)));
        
    }
    
    //// 최종 색상 계산
    //_Light.Color.rgb += LightInfo.light.Color.rgb * LightPow * Ratio;
    //_Light.Ambient.rgb += LightInfo.light.Ambient.rgb * Ratio;
    //_Light.SpecularCoef += LightInfo.light.SpecularCoef * SpecularPow * SpecularRatio;
    
    
    // diffuse + ambient + specular
    

    
    // diffuse
    float3 diffuse = LightInfo.light.Color.rgb * LightPow * Ratio;
    
    // ambient
    float3 ambient = LightInfo.light.Ambient.rgb * Ratio;
    
    // specular
    float3 specular = LightInfo.light.Color.rgb * LightInfo.light.SpecularCoef * SpecularPow * Ratio;
    
    light.Diffuse = diffuse;
    light.Ambient = ambient;
    light.Specular = specular; // SpecularCoef값에 임시로 specular값을 넣어둠
    
    return light;
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


float GetTessFactor(float _MinLevel, float _MaxLevel
                  , float _MinRange, float _MaxRange
                  , float3 _CamPos, float3 _Pos)
{
    float D = distance(_CamPos, _Pos);
    
    if (D < _MaxRange)
        return pow(2.f, _MaxLevel);
    else if (_MinRange < D)
        return pow(2.f, _MinLevel);
    else
    {
        float fRatio = 1.f - (D - _MaxRange) / abs(_MaxRange - _MinRange);
        
        float Level = 1.f + fRatio * (_MaxLevel - _MinLevel - 1.f);
        
        return pow(2.f, Level);
    }
}


int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir
                  , out float3 _CrossPos, out uint _Dist)
{
    // 삼각형 표면 방향 벡터
    float3 Edge[2] = { (float3) 0.f, (float3) 0.f };
    Edge[0] = _Pos[1] - _Pos[0];
    Edge[1] = _Pos[2] - _Pos[0];
    
    // 삼각형에 수직방향인 법선(Normal) 벡터
    float3 Normal = normalize(cross(Edge[0], Edge[1]));
    
    // 삼각형 법선벡터와 Ray 의 Dir 을 내적
    // 광선에서 삼각형으로 향하는 수직벡터와, 광선의 방향벡터 사이의 cos 값
    float NdotD = -dot(Normal, _vDir);
        
    float3 vStoP0 = _vStart - _Pos[0];
    float VerticalDist = dot(Normal, vStoP0); // 광선을 지나는 한점에서 삼각형 평면으로의 수직 길이
            
    // 광선이 진행하는 방향으로, 삼각형을 포함하는 평면까지의 거리
    float RtoTriDist = VerticalDist / NdotD;
        
    // 광선이, 삼각형을 포함하는 평면을 지나는 교점
    float3 vCrossPoint = _vStart + RtoTriDist * _vDir;
        
    // 교점이 삼각형 내부인지 테스트
    float3 P0toCross = vCrossPoint - _Pos[0];
    
    float3 Full = cross(Edge[0], Edge[1]);
    float3 U = cross(Edge[0], P0toCross);
    float3 V = cross(Edge[1], P0toCross);
       
    // 직선과 삼각형 평면의 교점이 삼각형 1번과 2번 사이에 존재하는지 체크
    //      0
    //     /  \
    //    1 -- 2    
    if (dot(U, Full) < 0.f)
        return 0;
    
    // 교점이 삼각형 내부인지 체크
    if (length(Full) < length(U) + length(V))
        return 0;
        
    _CrossPos = vCrossPoint;
    _Dist = (uint) RtoTriDist;
    
    return 1;
}



#endif