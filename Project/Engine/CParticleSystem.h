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

    Vec3	vForce;				// Particle �� ������ ��
    Vec3	vVelocity;			// Particle �� �ӵ�

    float	NoiseForceAccTime;	// Noise Force �ҿ� �����ϴ� ���� �ð�
    Vec3	NoiseForceDir;		// Noise Force �� ����

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
	UINT	SpawnRate;				// �ʴ� ��ƼŬ �������� (Spawn Per Second)
	Vec4	vSpawnColor;			// ���� ���� ����
	Vec4	vSpawnMinScale;			// ���� �� �ּ� ũ��
	Vec4	vSpawnMaxScale;			// ���� �� �ִ� ũ��

	float	MinLife;				// �ּ� ����
	float	MaxLife;				// �ִ� ����

	UINT	SpawnShape;				// ���� ���� ���� 0 : Box,  1: Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeScale.x == Radius

	UINT	BlockSpawnShape;		// �������� ���� ���� 0 : Box,  1: Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeScale.x == Radius

	UINT	SpaceType;				// ��ƼŬ ���� ��ġ ���� 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// �ѹ��� �߻���Ű�� Particle ��
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
	float	NoiseForceTerm;		// Noise Force �����Ű�� ��
	float	NoiseForceScale;	// Noise Force ũ��

	// Render Module
	Vec3	EndColor;			// ���� ����
	UINT	FadeOut;			// 0 : Off, 1 : Normalized Age
	float	FadeOutStartRatio;	// FadeOut ȿ���� ���۵Ǵ� Normalized Age ����
	UINT	VelocityAlignment;	// �ӵ� ���� 0 : Off, 1 : On


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
    CStructuredBuffer*      m_ParticleBuffer = nullptr;		// ��� ��ƼŬ ����
    CStructuredBuffer*      m_SpawnCountBuffer = nullptr;	// ��ƼŬ Ȱ��ȭ ���� ���޿� ����
	CStructuredBuffer*		m_ModuleBuffer = nullptr;					// Module Data Buffer

    Ptr<CTexture>           m_ParticleTex;
    float                   m_Time = 0;                 // �����ð�
	float					m_prevSpawnCountFraction = 0.f;	// ���������ӿ��� 1�����Ӵ� �����Ǿ���ϴ� ��ƼŬ ������ �Ҽ��κ�
	float                   m_BurstTime = 0;				// SpawnBurst üũ�� ����

    int                     m_MaxParticleCount = 1000;	// ��ƼŬ �ִ� ����

	tParticleModule         m_Module;               // ��ƼŬ�� ��� ����

public:
	void SetParticleTexture(Ptr<CTexture> _Texture);
	void CaculateSpawnCount();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;   // ���Ͽ� ������Ʈ�� ������ ����
    virtual void LoadFromFile(FILE* _File) override; // ���Ͽ� ������Ʈ�� ������ �ε�

public:
    CParticleSystem();
	CParticleSystem(const CParticleSystem& _Other);
    ~CParticleSystem();
    virtual CParticleSystem* Clone() { return new CParticleSystem(*this); }
};

