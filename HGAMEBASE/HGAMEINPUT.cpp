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

// 배웠던 문법의 응용이다.
HGAMEINPUT::HGAMESTATIC HGAMEINPUT::StaticInst;


bool HGAMEINPUT::HGAMEKEY::KeyCheck() 
{
	for (size_t i = 0; i < m_KeyVec.size(); i++)
	{
		// 'A' 'B' 
		if (0 == GetAsyncKeyState(m_KeyVec[i]))
		{
			// 하나라도 안눌렸다
			return false;
		}
	}

	// 전부다 눌렸다.
	return true;
}

// 여기서 키 체크를 한다.
void HGAMEINPUT::HGAMEKEY::Update()
{
	// 그럼 여기로 오겠죠?
	// 키가 다 눌렸는데?
	if (KeyCheck())
	{
		// 처음 눌렸다.
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
		// 이전프레임에 눌렸다 계속 눌리고 있었다.
		}
	}
	else 
	{
		// 이전까지 눌리고 있었다.
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
			// 이전프레임에 눌렸다 계속 눌리고 있었다.
		}
	}


}


void HGAMEINPUT::Update() 
{
	
	Game_Vector Pos = MousePos3D();
	// 현재위치 빼기 이전위치
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

// 방향과 크기를 동시에 알고 싶을때.
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