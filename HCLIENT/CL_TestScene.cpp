#include "CL_TestScene.h"
#include "CL_Scene.h"

#include <Chess_player.h>
/////////////////////////////////////// STATIC


CL_TestScene* CL_TestScene::Inst;
HBoneAnimationCom* CL_TestScene::AniCom = nullptr;

Game_Ptr<HCAM> CamPtr;

CL_TestScene::CL_TestScene()
{
	Inst = this;
}
CL_TestScene::~CL_TestScene()
{

}

void CL_TestScene::SceneChangeStart()
{
	//SCENE()->ColLink((int)Col_Name::Unit, (int)Col_Name::End);
	SCENE()->ColLink((int)Col_Name::Shop_Unit, (int)Col_Name::Mouse);
	SCENE()->ColLink((int)Col_Name::Button, (int)Col_Name::Mouse);
	SCENE()->ColLink((int)Col_Name::Board, (int)Col_Name::Mouse);

	//Load_Functions();

	//기본 캠 생성
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		CamActor = NewActor;
		NewActor->TRANS()->LPOS({ 0, 0, -10 });
		Game_Ptr<HCAM> NewCam = NewActor->CreateCom<HCAM>(0, (int)RenderType::Default);
		NewCam->MODE(CAMMODE::PERS);
		NewCam->CAMSIZE({ 12.8f, 7.2f });
		m_LineFilter = NewCam->AddFilter<HOutLineFilter>(10);
		m_BloomFilter = NewCam->AddFilter<HBloomFilter>(10);
		NewActor->CreateCom<FreeCam>(10.0f);

		//CamActor->TRANS()->LPOS({ 3.5f, 9.f, 0.5f });
		CamActor->TRANS()->LPOS({ 3.5f, 9.f, 2.4f });
		CamActor->TRANS()->LROT({ 90.f, 0.f, 0.f });
	}

	//UI 캠 생성
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		CamActor_Ui = NewActor;
		NewActor->TRANS()->LPOS({ 0, 0, -9 });
		Game_Ptr<HCAM> NewCam = NewActor->CreateCom<HCAM>(4448, (int)RenderType::Ui);
		NewCam->MODE(CAMMODE::ORTH);
		NewCam->CAMSIZE({ 12.80f, 7.20f });
		//m_LineFilter_Ui = NewCam->AddFilter<HOutLineFilter>(10);
		//m_BloomFilter_Ui = NewCam->AddFilter<HBloomFilter>(10);
	}


	size_t Size = sizeof(ObjectLightData);
	//스카이박스
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		Game_Ptr<Game_Renderer> NewRender = NewActor->CreateCom<Game_Renderer>(L"SPHERE", L"SKY", (int)RenderType::Default);
		NewRender->GetRenderPlayer(0)->RenderOption.IsDifTexture = true;
		NewRender->TEXTURE(L"Tex", L"SKY.jpg");

		NewActor->TRANS()->LPOS({ 0.0F, 0.0F, 0.0F });
		NewActor->TRANS()->LSCALE({ 1000.0F, 1000.0F, 1000.0F });
	}

	////공
	//{
	//	float4 Test = float4(0.5f, 0.5f, 0.5f, 0.9f);
	//	Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
	//	Game_Ptr<Game_Renderer> NewRender = NewActor->CreateCom<Game_Renderer>(L"SPHERE", L"Defferd");
	//	NewActor->TRANS()->LPOS({ -2.0F, 2.0F, 2.0F });
	//	NewRender->ShadowOn();
	//}
	////공+범프
	//{
	//	Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
	//	Game_Ptr<Game_Renderer> NewRender = NewActor->CreateCom<Game_Renderer>(L"SPHERE", L"Foward");
	//	NewRender->GetRenderPlayer(0)->RenderOption.IsDifTexture = false;
	//	NewRender->GetRenderPlayer(0)->RenderOption.IsNormalTexture = true;
	//	NewRender->TEXTURE(L"NormalTexture", L"BumpTest.png");
	//	NewActor->TRANS()->LPOS({ 2.0F, 2.0F, 2.0F });
	//}

	//맵
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		// 이녀석이 y 레티오가 되준다.
		NewActor->TRANS()->LSCALE({ 8.0f, 1.0f, 10.0f }); //100 20 100
		NewActor->TRANS()->LPOS({ 3.5f, 0.f , 2.5f });
		Map = NewActor->CreateCom<HHeightMap>(8, 10, L"MAPH1.png", (int)RenderType::Default); // 3131
		Map->BaseTexture(L"Tile3.png");
		Map->Render()->ShadowOn();
	}

	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		Game_Ptr<Game_Renderer> Ui_Black = NewActor->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		Ui_Black->TRANS()->LPOS({ 3.5f, 0.01f , 2.5f });
		Ui_Black->TRANS()->LSCALE({ 8.f, 10.f, 1.f });
		Ui_Black->TRANS()->LROT({ 90.f,0.f, 0.f });
		float4 CUTDATA;
		CUTDATA = { 0,0,1,1 };
		Ui_Black->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR;
		DRAWCOLOR = { 1,1,1,1 }; //50%반투명
		Ui_Black->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);

		Ui_Black->TEXTURE(L"Tex", L"ChessBoard.png");
		Ui_Black->SAMPLER(L"Smp", "LWSMP");
	}

	//땅 출력
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		Game_Ptr<Game_Renderer>  NewRenderer = NewActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		Game_Ptr<Game_StaticFBX> TestFbx;
		TestFbx->CreateStaticMesh(NewRenderer, L"Grass.fbx");
		NewActor->TRANS()->LPOS({ 0.f, -0.1f, 7.f });
		NewActor->TRANS()->WSCALE({ 0.01F, 0.01F, 0.01F });
	}

	////test
	//{
	//	PlayerActor = SCENE()->CreateActor();
	//	PlayerActor->TRANS()->WPOS({ 0.03f, 0.10f , 0.0f });
	//	Game_Ptr<Game_Actor> MeshActor = SCENE()->CreateActor();
	//	MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
	//	MeshActor->TRANS()->PARENT(PlayerActor->TRANS());
	//	Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>();
	//	Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
	//	std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"006S1_Idle.fbx", L"Defferd", 0);
	//	// std::vector<HPTR<HRENDERPLAYER>> Ptr2 = NewPtr->MainFbx(L"AnimMan.FBX", L"OUTLINE", 0);
	//	// NewPtr->FbxToPartRenderPlayer(L"AnimMan.FBX", L"OUTLINE", 0, 0, 0, 2);
	//	//NewPtr->FbxToPartRenderPlayer(L"006S1_Idle.FBX", L"Emission", 0, 0, 0, 2);
	//	NewPtr->CreateAni(L"006S1_Run.fbx", L"Test1", 0, 17, 0);
	//	NewPtr->CreateAni(L"006S1_Idle.fbx", L"Test", 0, 41, 0);
	//	NewPtr->ChangeAni(L"Test");
	//	//NewPtr->ChangeAni(L"Test1");
	//	NewRender->ShadowOn();
	//}

	//플레이어 생성
	{
		Game_Ptr<Game_Actor> PTR = SCENE()->CreateActor();
		PTR->CreateCom<Chess_player>();
	}

	//빛
	{
		LightTest = SCENE()->CreateActor();

		float4 Dir = Game_Vector(0.0f, 1.0f, -1.0f);
		Dir *= 5.0f;

		LightCom = LightTest->CreateCom<HLight>((int)RenderType::Default);
		Game_Ptr<Game_Renderer> NewRender = LightTest->CreateCom<Game_Renderer>(L"DIRMESH", L"DebugMesh");
		LightTest->TRANS()->WPOS(Dir);
		LightTest->TRANS()->WROTADDX(45.0f);
	}
}
void CL_TestScene::SceneChangeEnd()
{
	SCENE()->ActorClear();
}
void CL_TestScene::Init()
{

}

void CL_TestScene::Update()
{
	StartTime -= Game_Time::DeltaTime();

	if (StartTime <= 0.0f && Start == false)
	{
		HGAMESOUND::Play(L"BattleBGM", L"bgm_Battle_1.wav");
		Start = true;
	}

	if (!HGAMESOUND::IsPlay(L"BattleBGM"))
	{
		HGAMESOUND::Play(L"BattleBGM", L"bgm_Battle_1.wav");
	}

	PlayerUpdate();

	// H3DDEBUG::DrawDebugText(L"AAAA");
	//float Ratio = 2.0f;
	//Game_Vector Scale = { 128 * Ratio, 72 * Ratio };
	//H3DDEBUG::DrawDebugTexture(CamPtr->GbufferTarget()->Texture(0), Scale, Game_Vector{ (640.0f - Scale.x), 360, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(CamPtr->GbufferTarget()->Texture(1), Scale, Game_Vector{ (640.0f - Scale.x), 360 - Scale.y, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(CamPtr->GbufferTarget()->Texture(2), Scale, Game_Vector{ (640.0f - Scale.x), 360 - Scale.y * 2, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(CamPtr->GbufferTarget()->Texture(3), Scale, Game_Vector{ (640.0f - Scale.x), 360 - Scale.y * 3, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(CamPtr->GbufferTarget()->Texture(7), Scale, Game_Vector{ (640.0f - Scale.x), 360 - Scale.y * 4, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(LightCom->ShadowTarget()->Texture(0), Game_Vector{ Scale.x, Scale.x }, Game_Vector{ (640.0f - Scale.x * 2.0f), (360 - Scale.y * 4) + Scale.x - Scale.y, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(CamPtr->GbufferTarget()->Texture(6), Scale, Game_Vector{ (640.0f - Scale.x * 2.0F), 360 - Scale.y * 3, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(m_BloomFilter->OutTarget->Texture(0), Scale, Game_Vector{ (640.0f - Scale.x * 2.0f), 360, 0.0f }, Game_Vector::BLACK);
	//H3DDEBUG::DrawDebugTexture(CamPtr->DefferdLightTarget()->Texture(3), Scale, Game_Vector{ (640.0f - Scale.x * 2.0f), 360 - Scale.y * 1, 0.0f }, Game_Vector::BLACK);

	//Game_RightView::View->PushFbxList();

	//if (nullptr == AniCom)
	//{
	//	return;
	//}

	//if (AniCom->CurAni()->IsEnd())
	//{
	//	CString Text;

	//	Game_RightView::View->m_PlayBtn.GetWindowTextW(Text);

	//	if (Text != L"다시 재생")
	//	{
	//		Game_RightView::View->m_PlayBtn.SetWindowTextW(L"다시 재생");
	//	}
	//	return;
	//}

	//Game_RightView::View->CurFrame(AniCom->CurAni()->m_CurFrame);
}

void CL_TestScene::LoadAni(Game_String _Name)
{
	////유닛
	{
		Game_Ptr<Game_Actor> PlayerActor = SCENE()->CreateActor();
		PlayerActor->TRANS()->WPOS({ 5.0f, 5.0f , 0.0f });

		Game_Ptr<Game_Actor> MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->PARENT(PlayerActor->TRANS());

		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(_Name, L"3DANIDefferdTexture", 0);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(_Name);
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][1].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		//애니 로드.
		NewPtr->CreateAni(_Name, L"Test", -1, -1, 0);
		NewPtr->ChangeAni(L"Test");

		NewRender->ShadowOn();

		MeshActor->Off();
		PlayerActor->Death();
		MeshActor->Death();
	}
}

void CL_TestScene::Load_Functions()
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
}

void CL_TestScene::PlayerUpdate()
{
	//if (HGAMEINPUT::Press(L"L"))
	//{
	//	PlayerActor->TRANS()->LROTADDY(360.0f * Game_Time::DeltaTime());

	//	// PlayerActor->TRANS()->WMOVE(PlayerActor->TRANS()->WLEFT());
	//}

	//if (HGAMEINPUT::Press(L"R"))
	//{
	//	PlayerActor->TRANS()->LROTADDY(-360.0f * Game_Time::DeltaTime());
	//	// PlayerActor->TRANS()->WMOVE(PlayerActor->TRANS()->WRIGHT());
	//}

	//if (HGAMEINPUT::Press(L"F"))
	//{
	//	PlayerActor->TRANS()->WMOVE(PlayerActor->TRANS()->WFORWARD() * Game_Time::DeltaTime(20.0F));
	//}

	//if (HGAMEINPUT::Press(L"B"))
	//{

	//	PlayerActor->TRANS()->WMOVE(PlayerActor->TRANS()->WBACK() * Game_Time::DeltaTime(20.0F));
	//}

	Game_3D_Debug::DrawDebugText(L"캠 Lpos %f %f %f", CamActor->TRANS()->LPOS().x, CamActor->TRANS()->LPOS().y, CamActor->TRANS()->LPOS().z);
	Game_3D_Debug::DrawDebugText(L"캠 Lrot %f %f %f", CamActor->TRANS()->LROT().x, CamActor->TRANS()->LROT().y, CamActor->TRANS()->LROT().z);
	//Game_3D_Debug::DrawDebugText(L"마우스L %f %f %f", HGAMEINPUT::MousePos3D().x, HGAMEINPUT::MousePos3D().y, HGAMEINPUT::MousePos3D().z);

	//Game_Vector Pos = PlayerActor->TRANS()->WPOS();
	//Pos.y = Map->YHeight(PlayerActor->TRANS()->WPOS());
	//PlayerActor->TRANS()->WPOS(Pos);
}