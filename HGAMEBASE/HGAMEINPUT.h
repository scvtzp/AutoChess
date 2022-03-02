#pragma once
#include <vector>
#include <assert.h>
#include "HGAMESTRING.h"
#include <map>
#include "HGAMEMATH.h"

class HGAMEINPUT
{
public:
	class HGAMEKEY
	{
	public:
		friend HGAMEINPUT;

	public:
		Game_String Name;
		std::vector<int> m_KeyVec;

		bool Down; // ó�� ���ȴ�
		bool Press; // ��� ������ �ִ�
		bool Up; // Ű�� ������
		bool Free; // ��� ���� ���´�

	private:
		bool KeyCheck();
		void Update();

	private:
		HGAMEKEY() : Down(false), Press(false), Up(false), Free(true) {}
		~HGAMEKEY() {}
		// 0 != GetAsyncKeyState('A')
	};




public:
	class HGAMESTATIC
	{
	public:
		HGAMESTATIC();
		~HGAMESTATIC();
	};
	// ����� ������ �����̴�.
	static HGAMESTATIC StaticInst;

private:
	static std::map<Game_String, HGAMEKEY*> AllKey;

public:
	static HGAMEKEY* FindKey(const Game_String& _KeyName)
	{
		if (AllKey.end() == AllKey.find(_KeyName))
		{
			return nullptr;
		}

		// �����ε� �ٷ� ���� �������ش�.
		return AllKey[_KeyName];
	}

	static bool IsKey(const Game_String& _KeyName)
	{
		return nullptr != FindKey(_KeyName);
	}

	template<typename ... REST>
	static void CreateKey(const Game_String& _KeyName, REST ... Arg)
	{
		HGAMEKEY* NewKey = FindKey(_KeyName);

		if (nullptr != NewKey)
		{
			assert(false);
		}

		NewKey = new HGAMEKEY();
		NewKey->Name = _KeyName;
		NewKey->m_KeyVec.reserve(sizeof...(Arg));
		PushKey(NewKey, Arg...);
	}
	static void Init();

private:
	template<typename T, typename ... REST>
	static void PushKey(HGAMEKEY* _NewKey, T _Value, REST ... Arg)
	{
		_NewKey->m_KeyVec.push_back(_Value);
		PushKey(_NewKey, Arg...);
	}

	static void PushKey(HGAMEKEY* _NewKey)
	{
		AllKey.insert(std::map<Game_String, HGAMEKEY*>::value_type(_NewKey->Name, _NewKey));
	}

public:
	static bool Down(const Game_String& _Key)
	{
		HGAMEKEY* Ptr = FindKey(_Key);
		if (nullptr == Ptr) { assert(false); }
		return Ptr->Down;
	}
	static bool Press(const Game_String& _Key) {
		HGAMEKEY* Ptr = FindKey(_Key);
		if (nullptr == Ptr) { assert(false); }
		return Ptr->Press;
	}
	static bool Up(const Game_String& _Key) {
		HGAMEKEY* Ptr = FindKey(_Key);
		if (nullptr == Ptr) { assert(false); }
		return Ptr->Up;
	}
	static bool Free(const Game_String& _Key) {
		HGAMEKEY* Ptr = FindKey(_Key);
		if (nullptr == Ptr) { assert(false); }
		return Ptr->Free;
	}

private:
	static Game_Vector PrevPos;
	static Game_Vector MouseDir;
public:
	static Game_Vector MousePos3D();
	static Game_Vector MouseDir3D();
	static Game_Vector MouseDir3DNormal();

public:
	static void Update();

};

