#pragma once
#include "CShader.h"

class CComputeShader :
    public CShader
{
private:
    ComPtr<ID3DBlob>            m_CSBlob;
    ComPtr<ID3D11ComputeShader> m_CS;

protected:
    // 1개의 그룹 당 스레드 수
    const UINT                  m_ThreadPerGroupX;
    const UINT                  m_ThreadPerGroupY;
    const UINT                  m_ThreadPerGroupZ;


    // 그룹 수
    UINT                        m_GroupX;
    UINT                        m_GroupY;
    UINT                        m_GroupZ;

    // 상수 데이터 전달 용도
    tMtrlConst                  m_Const;

public:
    int Execute();

private:
    int CreateComputeShader(const wstring& _RelativePath, const string& _FuncName);

    virtual int Binding() = 0;
    virtual void CalcGroupNum() = 0;
    virtual void Clear() = 0;



public:
    CComputeShader(UINT _ThreadPerGroupX, UINT _ThreadPerGroupY, UINT _ThreadPerGroupZ
        , const wstring& _ShaderRelativePath, const string& _FuncName);
    ~CComputeShader();
};

