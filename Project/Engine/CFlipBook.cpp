#include "pch.h"
#include "CFlipBook.h"

#include "CAssetMgr.h"

CFlipBook::CFlipBook()
	: CAsset(ASSET_TYPE::FLIPBOOK)
{
}

CFlipBook::~CFlipBook()
{
}

void CFlipBook::FinalTick()
{

}

void CFlipBook::SetAllSpritesBackgroundPixelSize(Vec2 _Size)
{
	for (size_t i = 0; i < m_vecSprite.size(); ++i)
	{
		m_vecSprite[i]->SetBackgroundPixelSize(_Size);
	}
}

int CFlipBook::Save(const wstring& _FilePath)
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

	size_t SpriteCount = m_vecSprite.size();
	fwrite(&SpriteCount, sizeof(size_t), 1, File);

	for (size_t i = 0; i < SpriteCount; ++i)
	{
		SaveAssetRef(m_vecSprite[i], File);
	}

	fclose(File);

	return S_OK;
}

int CFlipBook::Load(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"rb");
	if (nullptr == File)
		return E_FAIL;

	size_t SpriteCount = 0;
	fread(&SpriteCount, sizeof(size_t), 1, File);
	m_vecSprite.resize(SpriteCount);

	for (size_t i = 0; i < SpriteCount; ++i)
	{
		LoadAssetRef(m_vecSprite[i], File);
	}

	fclose(File);

	return S_OK;
}