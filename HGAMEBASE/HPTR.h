#pragma once

class Game_Ref
{
public:
	int REFCOUNT;

public:
	void ADDREFCOUNT() {
		++REFCOUNT;
	}
	void DECREFCOUNT(/*HREF* const this*/)
	{
		--REFCOUNT;
		if (0 >= REFCOUNT)
		{
			delete this;
		}
	}

public:
	Game_Ref() : REFCOUNT(0) {}
	virtual ~Game_Ref() {}
};

template<typename T>
class Game_Ptr
{
public:
	T* PTR;

public:
	// 포인터 연산자 오버로딩
	T* operator->()
	{
		return PTR;
	}

	operator T*()
	{
		return PTR;
	}

	// 강제로 변환하면 안전하지가 않다.
	// 오크가 드래곤이 되어버리는 사태가 발생한다.
	// const_cast; 
	// const를 const가 아니게 만든다. 사용하지 마세요
	// static_cast;
	// 이녀석은 우리가 알고 있는 일반적인 mamcpy를 통한 캐스트나 다름 없다.
	// 크기검사만 하는 캐스팅에 가깝다.
	// reinterpret_cast;
	// 언젠 기억도 안남. 
	// dynamic_cast;
	// 상속관계일때만 캐스팅을 해준다
	// 하지만 가상함수 테이블의 값을 기반으로 캐스팅을 해주기 때문에
	// VIRTUAL이 부모쪽에 있어야 한다.
	// 안전할때만 사용하겠다
	template<typename OTHERTYPE>
	operator Game_Ptr<OTHERTYPE>()
	{
		return dynamic_cast<OTHERTYPE*>(PTR);
	}

	bool operator==(void* _nullptr) const
	{
		return PTR == _nullptr;
	}

	bool operator==(const Game_Ptr<T>& _Other) const
	{
		return PTR == _Other.PTR;
	}


	bool operator!=(void* _nullptr) const
	{
		return PTR != _nullptr;
	}

	Game_Ptr& operator=(const Game_Ptr<T>& _Other)
	{
		if (nullptr != PTR)
		{
			PTR->DECREFCOUNT();
		}


		PTR = _Other.PTR;
		if (nullptr != PTR)
		{
			PTR->ADDREFCOUNT();
		}
		return *this;
	}

	Game_Ptr& operator=(T* const _Other)
	{
		if (nullptr != PTR)
		{
			PTR->DECREFCOUNT();
		}


		PTR = _Other;
		if (nullptr != PTR)
		{
			PTR->ADDREFCOUNT();
		}
		return *this;
	}

public:
	Game_Ptr(const Game_Ptr<T>& _Other) : PTR(_Other.PTR)
	{
		if (nullptr == PTR)
		{
			return;
		}
		PTR->ADDREFCOUNT();
	}

	Game_Ptr() : PTR(nullptr)
	{
	}

	Game_Ptr(T* _Ptr) : PTR(_Ptr)
	{
		if (nullptr == _Ptr)
		{
			return;
		}

		_Ptr->ADDREFCOUNT();
	}
	~Game_Ptr()
	{
		if (nullptr == PTR)
		{
			return;
		}

		PTR->DECREFCOUNT();
	}
};

template<typename T>
bool operator!=(void* _Ptr, const Game_Ptr<T> _TypeName) 
{
	return _TypeName.operator!=(_Ptr);
}

template<typename T>
bool operator==(void* _Ptr, const Game_Ptr<T> _TypeName)
{
	return _TypeName.operator==(_Ptr);
}


template<typename T>
class HWPTR
{
public:
	T* PTR;

public:
	// 포인터 연산자 오버로딩
	T* operator->()
	{
		return PTR;
	}

	HWPTR& operator=(const HWPTR<T>& _Other)
	{
		PTR = _Other.PTR;
		return *this;
	}

public:
	HWPTR(const Game_Ptr<T>& _Other) : PTR(_Other.PTR)
	{
	}

	HWPTR() : PTR(nullptr)
	{
	}

	HWPTR(T* _Ptr) : PTR(_Ptr)
	{
	}
	virtual ~HWPTR()
	{
	}
};