#pragma once
#include <HMGR.h>
#include "HDEVICEBUFFER.h"
#include <HPTR.h>
#include <FW1FontWrapper.h>

#pragma comment(lib, "FW1FontWrapper.lib")

class HGAMEFONT : public HMGR<HGAMEFONT>
{
public:
	static void Create(const Game_String& _Name)
	{
		Game_Ptr<HGAMEFONT> Res = CreateToInsert(_Name);
		Res->CreateGameFont(_Name);
	}

private:
	class HGAMEFONTSTART
	{
	public:
		~HGAMEFONTSTART();
	};
	friend HGAMEFONTSTART;
	static HGAMEFONTSTART Starter;

public:
	static void Init();

private:
	static IFW1Factory* m_pFontFactory;
	IFW1FontWrapper* m_pFontWrapper;

public:
	// ����ũ�⸦ Ư���ϰ� �Ž��� ����� ���� �빮��.
	// ������Ʈ�� ���̴��� ����. 
	// �⺻������ ������ ��ǥ�踦 ������� �ϴ� �Լ��� �����̴�. 
	void DrawFont(const Game_String& _Text, float _Size, Game_Vector _Pos, Game_Vector _Color, FW1_TEXT_FLAG _Flag = FW1_TOP);



public:
	HGAMEFONT();
	~HGAMEFONT();

public:
	void CreateGameFont(const Game_String& _Name);
};

