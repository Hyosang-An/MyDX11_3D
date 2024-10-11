#pragma once

struct Vtx
{
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;

	Vec3	vTangent;	// ����
	Vec3	vNormal;	// ����
	Vec3	vBinormal;  // ������
};

struct tDebugShapeInfo
{
	DEBUG_SHAPE		Shape;
	Vec3			vPos;
	Vec3			vScale;
	Vec3			vRot;
	Matrix			matWorld;
	Vec4			vColor;
	float			LifeTime;
	float			Age;
	bool			DepthTest;
};

struct tTask
{
	TASK_TYPE	Type;
	DWORD_PTR	Param_0;
	DWORD_PTR	Param_1;
};

struct tLight
{
	Vec4	Color;		// ���� ����
	Vec4	Ambient;	// ȯ�汤 (���� �ް����� ���� ���¿����� ���̴� �繰�� ��)
	float	SpecularCoef;	// �ݻ籤�� ���� ���
};

struct tLightInfo
{
	tLight		light;		// ���� ��������
	Vec3		WorldPos;	// ���� ��ġ
	Vec3		WorldDir;	// ������ �����ϴ� ����
	float		Radius;		// ������ �ݰ�
	float		Angle;		// ���� ���� ����
	LIGHT_TYPE	Type;		// ���� ����
	int			padding[2]; // �е�
};

struct tPixel
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};

// ===================
// ������� ���� ����ü
// ===================
struct tTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;

	Matrix	matWV;
	Matrix	matWVP;

	Vec3	worldDir[3];

	int		Padding[3];
};

extern tTransform g_Trans;

struct tMtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	matArr[4];

	// Texture�� �ִ��� ������ (0 : ����, �� �� : ����)
	int		btex[TEX_PARAM::END + 2]; // 16����Ʈ ������ ������ �ϹǷ�.
};

struct tSpriteInfo
{
	Vec2 LeftTopInAtlasUV;
	Vec2 SliceSizeInAtlasUV;
	Vec2 BackGroundSizeInAtlasUV;
	Vec2 OffsetUV;
	int	 UseFlipbook;
	int  Padding[3];	// 16����Ʈ ���� ���߱� ����
};

struct tGlobalData
{
	// �ð� ���� ����
	float   g_DT;
	float   g_EngineDT;
	float   g_Time;
	float   g_EngineTime;

	// ī�޶� ���� ��ġ
	Vec4	g_CamWorldPos;

	// ����Ÿ���� �ػ� ����
	Vec2	g_Resolution;

	// ���ε� �� ����ȭ���ۿ� ������ � ����ִ���
	int     g_Light2DCount;
	int     g_Light3DCount;
};
extern tGlobalData g_GlobalData;