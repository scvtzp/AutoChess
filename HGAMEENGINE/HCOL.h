#pragma once
#include "Game_Trans.h"
#include <unordered_set>
#include <functional>
#include <list>
#include "HGAMEDEBUG.h"

enum class Col_Name
{
	Shop_Unit, //���� ����
	Unit, //����
	Mouse, //���콺
	Board, //ü������
	Button, //��ư
	End
};

class Game_Collision : public Game_Trans
{
private:
	COLTYPE m_Type;
	Game_String Note; //�޸��. �߸�
	int intNote; //�޸��. �߸�

public:
	//�޸� ��ȯ
	Game_String Get_Note() 
	{ 
		return Note; 
	}

	//�޸� ����
	void Set_Note(Game_String _String)
	{
		Note = _String; 
	}
	//�޸� ��ȯ
	int Get_intNote()	{	return intNote;	}

	//�޸� ����
	void Set_intNote(int _int)	{ intNote = _int;	}

public:
	void ColType(COLTYPE _Type)
	{
		m_Type = _Type;
	}

private:
	// �Ϲ������� �����Ʈ���� �º��� ������.
	// �乮�ڿ��� ��쿡��
	// �ʹ� �Ű澵 �ʿ�� ����.
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
			AMSG(L"nullptr�� �Լ��� �־�����ϴ�. �����Ҽ� �����ϴ�.");
		}

		m_EnterFunc.push_back(std::bind(_Ptr, _Obj, std::placeholders::_1, std::placeholders::_2));
	}
	template<typename T>
	void PushStayFunc(T* _Obj, void(T::* _Ptr)(Game_Collision*, Game_Collision*)) {
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr�� �Լ��� �־�����ϴ�. �����Ҽ� �����ϴ�.");
		}

		m_StayFunc.push_back(std::bind(_Ptr, _Obj, std::placeholders::_1, std::placeholders::_2));

	}
	template<typename T>
	void PushExitFunc(T* _Obj, void(T::* _Ptr)(Game_Collision*, Game_Collision*)) {
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr�� �Լ��� �־�����ϴ�. �����Ҽ� �����ϴ�.");
		}

		m_ExitFunc.push_back(std::bind(_Ptr, _Obj, std::placeholders::_1, std::placeholders::_2));

	}

	void PushEnterFunc(void(*_Ptr)(Game_Collision*, Game_Collision*))
	{
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr�� �Լ��� �־�����ϴ�. �����Ҽ� �����ϴ�.");
		}

		m_EnterFunc.push_back(std::bind(_Ptr, std::placeholders::_1, std::placeholders::_2));
	}
	void PushStayFunc(void(*_Ptr)(Game_Collision*, Game_Collision*)) 
	{
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr�� �Լ��� �־�����ϴ�. �����Ҽ� �����ϴ�.");
		}

		m_StayFunc.push_back(std::bind(_Ptr, std::placeholders::_1, std::placeholders::_2));
	}
	void PushExitFunc(void(*_Ptr)(Game_Collision*, Game_Collision*)) {
		if (nullptr == _Ptr)
		{
			AMSG(L"nullptr�� �Լ��� �־�����ϴ�. �����Ҽ� �����ϴ�.");
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


