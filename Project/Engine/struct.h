#pragma once

struct Vtx
{
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;

	Vec3	vTangent;	// ����
	Vec3	vNormal;	// ����
	Vec3	vBinormal;  // ������

	Vec4	vWeights;  // Bone ����ġ
	Vec4	vIndices;  // Bone �ε���
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

struct tRay
{
	Vec3	vStart;
	Vec3	vDir;
};

// ============
// Animation 3D
// ============
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};


struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Inverse ���( Skin ������ -> �⺻���·� �ǵ���)
	Matrix				matBone;
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring			strAnimName;
	int				iStartFrame;
	int				iEndFrame;
	int				iFrameLength;

	double			dStartTime;
	double			dEndTime;
	double			dTimeLength;
	float			fUpdateTime; // �̰� �Ⱦ�

	FbxTime::EMode	eMode;
};

// ===================
// ������� ���� ����ü
// ===================
struct tTransform
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;

	Matrix matWorldInv;
	Matrix matViewInv;
	Matrix matProjInv;

	Matrix	matWV;
	Matrix	matWVP;

	Vec3	worldDir[3];

	int		Padding[3];
};

extern tTransform g_Trans;

// Material ���
struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};


struct tMtrlConst
{
	tMtrlData	mtrl;
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	matArr[4];

	// Texture�� �ִ��� ������ (0 : ����, �� �� : ����)
	int		btex[TEX_PARAM::END]; // 10��¥�� �迭

	// 3D Animation ����
	int		arrAnimData[2];
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