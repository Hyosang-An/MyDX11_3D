#ifndef _SKYBOX
#define _SKYBOX

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
    float3 vLocalDir : TEXCOORD1;
};

VS_OUT VS_SkyBox(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    float4 vPos = mul(mul(float4(_in.vPos, 0.f), matView), matProj);
    
    // ��ī�̹ڽ��� ī�޶󿡼� ������ �ָ� �ִ� ���ó�� ���̱� ������, ī�޶��� z �࿡�� ������ ���̸� �������� z ���� w ������ �����Ͽ� 
    // z ���� ���̿� ������ ���� �ʵ��� �մϴ�. �̸� ���� ��ī�̹ڽ��� ���� �ٸ� ��ü���� �׻� �ָ� �ִ� ��ó�� �������˴ϴ�.
    vPos.z = vPos.w;
    
    output.vPosition = vPos;
    output.vUV = _in.vUV;                   // Sphere ���� ��, UV ��ǥ�� �ʿ�
    output.vLocalDir = normalize(_in.vPos); //ť��� ���ø� ��, ������ ���� ���Ͱ� �ʿ�
    
    return output;
}

float4 PS_SkyBox(VS_OUT _in) : SV_Target
{
    float4 vColor = float4(0.4f, 0.4f, 0.4f, 1.f);
        
    // Sphere
    if (0 == g_int_0)
    {
        if (g_btex_0)
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    // Cube
    else if (1 == g_int_0)
    {
        if (g_btexcube_0)
            vColor = g_texcube_0.Sample(g_sam_0, normalize(_in.vLocalDir));
        
        // �¾籤 ��� �κ� ����
        tLightInfo sun = g_Light3D[0];
        float3 sunDir = normalize(sun.WorldDir); // �ȼ� ���̴����� ���
        float fDot = dot(-sunDir, normalize(_in.vLocalDir));
        
        // smoothstep�� ����Ͽ� �ε巯�� ��� ��ȭ ����
        float degree = 2.f;
        float fadeEnd = cos(degree * PI / 180); // 30���� �ڻ��� ��
        float fadeStart = cos((degree + 2) * PI / 180); // fade ���� ���� (��: 32��)
        
        // fDot�� fadeStart���� fadeEnd�� �� �� �ε巴�� ��ȭ
        float intensity = saturate(smoothstep(fadeStart, fadeEnd, fDot));
        vColor = lerp(vColor, float4(1.f, 1.f, 1.f, 1.f), intensity); // vColor = vColor + (targetColor - vColor) * intensity;
        
        
        //tLightInfo sun = g_Light3D[0];
        //sun.WorldDir = normalize(sun.WorldDir);
        
        //float fDot = dot(-sun.WorldDir, normalize(_in.vLocalDir));
        //if (fDot > cos(30 * PI / 180))
        //{
        //    vColor = float4(1.f, 1.f, 1.f, 1.f);
        //}
        
    }
    
    return vColor;
}

#endif