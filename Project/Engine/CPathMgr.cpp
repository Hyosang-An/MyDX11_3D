#include "pch.h"
#include "CPathMgr.h"

#include "CEngine.h"
#include <filesystem>

CPathMgr::CPathMgr()
{
}

CPathMgr::~CPathMgr()
{
}

void CPathMgr::Init()
{
	// 실행 경로를 얻어낸다.
	wchar_t szBuffer[256]{};
	GetCurrentDirectory(256, szBuffer);

	wstring strCurrentPath{ szBuffer };
	wstring strBuildPath = GetParentPath(strCurrentPath);

	m_strContentPath = strBuildPath + L"\\content\\";
	m_strSolutionPath = GetParentPath(strBuildPath);
}

void CPathMgr::Render()
{
}

wstring CPathMgr::GetRelativePath(const wstring& _FilePath)
{
	size_t FindPos = _FilePath.find(m_strContentPath);
	if (FindPos == wstring::npos)
		return L"";

	return _FilePath.substr(FindPos + m_strContentPath.length(), _FilePath.length());
}

wstring CPathMgr::GetParentPath(const wstring& _strPath)
{
	std::filesystem::path fsPath{ _strPath };
	return fsPath.parent_path().wstring();
}
