#include "CL_LobbyScene.h"
#include "CL_Scene.h"

CL_LobbyScene::CL_LobbyScene() {}

CL_LobbyScene::~CL_LobbyScene() {}

void CL_LobbyScene::SceneChangeStart()
{
	SCENE()->ColLink((int)Col_Name::Shop_Unit, (int)Col_Name::Mouse);
	SCENE()->ColLink((int)Col_Name::Button, (int)Col_Name::Mouse);
	SCENE()->ColLink((int)Col_Name::Board, (int)Col_Name::Mouse);

	Load_Functions();


	//기본 캠 생성
	{
		Game_Ptr<Game_Actor> NewActor = SCENE()->CreateActor();
		CamActor = NewActor;
		NewActor->TRANS()->LPOS({ 0, 0, -8 });
		Game_Ptr<HCAM> NewCam = NewActor->CreateCom<HCAM>(0, (int)RenderType::Default);
		NewCam->MODE(CAMMODE::PERS);
		NewCam->CAMSIZE({ 12.8f, 7.2f });
		m_LineFilter = NewCam->AddFilter<HOutLineFilter>(10);
		m_BloomFilter = NewCam->AddFilter<HBloomFilter>(10);
		//NewActor->CreateCom<FreeCam>(10.0f);

		CamActor->TRANS()->LPOS({ 0.f, 0.f, -3.15f });
		CamActor->TRANS()->LROT({ 0.f, 0.f, 0.f });
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


	//마우스
	{
		Game_Ptr<Game_Actor> Mouse_Actor = SCENE()->CreateActor();
		Mouse_Actor->TRANS()->LSCALE({ 1.1f, 1.1f, 1.f });
		Mouse_Col = Mouse_Actor->CreateCom<Game_Collision>((int)Col_Name::Mouse);
		Mouse_Col->ColType(COLTYPE::AABB2D);
		//Mouse_Col->TRANS()->LSCALE(Mouse_Scale);

		Mouse_ColRenderer = Mouse_Actor->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Ui);
		float4 CUTDATA = { 0,0,1,1 };
		Mouse_ColRenderer->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR = { 1,1,1,1 };
		Mouse_ColRenderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Mouse_ColRenderer->TEXTURE(L"Tex", L"ColTest.png");
		Mouse_ColRenderer->SAMPLER(L"Smp", "LWSMP");
		//Mouse_ColRenderer->TRANS()->LSCALE(Mouse_Scale);
	}
	//배경
	{
		Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		Act->TRANS()->WSCALE({ 12.8f, 7.2f, 1.f });
		Act->TRANS()->WPOS({ 0.f, 0.f, 3.f });
		Game_Ptr<Game_Renderer> Mouse_ColRenderer = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		float4 CUTDATA = { 0,0,1,1 };
		Mouse_ColRenderer->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR = { 1,1,1,1 };
		Mouse_ColRenderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Mouse_ColRenderer->TEXTURE(L"Tex", L"Lobby.png");
		Mouse_ColRenderer->SAMPLER(L"Smp", "LWSMP");
	}

	//캐릭터
	{
		Game_Ptr<Game_Actor> MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.015f, 0.015f , 0.015f });
		MeshActor->TRANS()->WPOS({ 0.00f, -2.0f , 2.0f });
		MeshActor->TRANS()->WROT({ -90.0f, 180.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"Player_Born.fbx", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"Player_Born.fbx");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		NewPtr->CreateAni(L"Player_Born.fbx", L"Born", 0, 156, 0);
		NewPtr->CreateAni(L"Player_Idle.fbx", L"Idle", 0, 160, 0);

		NewPtr->ChangeAni(L"Idle");

		NewRender->ShadowOn();

		//MeshActor->Off();
	}

}

void CL_LobbyScene::SceneChangeEnd()
{
	SCENE()->ActorClear();
}

void CL_LobbyScene::Init()
{
}

void CL_LobbyScene::Update()
{
	Mouse_Col->TRANS()->LPOS({ HGAMEINPUT::MousePos3D().x / 100.f, HGAMEINPUT::MousePos3D().y / 100.f, HGAMEINPUT::MousePos3D().z / 100.f });
	Mouse_ColRenderer->TRANS()->LPOS({ HGAMEINPUT::MousePos3D().x / 100, HGAMEINPUT::MousePos3D().y / 100, HGAMEINPUT::MousePos3D().z / 100 });
	//Game_3D_Debug::DrawDebugText(L"캠 Lpos %f %f %f", CamActor->TRANS()->LPOS().x, CamActor->TRANS()->LPOS().y, CamActor->TRANS()->LPOS().z);

	if (HGAMEINPUT::Down(L"1"))
	{
		NewPtr->ChangeAni(L"Idle");
	}
	else if (HGAMEINPUT::Down(L"2"))
	{
		NewPtr->ChangeAni(L"Born");
	}
}

void CL_LobbyScene::Load_Functions()
{
	//FBX로드
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"MESH");
		Dic.Move(L"Unit");
		Dic.Move(L"Player");

		auto FileList = Dic.DirAllFile();
		Game_Fbx_Ex::Load(Dic.PlusFileName(L"Player_Born.fbx"));
		Game_Fbx_Ex::Load(Dic.PlusFileName(L"Player_Idle.fbx"));
	}

	//TEX로드
	{
		HGAMEDIRECTORY Dic;

		Dic.MoveParent(L"AutoChess");
		Dic.Move(L"RES");
		Dic.Move(L"TEXTURE");

		auto FileList = Dic.DirAllFile();
		Game_Sprite::Create(L"SwordMaster.png", 8, 5);
		Game_Sprite::Create(L"WindRanger.png", 8, 7);
		Game_Sprite::Create(L"MouseDef.png", 4, 1);
		Game_Sprite::Create(L"Black.png");
		Game_Sprite::Create(L"Red.png");
		Game_Sprite::Create(L"Lobby.png");
	}
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


}