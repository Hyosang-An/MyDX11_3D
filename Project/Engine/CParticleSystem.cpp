#include "pch.h"
#include "CParticleSystem.h"

#include "CTimeMgr.h"

#include "CDevice.h"
#include "CAssetMgr.h"
#include "CStructuredBuffer.h"

#include "CTransform.h"

//#include <algorithm> // std::shuffle
#include <random>    // std::default_random_engine
#include <chrono>    // std::chrono::system_clock

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	// Mesh / Material 
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"ParticleRenderMtrl"));

	// ParticleTick ComputeShader
	m_TickCS = (CParticleTickCS*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"ParticleTickCS").Get();


	// Test용 Texture
	SetParticleTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\particle\\TX_GlowScene_2.png"));

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SB_TYPE::SRV_UAV);

	m_SpawnCountBuffer = new CStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::SRV_UAV);

	// 파티클 기능(모듈) 정보 세팅
	// Spawn Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SPAWN] = true;
	m_Module.SpawnRate = 20;
	m_Module.vSpawnColor = Vec4(0.24f, 0.67f, 0.87f, 1.f);
	m_Module.MinLife = 1.f;
	m_Module.MaxLife = 5.f;
	m_Module.vSpawnMinScale = Vec3(50.f, 5.f, 1.f);
	m_Module.vSpawnMaxScale = Vec3(500.f, 50.f, 1.f);

	m_Module.SpawnShape = 1;
	m_Module.SpawnShapeScale.x = 500.f;

	m_Module.BlockSpawnShape = 1;
	m_Module.BlockSpawnShapeScale.x = 0.f;

	m_Module.SpaceType = 1; // Local Space 

	// Spawn Burst Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SPAWN_BURST] = true;
	m_Module.SpawnBurstRepeat = true;
	m_Module.SpawnBurstCount = 100;
	m_Module.SpawnBurstRepeatTime = 3.f;

	// Scale Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SCALE] = true;
	m_Module.StartScale = 1.f;
	m_Module.EndScale = 1.f;

	// AddVelocity Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	m_Module.AddVelocityType = 1;
	m_Module.AddVelocityFixedDir = Vec3(0.f, 1.f, 0.f);
	m_Module.AddMinSpeed = 100.f;
	m_Module.AddMaxSpeed = 500.f;

	// Drag Module (감속)
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::DRAG] = false;
	m_Module.DestNormalizedAge = 1.f;
	m_Module.TargetSpeed = 0.f;

	// Noise Force Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	m_Module.NoiseForceTerm = 0.3f;
	m_Module.NoiseForceScale = 100.f;

	// Render Module
	m_Module.isModuleOn[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_Module.EndColor = Vec3(1.f, 0.f, 0.f);
	m_Module.FadeOut = true;
	m_Module.FadeOutStartRatio = 0.9f;
	m_Module.VelocityAlignment = true;

	m_ModuleBuffer = new CStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::SRV_UAV, &m_Module);

	//// 현재 시간으로 시드 설정
	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	//// 랜덤 엔진 생성
	//std::default_random_engine engine(seed);

	//// 벡터 섞기
	//std::shuffle(vecParticle.begin(), vecParticle.end(), engine);

}

CParticleSystem::CParticleSystem(const CParticleSystem& _Other)
	: CRenderComponent(_Other)
	, m_TickCS(_Other.m_TickCS)
	, m_ParticleBuffer(nullptr)
	, m_SpawnCountBuffer(nullptr)
	, m_ModuleBuffer(nullptr)
	, m_ParticleTex(_Other.m_ParticleTex)
	, m_Time(0.f)
	, m_prevSpawnCountFraction(0.f)
	, m_BurstTime(0.f)
	, m_MaxParticleCount(_Other.m_MaxParticleCount)
	, m_Module(_Other.m_Module)
{
	assert(_Other.m_ParticleBuffer && _Other.m_SpawnCountBuffer && _Other.m_ModuleBuffer);

	m_ParticleBuffer = new CStructuredBuffer(*_Other.m_ParticleBuffer);
	m_SpawnCountBuffer = new CStructuredBuffer(*_Other.m_SpawnCountBuffer);
	m_ModuleBuffer = new CStructuredBuffer(*_Other.m_ModuleBuffer);
}

CParticleSystem::~CParticleSystem()
{
	DELETE_PTR(m_ParticleBuffer);
	DELETE_PTR(m_SpawnCountBuffer);
	DELETE_PTR(m_ModuleBuffer);
}

void CParticleSystem::FinalTick()
{
	// SpawnCount 계산
	CaculateSpawnCount();

	// ComputeShader
	m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetSpawnCountBuffer(m_SpawnCountBuffer);
	m_TickCS->SetModuleBuffer(m_ModuleBuffer);

	m_TickCS->Execute();
}

void CParticleSystem::Render()
{
	// 위치정보 바인딩
	Transform()->Binding();

	// 파티클 버퍼 바인딩
	m_ParticleBuffer->Binding(20);	// t20

	// 모듈 버퍼 바인딩
	m_ModuleBuffer->Binding(21);	// t21

	// 재질정보 바인딩
	GetMaterial()->Binding();

	// 렌더링
	GetMesh()->Render_Particle(m_MaxParticleCount);

	// 파티클 버퍼 바인딩 해제
	m_ParticleBuffer->Clear(20);	// t20
	m_ModuleBuffer->Clear(21);		// t21
}

void CParticleSystem::SetParticleTexture(Ptr<CTexture> _Texture)
{
	m_ParticleTex = _Texture;
	GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
}

void CParticleSystem::CaculateSpawnCount()
{
	m_Time += EngineDT;
	tSpawnCount count = {  };

	if (m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SPAWN])
	{
		//// SpawnRate 에 따른 이번 Tick 에서 생성시킬 파티클의 수
		//float Term = 1.f / (float)m_Module.SpawnRate;
		//UINT SpawnCount = 0;

		//if (Term < m_Time)
		//{
		//	float Value = m_Time / Term;
		//	SpawnCount = (UINT)Value;
		//	m_Time -= (float)SpawnCount * Term;
		//}

		//count.SpawnCount = SpawnCount;



		float SpawnCount = m_Module.SpawnRate * EngineDT + m_prevSpawnCountFraction;
		count.SpawnCount = floor(SpawnCount);
		m_prevSpawnCountFraction = SpawnCount - count.SpawnCount;
	}

	if (m_Module.isModuleOn[(UINT)PARTICLE_MODULE::SPAWN_BURST])
	{
		UINT BurstCount = 0;

		if (0.f == m_BurstTime)
		{
			BurstCount = m_Module.SpawnBurstCount;
		}

		m_BurstTime += EngineDT;

		if (m_Module.SpawnBurstRepeat && m_Module.SpawnBurstRepeatTime <= m_BurstTime)
		{
			m_BurstTime -= m_Module.SpawnBurstRepeatTime;
			BurstCount += m_Module.SpawnBurstCount;
		}

		count.SpawnCount += BurstCount;
	}

	// SpawnCount 를 Buffer 에 전달	
	m_SpawnCountBuffer->SetData(&count);
}

void CParticleSystem::SaveToFile(FILE* _File)
{

}

void CParticleSystem::LoadFromFile(FILE* _File)
{

}