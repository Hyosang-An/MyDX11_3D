#include "pch.h"

#include "CRenderMgr.h"
#include "CTaskMgr.h"
#include "CLevelMgr.h"

#include "CGameObject.h"

void SpawnObject(CGameObject* _NewObject, int _LayerIndex)
{
	tTask task = {};
	task.Type = TASK_TYPE::SPAWN_OBJECT;
	task.Param_0 = _LayerIndex;
	task.Param_1 = (DWORD_PTR)_NewObject;

	CTaskMgr::GetInst()->AddTask(task);
}

void DeleteObject(CGameObject* _DeleteObject)
{
	tTask task = {};
	task.Type = TASK_TYPE::DELETE_OBJECT;
	task.Param_0 = (DWORD_PTR)_DeleteObject;

	CTaskMgr::GetInst()->AddTask(task);

	CLevelMgr::GetInst()->LevelChanged();
}

void ChangeLevelState(LEVEL_STATE _NextState)
{
	tTask task = {};
	task.Type = TASK_TYPE::CHANGE_LEVELSTATE;
	task.Param_0 = (DWORD_PTR)_NextState;

	CTaskMgr::GetInst()->AddTask(task);
}

void ChangeLevel(CLevel* _nextLevel, LEVEL_STATE _nextLevelState)
{
	tTask task = {};
	task.Type = TASK_TYPE::CHANGE_LEVEL;

	task.Param_0 = (DWORD_PTR)_nextLevel;
	task.Param_1 = (DWORD_PTR)_nextLevelState;

	CTaskMgr::GetInst()->AddTask(task);
}

bool IsValid(CGameObject*& _Object)
{
	if (nullptr == _Object)
		return false;

	if (_Object->IsDead())
	{
		_Object = nullptr;
		return false;
	}

	return true;
}

void DrawDebugRect(Vec3 _Pos, Vec3 _Scale, Vec3 _Rot, Vec4 _Color, float _Life, bool _DepthTest)
{
	tDebugShapeInfo Info = {};

	Info.Shape = DEBUG_SHAPE::RECT;
	Info.vPos = _Pos;
	Info.vScale = _Scale;
	Info.vRot = _Rot;
	Info.LifeTime = _Life;

	Info.matWorld = XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z)
		* XMMatrixRotationX(_Rot.x)
		* XMMatrixRotationY(_Rot.y)
		* XMMatrixRotationZ(_Rot.z)
		* XMMatrixTranslation(_Pos.x, _Pos.y, _Pos.z);

	Info.vColor = _Color;
	Info.DepthTest = _DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(Info);
}

void DrawDebugRect(const Matrix& _matWorld, Vec4 _Color, float _Life, bool _DepthTest)
{
	tDebugShapeInfo Info = {};

	Info.Shape = DEBUG_SHAPE::RECT;
	Info.matWorld = _matWorld;
	Info.vColor = _Color;
	Info.DepthTest = _DepthTest;
	Info.LifeTime = _Life;

	CRenderMgr::GetInst()->AddDebugShapeInfo(Info);
}

void DrawDebugCircle(Vec3 _Pos, float _Radius, Vec4 _Color, float _Life, bool _DepthTest)
{
	tDebugShapeInfo Info = {};

	Info.Shape = DEBUG_SHAPE::CIRCLE;
	Info.vPos = _Pos;
	Info.vScale = Vec3(_Radius * 2.f, _Radius * 2.f, 1.f);
	Info.vRot = Vec3(0.f, 0.f, 0.f);
	Info.LifeTime = _Life;

	Info.matWorld = XMMatrixScaling(Info.vScale.x, Info.vScale.y, Info.vScale.z)
		* XMMatrixTranslation(_Pos.x, _Pos.y, _Pos.z);

	Info.vColor = _Color;
	Info.DepthTest = _DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(Info);
}

void SaveWString(const wstring& _String, FILE* _File)
{
	size_t len = _String.length();
	fwrite(&len, sizeof(size_t), 1, _File);
	fwrite(_String.c_str(), sizeof(wchar_t), len, _File);
}

void LoadWString(wstring& _String, FILE* _File)
{
	size_t len = 0;
	fread(&len, sizeof(size_t), 1, _File);

	_String.resize(len);
	fread((wchar_t*)_String.c_str(), sizeof(wchar_t), len, _File);
}

wstring incrementTrailingNumber(const std::wstring& input)
{
	size_t length = input.length();
	size_t pos = length; // ���ڰ� �ƴ� �κ��� ��ġ

	// ���ڿ��� ���������� ���ڰ� �ƴ� �κ��� ã���ϴ�.
	while (pos > 0 && std::isdigit(input[pos - 1])) {
		--pos;
	}

	// ���ڰ� �ƴ� �κа� ���� �κ��� �и��մϴ�.
	std::wstring nonNumberPart = input.substr(0, pos);
	std::wstring numberPart = input.substr(pos);

	// ���ڰ� �ִ� ��� ���ڸ� ������ŵ�ϴ�.
	if (!numberPart.empty()) {
		int number = std::stoi(numberPart);  // ���ڷ� ��ȯ
		number++;  // ���� ����
		return nonNumberPart + std::to_wstring(number);  // ������ ���ڿ� ����
	}
	else {
		// ���ڰ� ���� ��� ���� 1�� �߰��մϴ�.
		return nonNumberPart + L"_1";
	}
}
