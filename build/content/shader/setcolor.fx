#ifndef _SETCOLOR
#define _SETCOLOR

#include "value.fx"

RWTexture2D<float4> g_TargetTex : register(u0);
#define COLOR g_vec4_0

// SV_GroupID : 스레드가 속한 그룹의 아이디를 받는다.
// SV_GroupThreadID : 속한 그룹에서 스레드의 아이디
// SV_GroupIndex : SV_GroupThreadID 를 1차원으로 변환
// SV_DispatchThreadID : 전체기준 ID -> 텍스쳐의 픽셀 좌표

// 그룹당 스레드 수
[numthreads(32, 32, 1)]
void CS_SetColor(int3 _ID : SV_DispatchThreadID)
{
    // g_int_0 : width, g_int_1 : height
    // 픽셀을 초과해서 접근하는 스레드(초과 배정된 스레드)
    if (g_int_0 <= _ID.x || g_int_1 <= _ID.y)
        return;
    
    g_TargetTex[_ID.xy] = COLOR;
}

#endif