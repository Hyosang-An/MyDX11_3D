#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CAssetMgr.h"
#include "CPathMgr.h"

CMaterial::CMaterial(bool _IsEngine)
	: CAsset(ASSET_TYPE::MATERIAL)
{
	if (_IsEngine)
	{
		SetEngineAsset();
	}
}

CMaterial::~CMaterial()
{

}

void* CMaterial::GetScalarParam(SCALAR_PARAM _Param)
{
	switch (_Param)
	{
		case INT_0:
		case INT_1:
		case INT_2:
		case INT_3:
			return  m_Const.iArr + _Param;
			break;

		case FLOAT_0:
		case FLOAT_1:
		case FLOAT_2:
		case FLOAT_3:
			return  m_Const.fArr + (_Param - FLOAT_0);
			break;

		case VEC2_0:
		case VEC2_1:
		case VEC2_2:
		case VEC2_3:
			return  m_Const.v2Arr + (_Param - VEC2_0);
			break;

		case VEC4_0:
		case VEC4_1:
		case VEC4_2:
		case VEC4_3:
			return  m_Const.v4Arr + (_Param - VEC4_0);
			break;

		case MAT_0:
		case MAT_1:
		case MAT_2:
		case MAT_3:
			return  m_Const.matArr + (_Param - MAT_0);
			break;
	}

	return nullptr;
}

void CMaterial::Binding()
{
	if (!m_Shader)
		return;

	// Textrue 바인딩
	for (int i = 0; i < TEX_PARAM::END; ++i)
	{
		if (m_arrTex[i] == nullptr)
		{
			m_Const.btex[i] = 0;
			CTexture::Clear(i);
		}
		else
		{
			m_Const.btex[i] = 1;
			m_arrTex[i]->Binding(i);
		}
	}

	// 상수버퍼 바인딩
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding();

	// 쉐이더 바인딩
	m_Shader->Binding();
}

int CMaterial::Save(const wstring& _FilePath)
{
	// 어디에 저장해뒀는지 알고 있는게 좋음
	wstring relativePath = CPathMgr::GetInst()->GetRelativePath(_FilePath);
	SetRelativePath(relativePath);

	std::filesystem::path dir_path = _FilePath;
	if (!std::filesystem::exists(dir_path.parent_path())) 
		std::filesystem::create_directories(dir_path.parent_path()); // 중간 디렉토리 재귀적으로 생성

	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"wb");

	if (nullptr == File)
		return E_FAIL;

	// 어떤 쉐이더를 참조했는지
	SaveAssetRef(m_Shader, File);

	// 상수 데이터
	fwrite(&m_Const, sizeof(tMtrlConst), 1, File);

	for (UINT i = 0; i < TEX_PARAM::END; ++i)
	{
		SaveAssetRef(m_arrTex[i], File);
	}

	fclose(File);

	return S_OK;
}

int CMaterial::Load(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"rb");

	if (nullptr == File)
		return E_FAIL;

	// 어떤 쉐이더를 참조했는지
	LoadAssetRef(m_Shader, File);

	// 상수 데이터
	fread(&m_Const, sizeof(tMtrlConst), 1, File);

	for (UINT i = 0; i < TEX_PARAM::END; ++i)
	{
		LoadAssetRef(m_arrTex[i], File);
	}

	fclose(File);

	return S_OK;
}