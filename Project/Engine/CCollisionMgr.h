#pragma once
#include "singleton.h"

class CCollider2D;

union COLLIDER_ID
{
    struct
    {
        DWORD LeftID;
        DWORD RightID;
    };

    ULONGLONG ID;
};
class CCollisionMgr :
    public CSingleton<CCollisionMgr>
{
private: 
    friend class CSingleton<CCollisionMgr>;
    CCollisionMgr(); 
    ~CCollisionMgr(); 

private:
    UINT					m_Matrix[MAX_LAYER];
    map<ULONGLONG, bool>	m_mapCollisionInfo;


public:
    void CollisionCheck(UINT _Layer1, UINT _Layer2);
    void CollisionCheckClear() { memset(m_Matrix, 0, sizeof(UINT) * MAX_LAYER); }

private:
    void CollisionBtwLayer(UINT _Left, UINT _Right);
    bool IsCollision(CCollider2D* _Left, CCollider2D* _Right);

public:
    void Tick();
};

