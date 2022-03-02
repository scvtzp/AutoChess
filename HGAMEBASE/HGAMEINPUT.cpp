#include "HGAMEINPUT.h"
#include <Windows.h>
#include "HGAMEWINDOW.h"

std::map<Game_String, HGAMEINPUT::HGAMEKEY*> HGAMEINPUT::AllKey;

Game_Vector HGAMEINPUT::PrevPos = Game_Vector::ZERO;
Game_Vector HGAMEINPUT::MouseDir = Game_Vector::ZERO;

HGAMEINPUT::HGAMESTATIC::HGAMESTATIC() 
{

}

HGAMEINPUT::HGAMESTATIC::~HGAMESTATIC() 
{
	for (auto& Item: AllKey)
	{
		delete Item.second;
	}
}

// ����� ������ �����̴�.
HGAMEINPUT::HGAMESTATIC HGAMEINPUT::StaticInst;


bool HGAMEINPUT::HGAMEKEY::KeyCheck() 
{
	for (size_t i = 0; i < m_KeyVec.size(); i++)
	{
		// 'A' 'B' 
		if (0 == GetAsyncKeyState(m_KeyVec[i]))
		{
			// �ϳ��� �ȴ��ȴ�
			return false;
		}
	}

	// ���δ� ���ȴ�.
	return true;
}

// ���⼭ Ű üũ�� �Ѵ�.
void HGAMEINPUT::HGAMEKEY::Update()
{
	// �׷� ����� ������?
	// Ű�� �� ���ȴµ�?
	if (KeyCheck())
	{
		// ó�� ���ȴ�.
		if (false == Press)
		{
			Down = true;
			Press = true;
			Up = false;
			Free = false;
		}
		else 
		{
			Down = false;
			Press = true;
			Up = false;
			Free = false;
		// ���������ӿ� ���ȴ� ��� ������ �־���.
		}
	}
	else 
	{
		// �������� ������ �־���.
		if (true == Press)
		{
			Down = false;
			Press = false;
			Up = true;
			Free = true;
		}
		else
		{
			Down = false;
			Press = false;
			Up = false;
			Free = true;
			// ���������ӿ� ���ȴ� ��� ������ �־���.
		}
	}


}


void HGAMEINPUT::Update() 
{
	
	Game_Vector Pos = MousePos3D();
	// ������ġ ���� ������ġ
	MouseDir = Pos - PrevPos;
	PrevPos = Pos;

	if (MouseDir != Game_Vector::ZERO)
	{
		int a = 0;
	}

	for (auto& Item : AllKey)
	{
		Item.second->Update();
	}
}

Game_Vector HGAMEINPUT::MousePos3D() 
{
	return HGAMEWINDOW::MAINOBJ()->MousePosTo3D();
}

// ����� ũ�⸦ ���ÿ� �˰� ������.
Game_Vector HGAMEINPUT::MouseDir3D() {
	return MouseDir;
}

Game_Vector HGAMEINPUT::MouseDir3DNormal() 
{
	return MouseDir.NORMAL3DTORETURN();
}

 void HGAMEINPUT::Init()
{
	HGAMEINPUT::CreateKey(L"1", '1');
	HGAMEINPUT::CreateKey(L"2", '2');

	HGAMEINPUT::CreateKey(L"L", 'J');
	HGAMEINPUT::CreateKey(L"R", 'L');
	HGAMEINPUT::CreateKey(L"F", 'I');
	HGAMEINPUT::CreateKey(L"B", 'K');
	HGAMEINPUT::CreateKey(L"Test", 'T');

	HGAMEINPUT::CreateKey(L"A", 'A');
	HGAMEINPUT::CreateKey(L"D", 'D');
	HGAMEINPUT::CreateKey(L"CAMUP", 'Q');
	HGAMEINPUT::CreateKey(L"CAMDOWN", 'E');
	HGAMEINPUT::CreateKey(L"W", 'W');
	HGAMEINPUT::CreateKey(L"S", 'S');

	HGAMEINPUT::CreateKey(L"Mouse_L", VK_LBUTTON);
	HGAMEINPUT::CreateKey(L"Mouse_R", VK_RBUTTON);
}