#pragma once



template<typename T>
class Ptr
{
private:
	T* Asset;

public:
	T* Get() const { return Asset; }
	T** GetAdressOf() const { return &Asset; }
	T* operator ->() const { return Asset; }
	// operator T* () { return Asset; }


public:
	bool operator ==(Ptr<T> _Other) { return Asset == _Other.Asset; }

	bool operator ==(T* _Other) { return Asset == _Other; }

	bool operator != (Ptr<T> _Other) { return Asset != _Other.Asset; }

	bool operator !=(T* _Other) { return Asset != _Other; }

	bool operator !() { return !Asset; }

	Ptr& operator = (T* _Other)
	{
		if (nullptr != Asset)
			Asset->Release();

		Asset = _Other;

		if (nullptr != Asset)
			Asset->AddRef();

		return *this;
	}

	Ptr& operator = (const Ptr<T>& _Other)
	{
		if (nullptr != Asset)
			Asset->Release();

		Asset = _Other.Asset;

		if (nullptr != Asset)
			Asset->AddRef();

		return *this;
	}

public:
	Ptr()
		: Asset(nullptr)
	{}

	Ptr(T* _Asset)
		: Asset(_Asset)
	{
		if (nullptr != Asset)
			Asset->AddRef();
	}

	Ptr(const Ptr<T>& _Ptr)
		: Asset(_Ptr.Asset)
	{
		if (nullptr != Asset)
			Asset->AddRef();
	}

	~Ptr()
	{
		if (nullptr != Asset)
			Asset->Release();
	}
};


template<typename T>
bool operator == (void* _Asset, Ptr<T> _PtrAsset)
{
	return _Asset == _PtrAsset.Get();
}

template<typename T>
bool operator != (void* _Asset, Ptr<T> _PtrAsset)
{
	return _Asset != _PtrAsset.Get();
}