#include "CL_LoadScene.h"
#include "CL_Scene.h"
#include <thread>
#include <vector>
#include <Game_2DAnimation.h>
#include <Unit_Header.h>

//잠깐 100으로 바꿈
#define LoadSceneNum 1

CL_LoadScene::CL_LoadScene()
{
}

CL_LoadScene::~CL_LoadScene()
{
}

void CL_LoadScene::SceneChangeStart()
{
	//UI 캠 생성
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		Game_Ptr<Game_Actor> CamActor_Ui = NewActor;
		NewActor->TRANS()->LPOS({ 0, 0, -9 });
		Game_Ptr<HCAM> NewCam = NewActor->CreateCom<HCAM>(4448, (int)RenderType::Ui);
		NewCam->MODE(CAMMODE::ORTH);
		NewCam->CAMSIZE({ 12.80f, 7.20f });
		//m_LineFilter_Ui = NewCam->AddFilter<HOutLineFilter>(10);
		//m_BloomFilter_Ui = NewCam->AddFilter<HBloomFilter>(10);
	}

	//로딩에 사용할 리소스 먼저 스레드 사용 안하고 로드.
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");
		Dic.Move(L"Load");
		auto FileList = Dic.DirAllFile();
		for (auto& _File : FileList)
		{
			HTEXTURE::Load(_File);
		}

		//로딩바는 설치 안함.
		Game_Sprite::Create(L"LoadingSceneSprite.png", 15, 4); //60개
		Game_Sprite::Create(L"LoadingCircle.png", 4, 3); //9개
	}

	//배경화면 일단 애니메이션 재생.
	{
		float4 DRAWCOLOR = { 1,1,1,1 };

		Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		Act->TRANS()->WSCALE({ 12.8f, 7.2f, 1.f });
		Act->TRANS()->WPOS({ 0.f, 0.f, 3.f });
		Game_Ptr<Game_Renderer> Renderer = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		Game_Ptr<Game_2DAnimation> Ani = Act->CreateCom<Game_2DAnimation>(Renderer);
		Ani->CreateAni(L"Load_Background", L"LoadingSceneSprite.png", 0, 59, 0.1f, true);
		Ani->ChangeAni(L"Load_Background");
		Renderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Renderer->TEXTURE(L"Tex", L"LoadingSceneSprite.png");
		Renderer->SAMPLER(L"Smp", "LWSMP");
	}

	std::thread th_LoadFnc(&CL_LoadScene::Load_Functions, this);
	th_LoadFnc.detach();

	//for (int i = 0; i < LoadSceneNum; ++i)
	//{
	//	//스레드 생성.
	//	std::thread th1(&CL_LoadScene::Load, this, i);

	//	//스레드 종료 조건 설정-detach : 종료를 기다리지 않습니다.
	//	//종료 기다리려면 .join();
	//	th1.detach();
	//}
}

void CL_LoadScene::SceneChangeEnd()
{
	SCENE()->ActorClear();
}

void CL_LoadScene::Init()
{
}

void CL_LoadScene::Update()
{
	//1차 로드 이후 본격적인 다중 스레드 사용하는 애니메이션 로드.(하나씩 미리 만들기)
	if (First_LoadCheck == true)
	{
		//여까지 37초 소요됨
		First_LoadCheck = false; //이후 다시는 true될 일 없음.

		//스레드 생성. 
		for (int i = 0; i < LoadSceneNum; ++i)
		{
			std::thread th1(&CL_LoadScene::Load, this, i);
			th1.detach();
		}
	}

	//스레드 로드가 끝나면 씬 전환. 
	//조건은 뮤텍스 사용한 안전한 증감.
	if(LoadCount >= LoadSceneNum)
		HGAMESCENE::ChangeScene(L"TestScene");
}

//어떤거 로드할건지.
//다 상세 수치가 제각각이라 그냥 때려박음.
void CL_LoadScene::Load(int _Load_Num)
{
	switch (_Load_Num)
	{
	case 0:
	{
		Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
		Game_Ptr<SwordMaster> TestEnemy = PTR1->CreateCom<SwordMaster>();
	}
		break;
	case 1:
	{
		Game_Ptr<Game_Actor> PTR1 = SCENE()->CreateActor();
		Game_Ptr<SoulBreaker> TestEnemy = PTR1->CreateCom<SoulBreaker>();
	}
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	case 11:
		break;
	case 12:
		break;
	case 13:
		break;
	case 14:
		break;
	case 15:
		break;
	case 16:
		break;
	case 17:
		break;
	case 18:
		break;
	case 19:
		break;
	case 20:
		break;
	default:
		break;
	}

	//for (int i = 0; i < 100; ++i)
	//	for (int j = 0; j < 10000; ++j)
	//		cout << _Load_Num << "\n";

	//로드 이후(케이스 문 이후) 카운트 증가.
	//_Count++;
	Mut.lock();
	LoadCount++;
	Mut.unlock();
}

void CL_LoadScene::Load_Functions()
{
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");
		Dic.Move(L"GLOBAL");

		auto FileList = Dic.DirAllFile();

		for (auto& _File : FileList)
		{
			HTEXTURE::Load(_File);
		}
	}
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");

		auto FileList = Dic.DirAllFile();

		for (auto& _File : FileList)
		{
			HTEXTURE::Load(_File);
		}
		for (auto& _File : FileList)
		{
			Game_Sprite::Create(_File.FileName());
		}
	}
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");
		Dic.Move(L"Shop");
		auto FileList = Dic.DirAllFile();
		for (auto& _File : FileList)
		{
			HTEXTURE::Load(_File);
		}

		Game_Sprite::Create(L"SwordMaster.png", 8, 5);
		Game_Sprite::Create(L"WareWolf.png", 8, 7);
		Game_Sprite::Create(L"WindRanger.png", 8, 7);
	}

	//FBX로드
	//소드마스터
	//윈드레인저
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"MESH");
		Dic.Move(L"Unit");

		auto FileList = Dic.DirAllFile();
		Game_Fbx_Ex::Load(Dic.PlusFileName(L"SwordMasterS1.FBX"));
		Game_Fbx_Ex::Load(Dic.PlusFileName(L"WindRangerS1.FBX"));
	}

	//웨어울프
	{
		{
			HGAMEDIRECTORY Dic;

			Dic.MoveParent(L"AutoChess");
			Dic.Move(L"RES");
			Dic.Move(L"MESH");
			Dic.Move(L"Unit");
			Dic.Move(L"WareWolf");

			auto FileList = Dic.DirAllFile();
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1.FBX"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Run.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Idle.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Born.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Death.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Dizzy.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Skill01.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"WareWolfS1_Victory.fbx"));
		}
		{
			HGAMEDIRECTORY Dic;

			Dic.MoveParent(L"AutoChess");
			Dic.Move(L"RES");
			Dic.Move(L"MESH");
			Dic.Move(L"Unit");
			Dic.Move(L"WareWolf");
			Dic.Move(L"S");

			auto FileList = Dic.DirAllFile();
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Attack01.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Attack02.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Born.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Death.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Dizzy.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Idle.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Run.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"SWareWolfS1_Victory.fbx"));
		}
		{
			HGAMEDIRECTORY Dic;

			Dic.MoveParent(L"AutoChess");
			Dic.Move(L"RES");
			Dic.Move(L"MESH");
			Dic.Move(L"Unit");
			Dic.Move(L"WareWolf");
			Dic.Move(L"C");

			auto FileList = Dic.DirAllFile();
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Attack01.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Attack02.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Born.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Death.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Dizzy.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Idle.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Run.fbx"));
			Game_Fbx_Ex::Load(Dic.PlusFileName(L"CWareWolfS1_Victory.fbx"));
		}
	}

	//(암살고블린) 소울브레이커
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"MESH");
		Dic.Move(L"Unit");
		Dic.Move(L"SoulBreaker");

		auto FileList = Dic.DirAllFile();
		for (auto& i : FileList)
		{
			Game_Fbx_Ex::Load(Dic.PlusFileName(i.FileName()));
		}
		//Game_Fbx_Ex::Load(Dic.PlusFileName(L"SwordMasterS1.FBX"));
		//Game_Fbx_Ex::Load(Dic.PlusFileName(L"WindRangerS1.FBX"));
	}

	//유니콘
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"MESH");
		Dic.Move(L"Unit");
		Dic.Move(L"Unicorn");

		auto FileList = Dic.DirAllFile();
		for (auto& i : FileList)
		{
			Game_Fbx_Ex::Load(Dic.PlusFileName(i.FileName()));
		}
	}
	//유니콘 이펙트
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");
		Dic.Move(L"Effect");
		auto FileList = Dic.DirAllFile();
		for (auto& _File : FileList)
		{
			HTEXTURE::Load(_File);
		}

		Game_Sprite::Create(L"Unicorn_Circle.png", 4, 4);
	}

	//Static FBX로드
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"MESH");

		HGAMEFILE NewFile;
		NewFile.SetPath(Dic.PlusFileName(L"Grass.fbx"));
		Game_StaticFBX::Load(NewFile);
	}
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"MESH");

		HGAMEFILE NewFile;
		NewFile.SetPath(Dic.PlusFileName(L"Board.fbx"));
		Game_StaticFBX::Load(NewFile);
	}

	Mut.lock();
	First_LoadCheck = true;
	Mut.unlock();
}
