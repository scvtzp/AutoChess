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

void WindRanger::Init()
{
	//출력
	{
		MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });
	
		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
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

	Make_HpBar();
}


//void WindRanger::Update()
//{
//	Base_Update();
//
//	Skile();
//}

void WindRanger::Skile()
{


}

void WindRanger::Fbx_Init()
{
}

void WindRanger::Skill_Init()
{
}

void WindRanger::Skill_Update()
{
}
