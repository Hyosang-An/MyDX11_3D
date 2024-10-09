#pragma once

#include "EditorUI.h"

class ParamUI
{
private:
	static UINT g_ID;

public:
	static void ResetID() { g_ID = 0; }

public:
	static bool InputInt(int* _Data, const string& _Desc);
	static bool DragInt(int* _Data, float _Step, const string& _Desc);

	static bool InputFloat(float* _Data, const string& _Desc);
	static bool DragFloat(float* _Data, float _Step, const string& _Desc);

	static bool InputVec2(Vec2* _Data, const string& _Desc);
	static bool DragVec2(Vec2* _Data, float _Step, const string& _Desc);

	static bool InputVec4(Vec4* _Data, const string& _Desc);
	static bool DragVec4(Vec4* _Data, float _Step, const string& _Desc);

	static bool InputTexture(Ptr<CTexture>& _CurTex, const string& _Desc, EditorUI* _Inst = nullptr, DELEGATE_1 _MemFunc = nullptr);

	static bool InputPrefab(Ptr<CPrefab>& _CurPrefab, const string& _Desc, EditorUI* _Inst = nullptr, DELEGATE_1 _MemFunc = nullptr);
};

