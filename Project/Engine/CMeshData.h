#pragma once
#include "CAsset.h"

#include "CMesh.h"
#include "CMaterial.h"

class CMeshData :
    public CAsset
{
	Ptr<CMesh>				m_pMesh;
	vector<Ptr<CMaterial>>	m_vecMtrl;

public:
	static CMeshData* LoadFromFBX(const wstring& _RelativePath);

	class CGameObject* Instantiate();
	virtual int Save(const wstring& _FilePath) override;
	virtual int Load(const wstring& _strFilePath) override;

public:
	CLONE_DISABLE(CMeshData);
	CMeshData(bool _Engine = false);
	~CMeshData();
};

