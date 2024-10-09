#include "pch.h"
#include "CEntity.h"

UINT CEntity::g_ID = 0;

CEntity::CEntity()
	: m_ID(g_ID++)
{
}

CEntity::CEntity(const CEntity& _Other)
	: m_ID(g_ID++)
	, m_Name(_Other.m_Name)
{
}

CEntity::~CEntity()
{
}
