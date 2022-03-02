#pragma once
#include "Game_Trans.h"
#include <unordered_set>
#include <functional>
#include <list>
#include "HGAMEDEBUG.h"

enum class Col_Name
{
	Shop_Unit, //상점 유닛
	Unit, //유닛
	Mouse, //마우스
	Board, //체스보드
	Button, //버튼
	End
};

class Game_Collision : public Game_Trans
{
private:
	COLTYPE m_Type;
	Game_String Note; //메모용. 야매
	int intNote; //메모용. 야매

public:
	//메모 반환
	Game_String Get_Note() 
	{ 
		return Note; 
	}

	//메모 세팅
	void Set_Note(Game_String _String)
	{
		Note = _String; 
	}
	//메모 반환
	int Get_intNote()	{	return intNote;	}

	//메모 세팅
	void Set_intNote(int _int)	{ intNote = _int;	}

public:
	void ColType(COLTYPE _Type)
	{
		m_Type = _Type;
	}

private:
	// 일반적으로 레드블랙트리인 셋보다 빠르다.
	// 긴문자열인 경우에는
	// 너무 신경쓸 필요는 없다.
	std::unordered_set<Game_Collision*> m_ColSet;

private:
	std::list<std::function<void(Game_Collision*, Game_Collision*)>> m_EnterFunc;
	std::list<std::function<void(Game_Collision*, Game_Collision*)>> m_StayFunc;
	std::list<std::function<void(Game_Collision*, Game_Collision*)>> m_ExitFunc;

public:
	template<typename T>
	void PushEnterFunc(T* _Obj, void(T::*_Ptr)(Game_Collision*, Game_Collision*)) 
	{
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr인 함수를 넣어줬습니다. 실행할수 없습니다.");
		}

		m_EnterFunc.push_back(std::bind(_Ptr, _Obj, std::placeholders::_1, std::placeholders::_2));
	}
	template<typename T>
	void PushStayFunc(T* _Obj, void(T::* _Ptr)(Game_Collision*, Game_Collision*)) {
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr인 함수를 넣어줬습니다. 실행할수 없습니다.");
		}

		m_StayFunc.push_back(std::bind(_Ptr, _Obj, std::placeholders::_1, std::placeholders::_2));

	}
	template<typename T>
	void PushExitFunc(T* _Obj, void(T::* _Ptr)(Game_Collision*, Game_Collision*)) {
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr인 함수를 넣어줬습니다. 실행할수 없습니다.");
		}

		m_ExitFunc.push_back(std::bind(_Ptr, _Obj, std::placeholders::_1, std::placeholders::_2));

	}

	void PushEnterFunc(void(*_Ptr)(Game_Collision*, Game_Collision*))
	{
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr인 함수를 넣어줬습니다. 실행할수 없습니다.");
		}

		m_EnterFunc.push_back(std::bind(_Ptr, std::placeholders::_1, std::placeholders::_2));
	}
	void PushStayFunc(void(*_Ptr)(Game_Collision*, Game_Collision*)) 
	{
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr인 함수를 넣어줬습니다. 실행할수 없습니다.");
		}

		m_StayFunc.push_back(std::bind(_Ptr, std::placeholders::_1, std::placeholders::_2));
	}
	void PushExitFunc(void(*_Ptr)(Game_Collision*, Game_Collision*)) {
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr인 함수를 넣어줬습니다. 실행할수 없습니다.");
		}

		m_ExitFunc.push_back(std::bind(_Ptr, std::placeholders::_1, std::placeholders::_2));
	}



private:
	void CallEnter(Game_Collision* _Other);
	void CallStay(Game_Collision* _Other);
	void CallExit(Game_Collision* _Other);

public:
	void ColCheck(Game_Ptr<Game_Collision> _Other);

	void Release() override;

public:
	void Init(int _Order = 0);
};


