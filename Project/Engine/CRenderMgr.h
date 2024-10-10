#pragma once

class CCamera;
class CGameObject;
class CLight2D;
class CLight3D;
class CStructuredBuffer;

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
	Ptr<CTexture>			m_PostProcessDSTex;

public:
	void RegisterCamera(CCamera* _cam, int _camPriority);
	void RegisterEditorCamera(CCamera* _Cam) { m_EditorCamera = _Cam; }
	void AddDebugShapeInfo(const tDebugShapeInfo& _Info) { m_DebugShapeList.push_back(_Info); }
	void RegisterLight2D(CLight2D* _light);
	void RegisterLight3D(CLight3D* _Light) { m_vecLight3D.push_back(_Light); }

	void PostProcessCopy(int _postProcessRTTex_index);

private:
	void SetPostProcessTex();
	void RenderStart();
	void Clear();

	void RenderDebugShape();

public:
	void Init();
	void Tick();
};

