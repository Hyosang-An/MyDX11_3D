#pragma once
class CPathMgr
	: public CSingleton<CPathMgr>
{
private:
	friend class CSingleton<CPathMgr>;
	CPathMgr();
	~CPathMgr();

private:
	wstring m_strContentPath;
	wstring m_strSolutionPath;

public:
	void Init();
	void Render();

	const wstring& GetContentPath() { return m_strContentPath; }
	const wstring& GetSolutionPath() { return m_strSolutionPath; }
	wstring GetRelativePath(const wstring& _FilePath);

private:
	wstring GetParentPath(const wstring& _strPath);
};

