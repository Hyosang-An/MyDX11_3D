#include "pch.h"
#include "CGraphicShader.h"

#include "CDevice.h"
#include "CPathMgr.h"

CGraphicShader::CGraphicShader()
	: CShader(ASSET_TYPE::GRAPHIC_SHADER)
	, m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_RSType{RS_TYPE::CULL_BACK}
	, m_DSType(DS_TYPE::LESS)
	, m_BSType(BS_TYPE::DEFAULT)
{
	SetEngineAsset();
}

CGraphicShader::~CGraphicShader()
{
}

int CGraphicShader::CreateVertexShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _RelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "vs_5_0", D3DCOMPILE_DEBUG, 0
		, m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		if (nullptr != m_ErrBlob)
		{
			MessageBoxA(nullptr, (char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		}
		else
		{
			errno_t err = GetLastError();
			wchar_t szErrMsg[255] = {};
			swprintf_s(szErrMsg, 255, L"Error Code : %d", err);
			MessageBox(nullptr, szErrMsg, L"쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf());

	// Layout 생성
	D3D11_INPUT_ELEMENT_DESC Element[3] = {};

	Element[0].AlignedByteOffset = 0;
	Element[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	Element[0].InputSlot = 0;
	Element[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Element[0].InstanceDataStepRate = 0;
	Element[0].SemanticName = "POSITION";
	Element[0].SemanticIndex = 0;

	Element[1].AlignedByteOffset = 12;
	Element[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Element[1].InputSlot = 0;
	Element[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Element[1].InstanceDataStepRate = 0;
	Element[1].SemanticName = "COLOR";
	Element[1].SemanticIndex = 0;

	Element[2].AlignedByteOffset = 28;
	Element[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	Element[2].InputSlot = 0;
	Element[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Element[2].InstanceDataStepRate = 0;
	Element[2].SemanticName = "TEXCOORD";
	Element[2].SemanticIndex = 0;

	DEVICE->CreateInputLayout(Element, 3
		, m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf());

	return S_OK;
}

int CGraphicShader::CreateGeometryShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _RelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "gs_5_0", D3DCOMPILE_DEBUG, 0
		, m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		if (nullptr != m_ErrBlob)
		{
			MessageBoxA(nullptr, (char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		}
		else
		{
			errno_t err = GetLastError();
			wchar_t szErrMsg[255] = {};
			swprintf_s(szErrMsg, 255, L"Error Code : %d", err);
			MessageBox(nullptr, szErrMsg, L"쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer()
		, m_GSBlob->GetBufferSize(), nullptr, m_GS.GetAddressOf());

	return S_OK;
}

int CGraphicShader::CreatePixelShader(const wstring& _RelativePath, const string& _FuncName)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _RelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _FuncName.c_str(), "ps_5_0", D3DCOMPILE_DEBUG, 0
		, m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		if (nullptr != m_ErrBlob)
		{
			MessageBoxA(nullptr, (char*)m_ErrBlob->GetBufferPointer(), "쉐이더 컴파일 실패", MB_OK);
		}
		else
		{
			errno_t err = GetLastError();
			wchar_t szErrMsg[255] = {};
			swprintf_s(szErrMsg, 255, L"Error Code : %d", err);
			MessageBox(nullptr, szErrMsg, L"쉐이더 컴파일 실패", MB_OK);
		}

		return E_FAIL;
	}

	DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
		, m_PSBlob->GetBufferSize(), nullptr, m_PS.GetAddressOf());

	return S_OK;
}

void CGraphicShader::Binding()
{
	CONTEXT->IASetPrimitiveTopology(m_Topology);
	CONTEXT->IASetInputLayout(m_Layout.Get());

	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);
	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	CONTEXT->RSSetState(CDevice::GetInst()->GetRSState(m_RSType));
	CONTEXT->OMSetDepthStencilState(CDevice::GetInst()->GetDSState(m_DSType), 0);
	CONTEXT->OMSetBlendState(CDevice::GetInst()->GetBSState(m_BSType), nullptr, 0xffffffff);
}
