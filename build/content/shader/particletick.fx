#ifndef _PARTICLE_TICK
#define _PARTICLE_TICK

#include "value.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<tSpawnCount> SpawnCountBuffer : register(u1);
Texture2D NoiseTex : register(t20);
StructuredBuffer<tParticleModule> Module : register(t21);

#define ParticleObjectPos   g_vec4_0.xyz
#define MAX_COUNT           g_int_0
#define Particle            ParticleBuffer[ThreadID.x]

// Module Check
#define SpawnModule         Module[0].isModuleOn[0]
#define SpawnBurstModule    Module[0].isModuleOn[1]
#define SpawnShapeType      Module[0].SpawnShape
#define AddVelocityModule   Module[0].isModuleOn[2]
#define ScaleModule         Module[0].isModuleOn[3]
#define DragModule          Module[0].isModuleOn[4]
#define NoiseForce          Module[0].isModuleOn[5]
#define Render              Module[0].isModuleOn[6]

[numthreads(1024, 1, 1)]
void CS_ParticleTick(int3 ThreadID : SV_DispatchThreadID)
{
    // 최대 파티클 개수보다 큰 ID는 작업을 수행하지 않음
    if (MAX_COUNT <= ThreadID.x)
        return;
    
    // 스폰 카운트의 현재 값을 읽어옴
    int currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
    
    // 파티클이 활성화되지 않은 경우
    if (!Particle.Active)
    {
        // 스폰 카운트가 0보다 큰 동안 반복
        while (currentSpawnCount > 0)
        {
            int originalCount = 0;
            int decrementedCount = currentSpawnCount - 1;
                        
            // InterlockedCompareExchange를 사용해 스폰 카운트를 원자적으로 감소
            // 다른 스레드와 경합하여 값을 업데이트
            InterlockedCompareExchange(
                SpawnCountBuffer[0].iSpawnCount,
                currentSpawnCount,
                decrementedCount,
                originalCount
            );

            // 만약 currentSpawnCount가 변경 전의 값과 일치하면 파티클을 활성화하고 루프 종료
            if (currentSpawnCount == originalCount)
            {
                float2 vUV = (float2) 0.f;
                
                // 스레드를 UV 로 맵핑하기위해서 ID 를 0~1 범위로 정규화     
                float3 vRandom0 = GetRandom(NoiseTex, ThreadID.x, MAX_COUNT);
                float3 vRandom1 = GetRandom(NoiseTex, ThreadID.x + 1, MAX_COUNT);
                float3 vRandom2 = GetRandom(NoiseTex, ThreadID.x + 2, MAX_COUNT);
                                
                float3 vSpawnPos = (float3) 0.f;
                
                // 0 : Box,  1 : Sphere
                if (0 == SpawnShapeType)
                {
                    vSpawnPos.x = vRandom0.x * Module[0].SpawnShapeScale.x - (Module[0].SpawnShapeScale.x / 2.f);
                    vSpawnPos.y = vRandom0.y * Module[0].SpawnShapeScale.y - (Module[0].SpawnShapeScale.y / 2.f);
                    vSpawnPos.z = vRandom0.z * Module[0].SpawnShapeScale.z - (Module[0].SpawnShapeScale.z / 2.f);
                }
                else if (1 == SpawnShapeType)
                {
                    float fRadius = Module[0].SpawnShapeScale.x;
                    float fBlockRadius = Module[0].BlockSpawnShapeScale.x;
                    float fDifferRadius = fRadius - fBlockRadius;
                        
                    //vSpawnPos = normalize(vRandom1 - 0.5f) * fDifferRadius * vRandom2.x + normalize(vRandom1 - 0.5f) * fBlockRadius;
                    vSpawnPos = normalize(vRandom1 - 0.5f) * (fDifferRadius * vRandom2.x + fBlockRadius); // 방향벡터 * 거리

                }
                                                        
                // Add Velocity Module
                Particle.vVelocity = (float3) 0.f;
                
                if (AddVelocityModule)
                {
                    float fSpeed = Module[0].AddMinSpeed + (Module[0].AddMaxSpeed - Module[0].AddMinSpeed) * vRandom2.x;
                        
                    // Random
                    if (0 == Module[0].AddVelocityType)                        
                        Particle.vVelocity = normalize(vRandom2 - 0.5f) * fSpeed;
                    // FromCenter
                    else if (1 == Module[0].AddVelocityType)                        
                        Particle.vVelocity = normalize(vSpawnPos) * fSpeed;
                    // ToCenter
                    else if (2 == Module[0].AddVelocityType)
                        Particle.vVelocity = -normalize(vSpawnPos) * fSpeed;
                    // Fixed
                    else
                        Particle.vVelocity = normalize(Module[0].AddVelocityFixedDir) * fSpeed;
                }
                    
                    
                Particle.vLocalPos = vSpawnPos;
                Particle.vWorldPos = Particle.vLocalPos + ParticleObjectPos.xyz;
                Particle.vWorldInitScale = (Module[0].vSpawnMaxScale - Module[0].vSpawnMinScale) * vRandom0.x + Module[0].vSpawnMinScale;
                                    
                Particle.vColor = Module[0].vSpawnColor;
                Particle.Mass = 1.f;
                
                Particle.Age = 0.f;
                Particle.NormalizedAge = 0;
                Particle.Life = (Module[0].MaxLife - Module[0].MinLife) * vRandom1.y + Module[0].MinLife;
                Particle.Active = 1;
                
                break;
            }
            
            // 다른 스레드가 값을 변경했을 수 있으므로 현재 스폰 카운트를 업데이트
            currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
        }
    }
    else
    {
        Particle.vForce = float3(0.f, 0.f, 0.f);
        
        // Noise Force Module
        if (NoiseForce)
        {
            // 일정 시간마다 Noise Force 의 방향을 랜덤하게 구함
            if (Module[0].NoiseForceTerm <= Particle.NoiseForceAccTime)
            {
                Particle.NoiseForceAccTime -= Module[0].NoiseForceTerm;
                
                float3 vRandom = GetRandom(NoiseTex, ThreadID.x, MAX_COUNT);
                float3 vNoiseForce = normalize(vRandom.xyz - 0.5f);

                //Particle.NoiseForceDir = normalize(vRandom.xyz - 0.5f);
            }
                        
            Particle.vForce += Particle.NoiseForceDir * Module[0].NoiseForceScale;
            Particle.NoiseForceAccTime += g_EngineDT;
        }
        
        // Particle 에 주어진 힘에 따른 가속도 계산
        float3 vAccel = Particle.vForce / Particle.Mass;
        
        // 가속도에 따른 속도의 변화
        Particle.vVelocity += vAccel * g_EngineDT;
                        
        // Velocity 에 따른 이동 구현        
        if (0 == Module[0].SpaceType)
        {
            // Space 가 Local 이라면
            Particle.vLocalPos += Particle.vVelocity * g_EngineDT;
            Particle.vWorldPos = Particle.vLocalPos + ParticleObjectPos.xyz;
        }
        else
        {
            Particle.vLocalPos += Particle.vVelocity * g_EngineDT;
            Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
        }
        
        // Scale 모듈에 따른 현재 크기 계산
        Particle.vWorldCurrentScale = Particle.vWorldInitScale;
        if (ScaleModule)
            Particle.vWorldCurrentScale = ((Module[0].EndScale - Module[0].StartScale) * Particle.NormalizedAge + Module[0].StartScale) * Particle.vWorldInitScale;
        
        
        if (DragModule)
        {
            if (Particle.NormalizedAge < Module[0].DestNormalizedAge)
            {
                // 최소 허용 분모 값 설정
                float minDenominator = 1e-6; // 매우 작은 값, 분모가 너무 작아지는 것을 방지
                float denominator = max(Module[0].DestNormalizedAge - Particle.NormalizedAge, minDenominator);

                // 기울기 계산
                float Gradient = (Module[0].TargetSpeed - length(Particle.vVelocity)) / denominator;
                float NADT = g_EngineDT / Particle.Life;

                // 새로운 속도를 계산
                float NewSpeed = length(Particle.vVelocity) + Gradient * NADT;

                // TargetSpeed를 넘어가는 것을 방지
                if ((Module[0].TargetSpeed - length(Particle.vVelocity)) * (Module[0].TargetSpeed - NewSpeed) < 0.0f)
                {
                    // 속도가 목표 속도를 넘어가면 목표 속도로 고정
                    NewSpeed = Module[0].TargetSpeed;
                }
                    
                // 속도 벡터 업데이트            
                Particle.vVelocity = normalize(Particle.vVelocity) * NewSpeed;
            }
            else
            {
                // NormalizedAge >= DestNormalizedAge인 경우, 속도를 바로 TargetSpeed로 고정
                Particle.vVelocity = normalize(Particle.vVelocity) * Module[0].TargetSpeed;
            }
        }
        
        if (Render)
        {
            Particle.vColor.rgb = (Module[0].EndColor - Module[0].vSpawnColor.rgb) * Particle.NormalizedAge + Module[0].vSpawnColor.rgb;

            if (Module[0].FadeOut)
            {
                float fRatio = saturate(1.f - (Particle.NormalizedAge - Module[0].StartRatio) / (1.f - Module[0].StartRatio));
                Particle.vColor.a = fRatio;
            }
        }
        
        
        Particle.Age += g_EngineDT;
        Particle.NormalizedAge = Particle.Age / Particle.Life;
        if (Particle.Life <= Particle.Age)
        {
            Particle.Active = 0;
        }
    }
}

//[numthreads(1024, 1, 1)]
//void CS_ParticleTick(int3 ThreadID : SV_DispatchThreadID)
//{
//    // 최대 파티클 개수보다 큰 ID는 작업을 수행하지 않음
//    if (MAX_COUNT <= ThreadID.x)
//        return;

//    // 파티클이 활성화되지 않은 경우
//    if (!Particle.Active)
//    {
//        int currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;

//        while (currentSpawnCount > 0)
//        {
//            int originalCount;

//            // SpawnCountBuffer[0].iSpawnCount 값을 원자적으로 1 감소시키고, 이전 값을 originalCount에 저장
//            InterlockedAdd(SpawnCountBuffer[0].iSpawnCount, -1, originalCount);

//            // originalCount가 0보다 큰 경우 파티클을 활성화하고 루프 종료
//            if (originalCount > 0)
//            {
//                Particle.Active = true;
                
//                float2 vUV = (float2) 0.f;
                
//                // 스레드를 UV 로 맵핑하기위해서 ID 를 0~1 범위로 정규화
//                vUV.x = (ThreadID.x / (MAX_COUNT - 1)) + g_EngineTime;
//                vUV.y = sin(vUV.x);
                
//                break;
//            }

//            // 다른 스레드가 값을 변경했을 수 있으므로 현재 스폰 카운트를 업데이트
//            currentSpawnCount = SpawnCountBuffer[0].iSpawnCount;
//        }
//    }
//    else
//    {
//        // 파티클이 활성화된 경우 위치 업데이트
//        Particle.vWorldPos += Particle.vVelocity * g_EngineDT;
//    }
//}



// 1. 파티클 렌더링, 비활성화 파티클 처리 + 빌보드 (완)

// 2. 파티클 스레드 동기화

// 3. 파티클 CS 에서 랜덤.



#endif