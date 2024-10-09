#pragma once

#include "CPathMgr.h"
#include "CTaskMgr.h"

class CAsset;

class CAssetMgr
	: public CSingleton<CAssetMgr>
{
private:
	friend class CSingleton<CAssetMgr>;
	friend class CTaskMgr;

	CAssetMgr();
	~CAssetMgr();

private:
	map<wstring, Ptr<CAsset>>	m_arrAssetMap[(UINT)ASSET_TYPE::END];
	bool						m_Changed = false;

	int							m_PostProcessTextCnt = 4;

public:
	void Init();
	void Tick();

	void GetAssetNames(ASSET_TYPE _Type, vector<string>& _vecOut);
	const map<wstring, Ptr<CAsset>>& GetAssetMap(ASSET_TYPE _Type) { return m_arrAssetMap[(UINT)_Type]; }
	bool IsChanged() { return m_Changed; }
	int GetPostProcessTextCnt() { return m_PostProcessTextCnt; }

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

	Ptr<CAsset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);
	//void AddAsset(const wstring& _Key, CAsset* _Asset);

	template<typename T>
	Ptr<T> FindAsset(const wstring& _Key);

	template<typename T>
	void AddAsset(const wstring& _Key, Ptr<T> _Asset);

	// _Flags : D3D11_BIND_FLAG
	Ptr<CTexture> CreateTexture(wstring _strKey, UINT _Width, UINT _Height
		, DXGI_FORMAT _Format, UINT _Flags
		, D3D11_USAGE _Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT);

	Ptr<CTexture> CreateTexture(wstring _strKey, ComPtr<ID3D11Texture2D> _Tex2D);



private:
	void CreateEngineMesh();
	void CreateEngineTexture();
	void CreateEngineSprite();
	void CreateEngineGraphicShader();
	void CreateEngineComputeShader();
	void CreateEngineMaterial();

private:
	void DeleteAsset(ASSET_TYPE _Type, const wstring& _Key);

};

template<typename T>
Ptr<T> CAssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
	// 동일 키값 에셋이 있는지 확인
	Ptr<T> Asset = FindAsset<T>(_Key);

	if (nullptr != Asset)
	{
		return Asset;
	}

	// 동일 키값의 에셋이 없었으면
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _RelativePath;

	Asset = new T;

	// 로딩 실패 시 예외처리
	if (FAILED(Asset->Load(strFilePath)))
	{
		MessageBox(nullptr, L"애셋 로딩 실패", L"텍스쳐 로딩 실패", MB_OK);
		return nullptr;
	}

	// Asset 이 자신의 경로를 알게 함
	Asset->m_RelativePath = _RelativePath;

	// 맵에 등록
	AddAsset(_Key, Asset);

	// 로딩된 에셋 주소 반환
	return Asset;
}

template<typename T>
Ptr<T> CAssetMgr::FindAsset(const wstring& _Key)
{
	ASSET_TYPE Type = GetAssetType<T>();

	map<wstring, Ptr<CAsset>>::iterator iter = m_arrAssetMap[(UINT)Type].find(_Key);

	if (iter == m_arrAssetMap[(UINT)Type].end())
		return nullptr;

	//Ptr<CAsset> pAsset;
	//Ptr<CMesh> pMesh = (CMesh*)pAsset.Get()

	return (T*)iter->second.Get();
}

template<typename T>
void CAssetMgr::AddAsset(const wstring& _Key, Ptr<T> _Asset)
{
	ASSET_TYPE Type = GetAssetType<T>();

	assert(!FindAsset(Type, _Key).Get());

	_Asset->SetKey(_Key);
	m_arrAssetMap[(UINT)Type].insert(make_pair(_Key, _Asset.Get()));

	// Asset 변경 알림	
	CTaskMgr::GetInst()->AddTask(tTask{ ASSET_CHANGED });
}

// File 에 Asset 참조정보 저장 불러오기
template<typename T>
void SaveAssetRef(Ptr<T> Asset, FILE* _File)
{
	bool bAsset = Asset.Get();
	fwrite(&bAsset, 1, 1, _File);

	if (bAsset)
	{
		SaveWString(Asset->GetKey(), _File);
		SaveWString(Asset->GetRelativePath(), _File);
	}
}

template<typename T>
void LoadAssetRef(Ptr<T>& Asset, FILE* _File)
{
	bool bAsset = false;
	fread(&bAsset, 1, 1, _File);

	if (bAsset)
	{
		wstring key, relativepath;
		LoadWString(key, _File);
		LoadWString(relativepath, _File);

		Asset = CAssetMgr::GetInst()->Load<T>(key, relativepath);
	}
}