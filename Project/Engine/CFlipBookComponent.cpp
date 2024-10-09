#include "pch.h"
#include "CFlipBookComponent.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"
#include "CFlipBook.h"


CFlipBookComponent::CFlipBookComponent()
	: CComponent(COMPONENT_TYPE::FLIPBOOK_COMPONENT)
	, m_CurFlipBook(nullptr)
	, m_CurFrmIdx(0)
{
}

CFlipBookComponent::CFlipBookComponent(CFlipBookComponent& _Origin)
	: CComponent(_Origin)
	, m_mapFlipBook(_Origin.m_mapFlipBook)
	, m_CurFlipBook(_Origin.m_CurFlipBook)
	, m_CurFrmIdx(0)
	, m_FPS(_Origin.m_FPS)
	, m_AccTime(0.f)
	, m_Repeat(_Origin.m_Repeat)
	, m_Finish(false)
{
	if (nullptr != m_CurFlipBook)
	{
		Play(path(m_CurFlipBook->GetKey()).stem().wstring(), m_FPS, m_Repeat);
	}
}

CFlipBookComponent::~CFlipBookComponent()
{
}

void CFlipBookComponent::FinalTick()
{
	if (m_Finish)
	{
		if (false == m_Repeat)
			return;

		m_CurFrmIdx = 0;
		m_Finish = false;
	}

	if (nullptr != m_CurFlipBook)
	{
		float MaxTime = 1.f / m_FPS;

		m_AccTime += DT;

		if (MaxTime < m_AccTime)
		{
			m_AccTime -= MaxTime;
			++m_CurFrmIdx;

			if (m_CurFlipBook->GetMaxFrameCount() <= m_CurFrmIdx)
			{
				--m_CurFrmIdx;
				m_Finish = true;
			}
		}

		m_CurFrmSprite = m_CurFlipBook->GetSprite(m_CurFrmIdx);
	}
}

vector<wstring> CFlipBookComponent::GetFlipBookNames()
{
	vector<wstring> FlipBookNames;
	for(auto& pair : m_mapFlipBook)
	{
		FlipBookNames.push_back(pair.first);
	}
	return FlipBookNames;
}

void CFlipBookComponent::AddFlipBook(Ptr<CFlipBook> _Flipbook)
{
	wstring flipBookName = path(_Flipbook->GetKey()).stem().wstring();
	if (m_mapFlipBook.find(flipBookName) != m_mapFlipBook.end())
	{
		MessageBox(nullptr, (L"중복된 FlipBook 키가 존재합니다. \n" + flipBookName).c_str(), L"중복된 FlipBook 키", MB_OK);
		return;
	}
	else
		m_mapFlipBook.insert(make_pair(flipBookName, _Flipbook));
}

Ptr<CFlipBook> CFlipBookComponent::FindFlipBook(const wstring& _FlipBookName)
{
	auto iter = m_mapFlipBook.find(_FlipBookName);
	if (iter == m_mapFlipBook.end())
	{
		assert(false);
		MessageBox(nullptr, L"해당 이름의 FlipBook이 존재하지 않습니다.", L"FlipBook 찾기 실패", MB_OK);
		return nullptr;
	}
	else
	{
		return iter->second;
	}
}

void CFlipBookComponent::Play(wstring _FliBookName, float _FPS, bool _Repeat)
{
	auto iter =  m_mapFlipBook.find(_FliBookName);
	if (iter == m_mapFlipBook.end())
	{
		assert(false);
		MessageBox(nullptr, L"해당 이름의 FlipBook이 존재하지 않습니다.", L"FlipBook 찾기 실패", MB_OK);
		return;
	}
	else
	{
		m_CurFlipBook = iter->second;
	}

	Reset();
	m_FPS = _FPS;
	m_Repeat = _Repeat;

}

void CFlipBookComponent::Reset()
{
	m_CurFrmIdx = 0;
	m_AccTime = 0.f;
	m_Finish = false;
}

void CFlipBookComponent::Binding()
{
	if (nullptr != m_CurFrmSprite)
	{
		// 현재 표시해야하는 Sprite 의 정보를 Sprite 전용 상수버퍼를 통해서 GPU 에 전달
		tSpriteInfo tInfo = {};

		tInfo.LeftTopInAtlasUV = m_CurFrmSprite->GetLeftTopInAtlasUV();
		tInfo.SliceSizeInAtlasUV = m_CurFrmSprite->GetSliceSizeInAtlasUV();
		tInfo.BackGroundSizeInAtlasUV = m_CurFrmSprite->GetBackgroundSizeInAtlasUV();
		tInfo.OffsetUV = m_CurFrmSprite->GetOffsetUV();
		tInfo.UseFlipbook = 1;

		static CConstBuffer* CB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::SPRITE);

		CB->SetData(&tInfo);
		CB->Binding();

		// FlipBook Sprite 아틀라스 텍스쳐 전용 레지스터번호 t10 에 바인딩
		Ptr<CTexture> pAtlas = m_CurFrmSprite->GetAtlasTexture();
		pAtlas->Binding(10);
	}
	else
	{
		Clear();
	}
}

void CFlipBookComponent::Clear()
{
	tSpriteInfo tInfo = {};
	static CConstBuffer* CB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::SPRITE);
	CB->SetData(&tInfo);
	CB->Binding();
}

void CFlipBookComponent::SaveToFile(FILE* _File)
{
	// FlipBook 에셋맵을 저장
	size_t FlipBookCount = m_mapFlipBook.size();
	fwrite(&FlipBookCount, sizeof(size_t), 1, _File);
	for (auto& pair : m_mapFlipBook)
	{
		SaveAssetRef(pair.second, _File);
	}

	// 현재 재생중인 FlipBook 정보 저장
	SaveAssetRef(m_CurFlipBook, _File);

	// 현재 재생중인 FlipBook 내에서 지정된 Sprite
	SaveAssetRef(m_CurFrmSprite, _File);

	// 현재 재생중인 FlipBook 내에서 지정된 Sprite 가 몇번째 인덱스인지
	fwrite(&m_CurFrmIdx, sizeof(int), 1, _File);
	fwrite(&m_FPS, sizeof(float), 1, _File);
	fwrite(&m_AccTime, sizeof(float), 1, _File);
	fwrite(&m_Repeat, sizeof(bool), 1, _File);
}

void CFlipBookComponent::LoadFromFile(FILE* _File)
{
	// FlipBook 에셋 목록 불러오기
	size_t FlipBookCount = 0;
	fread(&FlipBookCount, sizeof(size_t), 1, _File);
	for (size_t i = 0; i < FlipBookCount; ++i)
	{
		Ptr<CFlipBook> pFlipBook;
		LoadAssetRef(pFlipBook, _File);
		wstring flipBookName = path(pFlipBook->GetKey()).stem().wstring();
		if (m_mapFlipBook.find(flipBookName) != m_mapFlipBook.end())
		{
			assert(false);
			MessageBox(nullptr, L"중복된 FlipBook 키가 존재합니다.", L"중복된 FlipBook 키", MB_OK);
		}
		else
			m_mapFlipBook.insert(make_pair(flipBookName, pFlipBook));
	}

	// 현재 재생중인 FlipBook 정보 로드
	LoadAssetRef(m_CurFlipBook, _File);

	// 현재 재생중인 FlipBook 내에서 지정된 Sprite
	LoadAssetRef(m_CurFrmSprite, _File);

	// 현재 재생중인 FlipBook 내에서 지정된 Sprite 가 몇번째 인덱스인지
	fread(&m_CurFrmIdx, sizeof(int), 1, _File);
	fread(&m_FPS, sizeof(float), 1, _File);
	fread(&m_AccTime, sizeof(float), 1, _File);
	fread(&m_Repeat, sizeof(bool), 1, _File);
}