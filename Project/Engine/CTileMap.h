#pragma once
#include "CRenderComponent.h"

class CStructuredBuffer;

struct tTileInfo
{
	int ImgIdx;
	int padding[3];
};

class CTileMap :
	public CRenderComponent
{
private:
	int                     m_Row;					// TileMap �� �� ����
	int                     m_Col;					// TileMap �� �� ����
	Vec2                    m_TileSize;				// Tile 1���� ũ��

	Ptr<CTexture>           m_TileAtlas;			// Tile ���� �̹������� �����ϰ� �ִ� ��Ʋ�� �ؽ���
	Vec2                    m_AtlasResolution;		// Atlas �ؽ��� �ػ�
	Vec2                    m_AtlasTileResolution;  // Atlas �ؽ��� ������ Ÿ�� 1���� ũ��
	Vec2                    m_AtlasTileSliceUV;		// Atlas �ؽ��� ������ Ÿ�� 1���� ũ�⸦ UV �� ��ȯ�� ũ��(SliceUV)

	int                     m_AtlasMaxRow;			// Atlas �ؽ��İ� �����ϰ� �ִ� Ÿ���� �ִ� �� ����
	int                     m_AtlasMaxCol;			// Atlas �ؽ��İ� �����ϰ� �ִ� Ÿ���� �ִ� �� ����

	int						m_isShowTileGrid = false;		// Ÿ�� �׸��带 �������� ����

	vector<tTileInfo>       m_vecTileInfo;			// ������ Ÿ���� ����
	CStructuredBuffer*		m_structuredBuffer;		// ������ Ÿ�������� t �������ͷ� �����ϱ� ���� ����ȭ����


public:
	void SetRowCol(UINT _Row, UINT _Col);
	void SetTileSize(Vec2 _Size);
	void SetAtlasTexture(Ptr<CTexture> _Atlas, Vec2 _tileResolution = Vec2(0, 0));
	void SetAtlasTileResolution(Vec2 _TileSize);
	void SetTileGridShow(bool _Show) { m_isShowTileGrid = _Show; }

	Vec2 GetRowCol() { return Vec2(m_Row, m_Col); }
	Vec2 GetTileSize() { return m_TileSize; }
	Ptr<CTexture> GetAtlasTexture() { return m_TileAtlas; }
	Vec2 GetAtlasTileResolution() { return m_AtlasTileResolution; }
	Vec2 GetAtlasMaxRowCol() { return Vec2(m_AtlasMaxRow, m_AtlasMaxCol); }
	Vec2 GetAtlasTileSliceUV() { return m_AtlasTileSliceUV; }
	bool IsShowTileGrid() { return (bool)m_isShowTileGrid; }
	
	vector<tTileInfo>& GetTileInfoVec() { return m_vecTileInfo; }

private:
	void ChangeTileMapSize();


public:
	virtual void Init() override;
	virtual void FinalTick() override;
	virtual void Render() override;

	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

public:
	CTileMap();
	CTileMap(const CTileMap& _Origin);
	~CTileMap();
	virtual CTileMap* Clone() { return new CTileMap(*this); }
};

