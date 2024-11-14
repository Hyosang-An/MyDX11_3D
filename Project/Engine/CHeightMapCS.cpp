#include "pch.h"
#include "CHeightMapCS.h"

#include "CStructuredBuffer.h"

CHeightMapCS::CHeightMapCS()
    : CComputeShader(32, 32, 1, L"shader\\heightmap.fx", "CS_HeightMap")
{
}

CHeightMapCS::~CHeightMapCS()
{

}

int CHeightMapCS::Binding()
{
 //   if (nullptr == m_HeightMapTex)
 //       return E_FAIL;

	//m_HeightMapTex->Binding_CS_UAV(0);

 //   CComputeShader::m_Const.iArr[0] = m_HeightMapTex->Width();
	//CComputeShader::m_Const.iArr[1] = m_HeightMapTex->Height();

 //   CComputeShader::m_Const.v2Arr[0] = m_BrushScale;
 //   CComputeShader::m_Const.v2Arr[1] = m_BrushPos;

 //   if (nullptr != m_BrushTex)
 //   {
 //       m_BrushTex->Binding_CS_SRV(0);
 //       m_Const.btex[0] = 1;
 //   }
 //   else
 //       m_Const.btex[0] = 0;

 //   return S_OK;

    if (nullptr == m_HeightMapTex)
        return E_FAIL;

    m_HeightMapTex->Binding_CS_UAV(0);

    m_Const.iArr[0] = (UINT)m_HeightMapTex->Width();
    m_Const.iArr[1] = (UINT)m_HeightMapTex->Height();

    m_Const.v2Arr[0] = m_BrushScale;
    m_Const.v2Arr[1] = m_BrushPos;

    if (nullptr != m_BrushTex)
    {
        m_BrushTex->Binding_CS_SRV(0);
        m_Const.btex[0] = 1;
    }
    else
        m_Const.btex[0] = 0;

    return S_OK;
}

void CHeightMapCS::CalcGroupNum()
{
	// 해상도를 32 x 32로 나누어 그룹 수 계산
    m_GroupX = (UINT)m_HeightMapTex->Width() / m_ThreadPerGroupX;
    m_GroupY = (UINT)m_HeightMapTex->Height() / m_ThreadPerGroupY;
    m_GroupZ = 1;

    if (0 < (UINT)m_HeightMapTex->Width() % m_ThreadPerGroupX)
        m_GroupX += 1;

    if (0 < (UINT)m_HeightMapTex->Height() % m_ThreadPerGroupY)
        m_GroupY += 1;
}

void CHeightMapCS::Clear()
{
    m_HeightMapTex->Clear_CS_UAV();
    m_HeightMapTex = nullptr;

    if (nullptr != m_BrushTex)
    {
        m_BrushTex->Clear_CS_SRV();
        m_BrushTex = nullptr;
    }
}
