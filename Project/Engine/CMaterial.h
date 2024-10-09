#pragma once
#include "CAsset.h"

class CMaterial :
    public CAsset
{
public:
    CMaterial(bool _IsEngine = false);
    ~CMaterial();
    virtual CMaterial* Clone() { return new CMaterial(*this); } // Material은 기본 복사생성자 써도 됨

private:
    Ptr<CGraphicShader>     m_Shader;

    tMtrlConst              m_Const;
    Ptr<CTexture>           m_arrTex[TEX_PARAM::END];

public:
    template<typename T>
    void SetScalarParam(SCALAR_PARAM _param, const T& _data);
    void SetTexParam(TEX_PARAM _Param, Ptr<CTexture> _Tex) { m_arrTex[_Param] = _Tex; }
    void SetShader(Ptr<CGraphicShader> _Shader) { m_Shader = _Shader; }

    void* GetScalarParam(SCALAR_PARAM _Param);
    Ptr<CTexture> GetTexParam(TEX_PARAM _Param) { return m_arrTex[(UINT)_Param]; }


    Ptr<CGraphicShader> GetShader() { return m_Shader; }

public:
    void Binding();

public:
    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;
};

template<typename T>
inline void CMaterial::SetScalarParam(SCALAR_PARAM _param, const T& _data)
{
    if constexpr (std::is_same_v<T, int>) { m_Const.iArr[_param] = _data; }
    if constexpr (std::is_same_v<T, float>) { m_Const.fArr[_param - FLOAT_0] = _data; }
    if constexpr (std::is_same_v<T, Vec2>) { m_Const.v2Arr[_param - VEC2_0] = _data; }
    if constexpr (std::is_same_v<T, Vec4>) { m_Const.v4Arr[_param - VEC4_0] = _data; }
    if constexpr (std::is_same_v<T, Matrix>) { m_Const.matArr[_param - MAT_0] = _data; }
}
