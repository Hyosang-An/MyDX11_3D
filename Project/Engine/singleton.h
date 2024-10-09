#pragma once
// 동적할당
// 1. 런타임에 생성 및 해제가 가능
// 2. 프로그램 종료시 반드시 객체 메모리 해제 시켜주어야 함

// 전역변수
// 1. 객체 메모리 해제에 신경쓰지 않아도 됨
// 2. 런타임 내내 전역변수로써 해제시킬 수 없음


template<typename T>
class CSingleton
{
private:
	static T* g_Inst;

	typedef void (*EXIT)(void);

public:
	static T* GetInst()
	{
		if (nullptr == g_Inst)
		{
			g_Inst = new T;
		}

		return g_Inst;
	}

	static void Destroy()
	{
		if (g_Inst)
		{
			delete g_Inst;
			g_Inst = nullptr;
		}
	}

public:
	CSingleton()
	{
		atexit((EXIT)&CSingleton<T>::Destroy);
	}

	CSingleton(const CSingleton& _Other) = delete;
	virtual ~CSingleton() {}
};

template<typename T>
T* CSingleton<T>::g_Inst = nullptr;