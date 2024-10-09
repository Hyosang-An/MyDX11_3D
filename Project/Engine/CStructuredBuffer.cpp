#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_Desc{}
	, m_ElementCount(0)
	, m_ElementSize(0)
{
}

CStructuredBuffer::~CStructuredBuffer()
{
}


int CStructuredBuffer::Create(UINT _ElementSize, UINT _ElementCount,
	SB_TYPE _Type, void* _InitData)
{
	// _Type 기본값: SRV_ONLY
	// _SysMemMove 기본값: false
	// _InitData 기본값: nullptr

	assert(0 == (_ElementSize % 16));	// 구조화 버퍼의 단위는 16바이트이어야 한다.

	m_Type = _Type;

	// CreateBuffer 또는 CreateShaderResourceView를 할 때, 이미 m_SB나 m_SRV가 존재한다면 이를 nullptr로 가리키게 하여 참조 카운트를 줄여줘야한다.
	m_SB = nullptr;
	m_SB_Write = nullptr;
	m_SB_Read = nullptr;

	m_SRV = nullptr;

	m_ElementSize = _ElementSize;
	m_ElementCount = _ElementCount;

	// ID3D11Buffer m_SB를 메인 StructuredBuffer 용도로 생성하기
	if (SB_TYPE::SRV_UAV == _Type)
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	else
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	m_Desc.ByteWidth = m_ElementSize * m_ElementCount;
	m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_Desc.StructureByteStride = m_ElementSize;


	if (SB_TYPE::SRV_UAV == _Type)
	{
		m_Desc.Usage = D3D11_USAGE_DEFAULT;
		m_Desc.CPUAccessFlags = 0;
	}
	else if (SB_TYPE::SRV_ONLY == _Type)
	{
		m_Desc.Usage = D3D11_USAGE_DYNAMIC;
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	HRESULT hr = S_OK;
	if (nullptr == _InitData)
	{
		hr = DEVICE->CreateBuffer(&m_Desc, nullptr, m_SB.GetAddressOf());
	}

	else
	{
		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = _InitData;
		hr = DEVICE->CreateBuffer(&m_Desc, &sub, m_SB.GetAddressOf());
	}

	if (FAILED(hr))
		return E_FAIL;


	// 추가버퍼 생성
	if (m_Type == SB_TYPE::SRV_UAV)
	{
		D3D11_BUFFER_DESC tRWBufferDesc = m_Desc;

		tRWBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tRWBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		tRWBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (nullptr == _InitData)
		{
			hr = DEVICE->CreateBuffer(&tRWBufferDesc, nullptr, m_SB_Write.GetAddressOf());
		}

		else
		{
			D3D11_SUBRESOURCE_DATA sub = {};
			sub.pSysMem = _InitData;
			hr = DEVICE->CreateBuffer(&tRWBufferDesc, &sub, m_SB_Write.GetAddressOf());
		}

		tRWBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		tRWBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (nullptr == _InitData)
		{
			hr = DEVICE->CreateBuffer(&tRWBufferDesc, nullptr, m_SB_Read.GetAddressOf());
		}

		else
		{
			D3D11_SUBRESOURCE_DATA sub = {};
			sub.pSysMem = _InitData;
			hr = DEVICE->CreateBuffer(&tRWBufferDesc, &sub, m_SB_Read.GetAddressOf());
		}
	}

	// ShaderResourceView 생성하기
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
	SRVDesc.BufferEx.NumElements = m_ElementCount;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &SRVDesc, m_SRV.GetAddressOf())))
	{
		return E_FAIL;
	}

	// UAV 생성
	if (_Type == SB_TYPE::SRV_UAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC Desc = {};

		Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Desc.Buffer.NumElements = m_ElementCount;
		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &Desc, m_UAV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}


void CStructuredBuffer::SetData(void* _pData, UINT _DataSize)
{
	// 입력데이터가 구조화버퍼 크기보다 작아야한다.
	assert(_DataSize <= m_Desc.ByteWidth);

	if (0 == _DataSize)
	{
		_DataSize = m_Desc.ByteWidth;
	}

	// 메인 버퍼만 사용하는 경우
	if (m_Type == SRV_ONLY)
	{
		D3D11_MAPPED_SUBRESOURCE tMapSub = {};
		CONTEXT->Map(m_SB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
		memcpy(tMapSub.pData, _pData, _DataSize);
		CONTEXT->Unmap(m_SB.Get(), 0);
	}

	// 읽기, 쓰기 버퍼를 따로 쓰는 경우
	else if (m_Type == SRV_UAV)
	{
		D3D11_MAPPED_SUBRESOURCE tMapSub = {};
		CONTEXT->Map(m_SB_Write.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
		memcpy(tMapSub.pData, _pData, _DataSize);
		CONTEXT->Unmap(m_SB_Write.Get(), 0);

		CONTEXT->CopyResource(m_SB.Get(), m_SB_Write.Get());
	}
}

void CStructuredBuffer::GetData(void* _pData, UINT _DataSize)
{
	// 요청한 데이터 크기가 구조화버퍼 크기보다 작아야한다.
	assert(_DataSize <= m_Desc.ByteWidth);
	assert(m_Type == SRV_ONLY);

	if (0 == _DataSize)
	{
		_DataSize = m_Desc.ByteWidth;
	}

	CONTEXT->CopyResource(m_SB_Read.Get(), m_SB.Get());

	D3D11_MAPPED_SUBRESOURCE tMapSub = {};
	CONTEXT->Map(m_SB_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tMapSub);
	memcpy(_pData, tMapSub.pData, _DataSize);
	CONTEXT->Unmap(m_SB_Read.Get(), 0);
}

void CStructuredBuffer::Binding(UINT _RegisterNum)
{
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::Clear(UINT _RegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, &pSRV);
}

void CStructuredBuffer::Binding_CS_SRV(UINT _RegisterNum)
{
	m_RecentRegisterNum = _RegisterNum;
	CONTEXT->CSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::Clear_CS_SRV()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->CSSetShaderResources(m_RecentRegisterNum, 1, &pSRV);
}

void CStructuredBuffer::Binding_CS_UAV(UINT _RegisterNum)
{
	m_RecentRegisterNum = _RegisterNum;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_RegisterNum, 1, m_UAV.GetAddressOf(), &i);
}

void CStructuredBuffer::Clear_CS_UAV()
{
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = 0;
	CONTEXT->CSSetUnorderedAccessViews(m_RecentRegisterNum, 1, &pUAV, &i);
}
