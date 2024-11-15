#pragma once
#include "CComponent.h"

class CGameObject;

enum PROJ_TYPE
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
};

class CCamera :
    public CComponent
{
    friend class CRenderMgr;

public:
    CCamera();
    CCamera(const CCamera& _Other);
    ~CCamera();
    virtual CCamera* Clone() { return new CCamera(*this); }

private:
    int         m_Priority;
    UINT        m_LayerCheck;   // 원하는 레이어만 카메라에 찍히도록 설정

    PROJ_TYPE   m_ProjType;
    class CFrustum* m_Frustum = nullptr;

    float       m_Width;
    float       m_Height;
    float       m_AspectRatio;  // 종횡비
    float       m_Far;          // 카메라가 볼 수 있는 시야 거리

    float       m_FOV; // Field Of View (시야 범위, 시야 각)
    float       m_ProjectionScale = 1.f;

    tRay                    m_Ray;

	Matrix      m_matView;
	Matrix      m_matViewInv;
	Matrix      m_matProj;
	Matrix      m_matProjInv;

    vector<CGameObject*>    m_vecDeferred;      // Deferred
    vector<CGameObject*>    m_vecDecal;         // Decal

    vector<CGameObject*>    m_vecOpaque;        // 불투명
    vector<CGameObject*>    m_vecMasked;        // 불투명, 투명
    vector<CGameObject*>    m_vecTransparent;   // 투명, 반투명
    vector<CGameObject*>    m_vecParticles;     // 투명, 반투명, 입자 타입
	vector<CGameObject*>    m_vecEffect;		// 효과 오브젝트
    vector<CGameObject*>    m_vecPostProcess;   // 후처리 오브젝트
    vector<CGameObject*>    m_vecUI;   // UI 오브젝트

    vector<CGameObject*>    m_vecShadowMap;     // 광원 시점에서 렌더링할 물체들

public:
    void SetPriority(int _Priority) { m_Priority = _Priority; }
    void SetLayer(UINT _LayerIdx, bool _bCheck)
    {
        if (_bCheck)
            m_LayerCheck |= (1 << _LayerIdx);
        else
            m_LayerCheck &= ~(1 << _LayerIdx);
    }
    void SetLayerAll() { m_LayerCheck = 0xffffffff; }

    bool GetLayerCheck(UINT _LayerIdx) { return m_LayerCheck & (1 << _LayerIdx); }

    void SetProjType(PROJ_TYPE _type) { m_ProjType = _type; }
    PROJ_TYPE GetProjType() { return m_ProjType; }

    void SetWidth(float _Width)
    {
        m_Width = _Width;
        m_AspectRatio = m_Width / m_Height;
    }

    void SetHeight(float _Height)
    {
        m_Height = _Height;
        m_AspectRatio = m_Width / m_Height;
    }

    float GetWidth() { return m_Width; }
    float GetHeight() { return m_Height; }
    float GetAspectRatio() { return m_AspectRatio; }

    void SetFar(float _Far) { m_Far = _Far; }
    float GetFar() { return m_Far; }

    void SetFOV(float _FOV) { m_FOV = _FOV; }
    float GetFOV() { return m_FOV; }

    void SetScale(float _Scale) { m_ProjectionScale = _Scale; }
    float GetScale() { return m_ProjectionScale; }

    const tRay& GetRay() { return m_Ray; }

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetViewMatInv() { return m_matViewInv; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetProjMatInv() { return m_matProjInv; }


private:
    void CalcRay();

public:
    void SortGameObject();
    void render_deferred();
    void render_decal();

    void render_opaque();
    void render_masked();
    void render_transparent();
    void render_particle();
    void render_effect();
    void render_postprocess();
    void render_ui();

    void clear();

    void SortGameObject_ShadowMap();
    void render_shadowmap();


public:
    virtual void Begin() override;
    virtual void FinalTick() override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;


};

