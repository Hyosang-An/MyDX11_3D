#pragma once

class CCamera;
class CGameObject;
class CLight2D;
class CLight3D;
class CStructuredBuffer;
class CMRT;

class CRenderMgr :
	public CSingleton<CRenderMgr>
{
private:
	friend class CSingleton<CRenderMgr>;
	CRenderMgr();
	~CRenderMgr();

private:
	vector<CCamera*>		m_vecCam;
	CCamera*				m_EditorCamera = nullptr;

	CGameObject*			m_DebugObject = nullptr;
	list<tDebugShapeInfo>   m_DebugShapeList;

	// Light
	vector<CLight2D*>       m_vecLight2D;
	CStructuredBuffer*		m_Light2DBuffer = nullptr;

	vector<CLight3D*>       m_vecLight3D;
	CStructuredBuffer*		m_Light3DBuffer = nullptr;

	// PostProcess
	vector<Ptr<CTexture>>   m_vecPostProcessRTTex;
	//Ptr<CTexture>			m_PostProcessDSTex;

	// MRT
	CMRT* m_arrMRT[(UINT)MRT_TYPE::END] = {};

	// ∫¥«’ Ω¶¿Ã¥ı
	Ptr<CMesh>              m_RectMesh;
	Ptr<CMaterial>          m_MergeMtrl;

public:
	void RegisterCamera(CCamera* _cam, int _camPriority);
	void RegisterEditorCamera(CCamera* _Cam) { m_EditorCamera = _Cam; }
	void AddDebugShapeInfo(const tDebugShapeInfo& _Info) { m_DebugShapeList.push_back(_Info); }
	void RegisterLight2D(CLight2D* _light);
	int RegisterLight3D(CLight3D* _Light) { m_vecLight3D.push_back(_Light); return m_vecLight3D.size() - 1; }

	void PostProcessCopy(int _postProcessRTTex_index);

	CCamera* GetPOVCam();

	CMRT* GetMRT(MRT_TYPE _Type) { return m_arrMRT[(UINT)_Type]; }

private:
	void SetPostProcessTex();
	void RenderStart();
	void Render(CCamera* _Cam);
	void Render_Sub(CCamera* _Cam);
	void Clear();

	void RenderDebugShape();

	void CreateMRT();
	void ClearMRT();
	void CreateMaterial();

public:
	void Init();
	void Tick();
};

