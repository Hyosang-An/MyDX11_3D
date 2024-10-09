#pragma once
#include "CRenderComponent.h"

#include "CParticleTickCS.h"

class CStructuredBuffer;

// Particle
struct tParticle
{
    int		Active;

    Vec4	vColor;
	Vec3	vLocalPos;
	Vec3	vWorldPos;
    Vec3	vWorldInitScale;
    Vec3	vWorldCurrentScale;
    Vec3	vWorldRotation;

    Vec3	vForce;				// Particle 에 누적된 힘
    Vec3	vVelocity;			// Particle 의 속도

    float	NoiseForceAccTime;	// Noise Force 텀에 도달하는 누적 시간
    Vec3	NoiseForceDir;		// Noise Force 의 방향

    float	Mass;
    float	Life;
    float	Age;
    float	NormalizedAge;

    Vec2	Padding;
};

// Particle Module
struct tParticleModule
{
	// Spawn
	UINT	SpawnRate;				// 초당 파티클 생성개수 (Spawn Per Second)
	Vec4	vSpawnColor;			// 생성 시점 색상
	Vec4	vSpawnMinScale;			// 생성 시 최소 크기
	Vec4	vSpawnMaxScale;			// 생성 시 최대 크기

	float	MinLife;				// 최소 수명
	float	MaxLife;				// 최대 수명

	UINT	SpawnShape;				// 스폰 생성 구역 0 : Box,  1: Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeScale.x == Radius

	UINT	BlockSpawnShape;		// 스폰되지 않을 구역 0 : Box,  1: Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeScale.x == Radius

	UINT	SpaceType;				// 파티클 생성 위치 기준 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// 한번에 발생시키는 Particle 수
	UINT	SpawnBurstRepeat;
	float	SpawnBurstRepeatTime;

	// Add Velocity
	UINT	AddVelocityType;		// 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	Vec3	AddVelocityFixedDir;
	float	AddMinSpeed;
	float	AddMaxSpeed;

	// Scale Module
	float	StartScale;
	float	EndScale;

	// Drag Module
	float	DestNormalizedAge;
	float	TargetSpeed;

	// Noise Force Module
	float	NoiseForceTerm;		// Noise Force 적용시키는 텀
	float	NoiseForceScale;	// Noise Force 크기

	// Render Module
	Vec3	EndColor;			// 최종 색상
	UINT	FadeOut;			// 0 : Off, 1 : Normalized Age
	float	FadeOutStartRatio;	// FadeOut 효과가 시작되는 Normalized Age 지점
	UINT	VelocityAlignment;	// 속도 정렬 0 : Off, 1 : On


	// Module On / Off
	int		isModuleOn[(UINT)PARTICLE_MODULE::END];
};

struct tSpawnCount
{
    UINT    SpawnCount;
    UINT    padding[3];
};

class CParticleSystem :
    public CRenderComponent
{
private:
    Ptr<CParticleTickCS>    m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer = nullptr;		// 모든 파티클 정보
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr;	// 파티클 활성화 숫자 전달용 버퍼
	CStructuredBuffer*		m_ModuleBuffer = nullptr;					// Module Data Buffer

    Ptr<CTexture>           m_ParticleTex;
    float                   m_Time = 0;                 // 누적시간
	float					m_prevSpawnCountFraction = 0.f;	// 이전프레임에서 1프레임당 생성되어야하는 파티클 개수의 소수부분
	float                   m_BurstTime = 0;				// SpawnBurst 체크용 변수

    int                     m_MaxParticleCount = 1000;	// 파티클 최대 개수

	tParticleModule         m_Module;               // 파티클의 기능 정의

public:
	void SetParticleTexture(Ptr<CTexture> _Texture);
	void CaculateSpawnCount();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) override; // 파일에 컴포넌트의 정보를 로드

public:
    CParticleSystem();
	CParticleSystem(const CParticleSystem& _Other);
    ~CParticleSystem();
    virtual CParticleSystem* Clone() { return new CParticleSystem(*this); }
};

