#pragma once

struct Vtx
{
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;

	Vec3	vTangent;	// 접선
	Vec3	vNormal;	// 법선
	Vec3	vBinormal;  // 종법선

	Vec4	vWeights;  // Bone 가중치
	Vec4	vIndices;  // Bone 인덱스
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
	Vec4	Color;		// 빛의 색상
	Vec4	Ambient;	// 환경광 (빛을 받고있지 않은 상태에서도 보이는 사물의 색)
	float	SpecularCoef;	// 반사광의 세기 계수
};

struct tLightInfo
{
	tLight		light;		// 광원 색상정보
	Vec3		WorldPos;	// 광원 위치
	Vec3		WorldDir;	// 광윈이 진행하는 방향
	float		Radius;		// 광원의 반경
	float		Angle;		// 광원 범위 각도
	LIGHT_TYPE	Type;		// 광원 종류
	int			padding[2]; // 패딩
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
	Matrix				matOffset;	// Inverse 행렬( Skin 정점을 -> 기본상태로 되돌림)
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
	float			fUpdateTime; // 이거 안씀

	FbxTime::EMode	eMode;
};

// ===================
// 상수버퍼 연동 구조체
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

// Material 계수
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

	// Texture가 있는지 없는지 (0 : 없음, 그 외 : 있음)
	int		btex[TEX_PARAM::END]; // 10개짜리 배열

	// 3D Animation 정보
	int		arrAnimData[2];
};

struct tSpriteInfo
{
	Vec2 LeftTopInAtlasUV;
	Vec2 SliceSizeInAtlasUV;
	Vec2 BackGroundSizeInAtlasUV;
	Vec2 OffsetUV;
	int	 UseFlipbook;
	int  Padding[3];	// 16바이트 단위 맞추기 위해
};

struct tGlobalData
{
	// 시간 관련 정보
	float   g_DT;
	float   g_EngineDT;
	float   g_Time;
	float   g_EngineTime;

	// 카메라 월드 위치
	Vec4	g_CamWorldPos;

	// 렌더타겟의 해상도 정보
	Vec2	g_Resolution;

	// 바인딩 된 구조화버퍼에 광원이 몇개 들어있는지
	int     g_Light2DCount;
	int     g_Light3DCount;
};
extern tGlobalData g_GlobalData;