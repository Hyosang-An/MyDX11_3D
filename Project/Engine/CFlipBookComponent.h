#pragma once
#include "CComponent.h"
class CFlipBookComponent :
    public CComponent
{
private:
    map<wstring, Ptr<CFlipBook>>    m_mapFlipBook;  // FliBook ������Ʈ�� ������ ��� FlipBook ���
    Ptr<CFlipBook>                  m_CurFlipBook;  // ���� ������� FlipBook
    Ptr<CSprite>                    m_CurFrmSprite; // ���� ������� FlipBook ���� ���� ������ �ε����� �ش��ϴ� ��������Ʈ
    int                             m_CurFrmIdx;    // ���� ������� FlipBook ���� ���° Sprite �� ��������� �ε��� ���
    float                           m_FPS = 0;          // ���� ������� FlipBook �� �ʴ� ������ ���� ��
    float                           m_AccTime = 0;      // ���� �ð��� üũ
    bool                            m_Repeat = true;
    bool                            m_Finish = false;       // FlipBook ����� ���� �����ߴ��� ����

public:
    virtual void FinalTick() override;
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;


public:
    vector<wstring> GetFlipBookNames();

    void AddFlipBook(Ptr<CFlipBook> _Flipbook);
    Ptr<CFlipBook> FindFlipBook(const wstring& _FlipBookName);
    void Play(wstring _FliBookName, float _FPS, bool _Repeat);
    Ptr<CSprite> GetCurSprite() { return m_CurFrmSprite; }
    void Reset();
    void Binding();
    static void Clear();


public:
    CFlipBookComponent();
    CFlipBookComponent(CFlipBookComponent& _Origin);
    ~CFlipBookComponent();
    virtual CFlipBookComponent* Clone() { return new CFlipBookComponent(*this); }
};

