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

	//콘솔 오픈
	//HGAMEDEBUG::OpenConsole();

	//key 로드
	HGAMEINPUT::Init();
	
	//sound 로드
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
	HGAMESCENE::Create<CL_LoadScene>(L"LoadScene"); //로딩씬
	HGAMESCENE::Create<CL_LobbyScene>(L"LobbyScene"); //로비 화면
	HGAMESCENE::Create<CL_StageScene>(L"StageScene"); //메인 전투 씬
	HGAMESCENE::Create<CL_TestScene>(L"TestScene"); //충돌처리 등 각종 실험용 씬
	HGAMESCENE::ChangeScene(L"LoadScene");

	return 1; 
}

// 데드타임
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


	// 아래의 정보들을 하나의 세트로 보고.
	// 재질이라고 메테리얼 머티리얼

	// 2DIMAGE_BACK 메테리얼
	// 버텍스쉐이더 VS_2D_IMAGE
	// 픽셀쉐이더 PS_2D_IMAGE
	// 뎁스스텐실 DEFDEPTH
	// 블랜드 알파블랜드
	// 래스터라이저 FRONT


	// 2DIMAGE_FRONT 메테리얼
	// 버텍스쉐이더 VS_2D_IMAGE
	// 픽셀쉐이더 PS_2D_IMAGE
	// 뎁스스텐실 DEFDEPTH
	// 블랜드 알파블랜드
	// 래스터라이저 FRONT

	// 랜더링의 계열이라고 보면 된다.
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
