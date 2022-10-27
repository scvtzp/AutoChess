#include <Windows.h>
#include <HGAMESTRING.h>
#include <HGAMEWINDOW.h>
#include <Game_Device.h>
#include <HGAMESCENE.h>
#include <HGAMEACTOR.h>

#include <HGAMEINPUT.h>
#include <HGAMESOUND.h>

//Scene
#include "CL_TestScene.h"
#include "CL_LobbyScene.h"
#include "CL_LoadScene.h"
#include "CL_StageScene.h"

#pragma comment(lib, "HGAMEBASE.lib")
#pragma comment(lib, "HGAMEENGINE.lib")
#pragma comment(lib, "HGAMELOGIC.lib")

int __stdcall Start()
{
	srand((unsigned int)time(NULL));

	//�ܼ� ����
	//HGAMEDEBUG::OpenConsole();

	//key �ε�
	HGAMEINPUT::Init();
	
	//sound �ε�
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"SOUND");

		auto FileList = Dic.DirAllFile();
		for (auto& _File : FileList)
		{
			HGAMESOUND::Load(_File);
		}
	}

	//HGAMESCENE::Create<HCLIENTSCENECOM>(L"TEST");
	HGAMESCENE::Create<CL_LoadScene>(L"LoadScene"); //�ε���
	HGAMESCENE::Create<CL_LobbyScene>(L"LobbyScene"); //�κ� ȭ��
	HGAMESCENE::Create<CL_StageScene>(L"StageScene"); //���� ���� ��
	HGAMESCENE::Create<CL_TestScene>(L"TestScene"); //�浹ó�� �� ���� ����� ��
	HGAMESCENE::ChangeScene(L"LoadScene");

	return 1; 
}

// ����Ÿ��
int __stdcall Loop()
{
	HGAMESCENE::Progress();
	return 1;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// HGMAESTRING TTT = L"cccccc, dddddd, ewersda, vcxzvxzc , asdfaef,";
	// List<Game_String>
	//                   "FAASDF" + 1.0F
	//                   "FAASDF1.0" + 1.0F
	//                   "FAASDF " + 1.0F
	// HGMAESTRING TTT = HGAMESTR1 + float;
	// HGMAESTRING TTT = HGAMESTR2 + int;
	// HGMAESTRING TTT = "adfasdfwerfd";
	// List<Game_String>

// #ifde
	new int();

	HGAMEWINDOW::Create(L"MainWindow");
	HGAMEWINDOW::MAINOBJ()->Size({ 1280, 720 });
	Game_Device::Create(L"MainWindow");

	// Game_Ptr<SPRITERENDER> AAA;
	// AAA->SetSprite("....png");


	// �Ʒ��� �������� �ϳ��� ��Ʈ�� ����.
	// �����̶�� ���׸��� ��Ƽ����

	// 2DIMAGE_BACK ���׸���
	// ���ؽ����̴� VS_2D_IMAGE
	// �ȼ����̴� PS_2D_IMAGE
	// �������ٽ� DEFDEPTH
	// ���� ���ĺ���
	// �����Ͷ����� FRONT


	// 2DIMAGE_FRONT ���׸���
	// ���ؽ����̴� VS_2D_IMAGE
	// �ȼ����̴� PS_2D_IMAGE
	// �������ٽ� DEFDEPTH
	// ���� ���ĺ���
	// �����Ͷ����� FRONT

	// �������� �迭�̶�� ���� �ȴ�.
	// Game_Ptr<RENDER> CCC;
	// AAA->SETMAT("2DIMAGE");
	// AAA->SETTEXTURE("DIFTEX", ".png");
	// AAA->SAMPLER("SMP", ".png");
	// AAA->CBUFFER("TRANSDATA", );

	// AAA->CBUFFER("UVCOLOR", ".png");
	// AAA->MESH("UVCOLOR", ".png");

	// AAA->Texture("UVCOLOR", ".png");

	HGAMEWINDOW::HWINMESSAGE(Start, Loop);

}
