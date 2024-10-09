#include "pch.h"
#include "CTileMap.h"

#include "CTransform.h"
#include "CAssetMgr.h"

#include "CStructuredBuffer.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_Row(1)
	, m_Col(1)
	, m_AtlasMaxRow(0)
	, m_AtlasMaxCol(0)
	, m_structuredBuffer(nullptr)
{
	SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TileMapMtrl"));

	m_structuredBuffer = new CStructuredBuffer;
	m_vecTileInfo.resize(1);
}

CTileMap::CTileMap(const CTileMap& _Origin) :
	CRenderComponent(_Origin),
	m_Row(_Origin.m_Row),
	m_Col(_Origin.m_Col),
	m_TileSize(_Origin.m_TileSize),
	m_TileAtlas(_Origin.m_TileAtlas),
	m_AtlasResolution(_Origin.m_AtlasResolution),
	m_AtlasTileResolution(_Origin.m_AtlasTileResolution),
	m_AtlasTileSliceUV(_Origin.m_AtlasTileSliceUV),
	m_AtlasMaxRow(_Origin.m_AtlasMaxRow),
	m_AtlasMaxCol(_Origin.m_AtlasMaxCol),
	m_isShowTileGrid(_Origin.m_isShowTileGrid),
	m_vecTileInfo(_Origin.m_vecTileInfo),
	m_structuredBuffer(nullptr)
{
	m_structuredBuffer = new CStructuredBuffer;
}
CTileMap::~CTileMap()
{
	delete m_structuredBuffer;
}

void CTileMap::Init()
{
	// ��, �� �����ؼ� ����ȭ���� ũ�� ����
	SetRowCol(m_Row, m_Col);
	SetTileSize(m_TileSize);
}

void CTileMap::FinalTick()
{
}

void CTileMap::Render()
{
	// Ÿ���� ������ ����ȭ���۸� ���ؼ� t �������Ϳ� ���ε� ��Ų��.
	m_structuredBuffer->SetData(m_vecTileInfo.data(), sizeof(tTileInfo) * m_Row * m_Col);
	m_structuredBuffer->Binding(15);

	GetMaterial()->SetTexParam(TEX_0, m_TileAtlas);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_2, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(INT_3, m_isShowTileGrid);
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasTileSliceUV);
	GetMaterial()->SetScalarParam(VEC2_1, Vec2(m_Col, m_Row));
	GetMaterial()->Binding();
	Transform()->Binding();
	GetMesh()->Render();
}

void CTileMap::SetRowCol(UINT _Row, UINT _Col)
{
	UINT prevRow = m_Row;
	UINT prevCol = m_Col;
	m_Row = _Row;
	m_Col = _Col;

	ChangeTileMapSize();

	// Ÿ�� ����
	UINT TileCount = m_Row * m_Col;

	// Ÿ�� ������ �����ϴ� ������ ������ ������ Ÿ�ϰ����� �ٸ��� ��������
	if (m_vecTileInfo.size() != TileCount)
	{
		vector<tTileInfo> prevVecTileInfo = m_vecTileInfo;

		m_vecTileInfo.clear();
		m_vecTileInfo.resize(TileCount);

		// ���� �����͸� ���� ����� ������ �°� ���ο� ���Ϳ� ����
		for (size_t i = 0; i < prevRow; i++)
		{
			for (size_t j = 0; j < prevCol; j++)
			{
				if (i >= m_Row || j >= m_Col)
					continue;

				m_vecTileInfo[i * m_Col + j] = prevVecTileInfo[i * prevCol + j];
			}
		}
	}

	// Ÿ�������� ���޹޾Ƽ� t �������Ϳ� ���޽�ų ����ȭ���۰� Ÿ�� ��ü ������ ������� ������ ��������
	auto elementsize = m_structuredBuffer->GetElementSize();
	auto tileinfosize = sizeof(tTileInfo) * TileCount;
	if (m_structuredBuffer->GetBufferSize() < sizeof(tTileInfo) * TileCount)
	{
		m_structuredBuffer->Create(sizeof(tTileInfo), TileCount);
	}

	//// Ÿ�� �������� �Ѱ��� ������� �ֱ�
	//for (int i = 0; i < m_vecTileInfo.size(); i++)
	//{
	//	m_vecTileInfo[i].ImgIdx = i;
	//}
}

void CTileMap::SetTileSize(Vec2 _Size)
{
	m_TileSize = _Size;

	ChangeTileMapSize();
}

void CTileMap::ChangeTileMapSize()
{
	Vec2 vSize = m_TileSize * Vec2((float)m_Col, (float)m_Row);
	Transform()->SetRelativeScale(vSize.x, vSize.y, 1.f);
}

void CTileMap::SetAtlasTexture(Ptr<CTexture> _Atlas, Vec2 _tileResolution)
{
	m_TileAtlas = _Atlas;

	if (nullptr == m_TileAtlas)
		m_AtlasResolution = Vec2(0.f, 0.f);
	else
		m_AtlasResolution = Vec2((float)_Atlas->Width(), (float)_Atlas->Height());

	if (_tileResolution != Vec2(0, 0))
		SetAtlasTileResolution(_tileResolution);
	else
		SetAtlasTileResolution(m_AtlasTileResolution);
}

void CTileMap::SetAtlasTileResolution(Vec2 _TileResolution)
{
	m_AtlasTileResolution = _TileResolution;

	if (nullptr != m_TileAtlas)
	{
		m_AtlasTileSliceUV = m_AtlasTileResolution / m_AtlasResolution;

		m_AtlasMaxCol = int(m_AtlasResolution.x / m_AtlasTileResolution.x);
		m_AtlasMaxRow = int(m_AtlasResolution.y / m_AtlasTileResolution.y);
	}
}

void CTileMap::SaveToFile(FILE* _File)
{
	// �θ�Ŭ������ RenderComponent �� SaveToFile ȣ��
	SaveDataToFile(_File);

	fwrite(&m_Col, sizeof(int), 1, _File);
	fwrite(&m_Row, sizeof(int), 1, _File);

	fwrite(&m_TileSize, sizeof(Vec2), 1, _File);
	fwrite(&m_AtlasTileResolution, sizeof(Vec2), 1, _File);

	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fwrite(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}

	// ��Ʋ�� �ؽ���
	SaveAssetRef(m_TileAtlas, _File);
}

void CTileMap::LoadFromFile(FILE* _File)
{
	// �θ�Ŭ������ RenderComponent �� LoadFromFile ȣ��
	LoadDataFromFile(_File);
	
	int col = 0;
	int row = 0;

	fread(&col, sizeof(int), 1, _File);
	fread(&row, sizeof(int), 1, _File);

	SetRowCol(row, col);

	fread(&m_TileSize, sizeof(Vec2), 1, _File);
	fread(&m_AtlasTileResolution, sizeof(Vec2), 1, _File);

	SetTileSize(m_TileSize);

	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fread(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}

	// ��Ʋ�� �ؽ���
	LoadAssetRef(m_TileAtlas, _File);
	if (nullptr != m_TileAtlas)
	{
		SetAtlasTexture(m_TileAtlas, m_AtlasTileResolution);
	}
}