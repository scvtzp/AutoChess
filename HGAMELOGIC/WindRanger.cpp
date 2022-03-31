#include "WindRanger.h"
#include "Chess_Base.h"
#include <Game_3D_Debug.h>
#include "Chess_player.h"

/*
Attack01		[0,43]
Attack02		[44,87]
Born		[88,134]
Death		[135,222]
Dizzy		[223,271]
Idle		[272,322]
Jump		[323,373]
Run		[374,398]
Skill01		[399,492]
Victory		[493,575]
*/

WindRanger::WindRanger() 
{
	Set_Info(1);
}

WindRanger::WindRanger(int Star = 1) 
{
	Set_Info(Star); 
}

WindRanger::~WindRanger() {}

void WindRanger::Init()
{
	//출력
	{
		MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });
	
		Game_Ptr<Game_Renderer> NewRender = ACTOR()->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = ACTOR()->CreateCom<Game_BoneAnimationCom_Ex>();
		NewPtr->TRANS()->LSCALE({ 0.01f, 0.01f , 0.01f });
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"WindRangerS1.FBX", L"3DANIDefferdTexture", (int)RenderType::Default);
	
		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"WindRangerS1.FBX");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");
	
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Attack01", 0, 43, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Attack02", 44, 87, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Born", 88, 134, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Death", 135, 222, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Dizzy", 223, 271, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Idle", 272, 322, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Jump", 323, 373, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Run", 374, 398, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Skill01", 399, 492, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Victory", 493, 575, 0);
	
		NewPtr->ChangeAni(L"Idle");
	
		NewRender->ShadowOn();
	}

	//ui검정 뒷배경 생성
	{
		Game_Ptr<Game_Actor> Act = SCENE()->CreateActor();
		HP_Bar = Act->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		HP_Bar->TRANS()->LPOS({ 0.f, 0.f, 0.f });
		HP_Bar->TRANS()->LSCALE({ 0.8f, 0.15f, 1.f });
		HP_Bar->TRANS()->LROT({ 90.f, 0.f, 0.f });

		float4 CUTDATA;
		CUTDATA = { 0,0,1,1 };
		HP_Bar->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR;
		DRAWCOLOR = { 1,1,1,0.5f }; //50%반투명
		HP_Bar->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);

		HP_Bar->TEXTURE(L"Tex", L"Red.png");
		HP_Bar->SAMPLER(L"Smp", "LWSMP");
	}
	//ui빨강 뒷배경 생성
	{
		Game_Ptr<Game_Actor> Act1 = SCENE()->CreateActor();
		HP_BlackBar = Act1->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		HP_BlackBar->TRANS()->LPOS({ 0.f, 0.f, 0.f });
		HP_BlackBar->TRANS()->LSCALE({ 0.8f, 0.15f, 1.f });
		HP_BlackBar->TRANS()->LROT({ 90.f, 0.f, 0.f });

		float4 CUTDATA;
		CUTDATA = { 0,0,1,1 };
		HP_BlackBar->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		float4 DRAWCOLOR;
		DRAWCOLOR = { 1,1,1,0.5f }; //50%반투명
		HP_BlackBar->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);

		HP_BlackBar->TEXTURE(L"Tex", L"Black.png");
		HP_BlackBar->SAMPLER(L"Smp", "LWSMP");
	}
}



void WindRanger::Update()
{
	//if(!UpCheck)
	//	Fbx_Init();

	NewPtr->TRANS()->LPOS({ Info.Real_X, 0.f , Info.Real_Y });
	NewPtr->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
	NewPtr->TRANS()->LSCALE({ 0.01f, 0.01f , 0.01f });

	if (!Chess_player::Round)
	{
		HP_Bar->Off();
		HP_BlackBar->Off();
		return;
	}

	Death_Check();

	float Hp_Scale = Info.Hp / Info.MaxHp * 0.8f;
	HP_Bar->TRANS()->LSCALE({ Hp_Scale , 0.15f, 1.f });

	//Check_Death();
	IsBanch();

	//이동 (밴치에 있으면 패스)
	if (!Info.Banch)
	{
		HP_Bar->On();
		HP_BlackBar->On();

		Move();
		HP_Bar->TRANS()->LPOS({ Info.Real_X - (0.8f-Hp_Scale)/2, 1.f, Info.Real_Y });
		HP_BlackBar->TRANS()->LPOS({ Info.Real_X, 1.f, Info.Real_Y });
	}
	else
	{
		HP_Bar->Off(); 
		HP_BlackBar->Off();
	}

	Skile();

	switch (Info.State)
	{
	case Chess_State::Attack_1:
		NewPtr->ChangeAni(L"Attack01");
		break;
	case Chess_State::Attack_2:
		NewPtr->ChangeAni(L"Attack02");
		break;
	case Chess_State::Born:
		NewPtr->ChangeAni(L"Born");
		break;
	case Chess_State::Death:
		NewPtr->ChangeAni(L"Death");
		break;
	case Chess_State::Dizzy:
		NewPtr->ChangeAni(L"Dizzy");
		break;
	case Chess_State::Idle:
		NewPtr->ChangeAni(L"Idle");
		break;
	case Chess_State::Jump:
		NewPtr->ChangeAni(L"Jump");
		break;
	case Chess_State::Run:
		NewPtr->ChangeAni(L"Run");
		break;
	case Chess_State::Skill01:
		NewPtr->ChangeAni(L"Skill01");
		break;
	case Chess_State::Victory:
		NewPtr->ChangeAni(L"Victory");
		break;
	default:
		NewPtr->ChangeAni(L"Idle");
		break;
	}

}

void WindRanger::Set_Info(int Star = 1)
{
	Info.Name = "WindRanger";

	Info.RaceName1 = Chess_Race::End;
	Info.RaceName2 = Chess_Race::End;

	Info.ClassName = Chess_Class::End;

	Info.Cost = 1; //가격
	Info.Star = Star; //등급

	Info.Hp = 450; //체력
	Info.MaxHp = Info.Hp; //체력
	//Info.Mana = 0; //마나
	//Info.MaxMana = 100; //마나

	Info.Atk = 44; //공
	Info.Atk_Rate = 1.3f; //공속
	Info.Range = 5.7f; //루트32 //사거리
	Info.Hp_Regeneration = 0; //체젠
	Info.Armor = 5; //방어력
	Info.Magic_Resistance = 0; //마방

	Info.Damage_To_Player = 1; //플레이어에게 데미지.

	Info.Position_X = 0;
	Info.Position_Y = 0;

	Info.Real_X = Info.Position_X;
	Info.Real_Y = Info.Position_Y;
}

void WindRanger::Skile()
{


}

void WindRanger::Fbx_Init()
{
	if(Info.Star == 1)
	//출력
	{
		//MeshActor = SCENE()->CreateActor();
		//MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		//MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		//MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = ACTOR()->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = ACTOR()->CreateCom<Game_BoneAnimationCom_Ex>();
		NewPtr->TRANS()->LSCALE({ 0.01f, 0.01f , 0.01f });
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"WindRangerS1.FBX", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"WindRangerS1.FBX");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Attack01", 0, 43, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Attack02", 44, 87, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Born", 88, 134, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Death", 135, 222, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Dizzy", 223, 271, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Idle", 272, 322, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Jump", 323, 373, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Run", 374, 398, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Skill01", 399, 492, 0);
		NewPtr->CreateAni(L"WindRangerS1.FBX", L"Victory", 493, 575, 0);

		NewPtr->ChangeAni(L"Idle");

		NewRender->ShadowOn();
	}
	else if (Info.Star == 2)
	{
		Game_Ptr<Game_Renderer> NewRender = ACTOR()->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = ACTOR()->CreateCom<Game_BoneAnimationCom_Ex>();
		NewPtr->TRANS()->LSCALE({ 0.01f, 0.01f , 0.01f });
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"WarewolfS1.FBX", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"WarewolfS1.FBX");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		NewPtr->CreateAni(L"WarewolfS1.FBX", L"Attack01", 0, 33, 0);
		NewPtr->CreateAni(L"WarewolfS1_Run.fbx", L"Run", 0, 21, 0);
		NewPtr->CreateAni(L"WarewolfS1_Idle.fbx", L"Idle", 0, 50, 0);
		NewPtr->CreateAni(L"WarewolfS1_Born.fbx", L"Born", 0, 45, 0);
		NewPtr->CreateAni(L"WarewolfS1_Death.fbx", L"Death", 0, 71, 0);
		NewPtr->CreateAni(L"WarewolfS1_Dizzy.fbx", L"Dizzy", 0, 92, 0);
		NewPtr->CreateAni(L"WareWolfS1_Skill01.fbx", L"Skill01", 0, 40, 0);
		NewPtr->CreateAni(L"WarewolfS1_Victory.fbx", L"Victory", 0, 97, 0);

		NewPtr->ChangeAni(L"Attack01");

		NewRender->ShadowOn();
	}

	UpCheck = true;
}

void WindRanger::Skill_Init()
{
}

void WindRanger::Skill_Update()
{
}
