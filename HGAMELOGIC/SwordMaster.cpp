#include "SwordMaster.h"
#include <Game_3D_Debug.h>
#include "Chess_player.h"
/*
Attack01[0, 33]
Attack02[34, 67]
Born[68, 113]
Death[114, 194]
Dizzy[195, 235]
Idle[236, 276]
Jump[277, 335]
Run[336, 360]
Skill01[361, 376]
01_1[377, 385]
Victory[386, 483]
*/

SwordMaster::SwordMaster()
{
	Set_Info(1);
}

SwordMaster::SwordMaster(int Star = 1)
{
	Set_Info(Star);
}

SwordMaster::~SwordMaster()
{
}

void SwordMaster::Set_Info(int Star = 1)
{
	Info.Name = "SwordMaster";

	Info.RaceName1 = Chess_Race::Orc;
	Info.RaceName2 = Chess_Race::End;
	Info.ClassName = Chess_Class::End;

	Info.Cost = 2; //가격
	Info.Star = Star; //등급

	Info.Hp = 600; //체력

	Info.Atk = 67; //공
	Info.Atk_Rate = 1.1f; //공속
	Info.Range = 1.5f; //사거리
	Info.Hp_Regeneration = 0; //체젠
	Info.Armor = 5; //방어력
	Info.Magic_Resistance = 0; //마방

	Info.Damage_To_Player = 1; //플레이어에게 데미지.
	Info.Position_X = 0;
	Info.Position_Y = 0;

	Info.Real_X = Info.Position_X;
	Info.Real_Y = Info.Position_Y;
}

void SwordMaster::Init()
{
	//출력
	{
		MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"SwordMasterS1.FBX", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"SwordMasterS1.FBX");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][1].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Attack01", 0, 33, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Attack02", 34, 67, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Born", 68, 113, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Death", 114, 194, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Dizzy", 195, 235, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Idle", 236, 276, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Run", 336, 360, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Skill01", 361, 376, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Skill02", 377, 385, 0);
		NewPtr->CreateAni(L"SwordMasterS1.FBX", L"Victory", 386, 483, 0);

		NewPtr->ChangeAni(L"Idle");
		NewRender->ShadowOn();
	}
	Make_HpBar();

}

//void SwordMaster::Update()
//{
//	MeshActor->TRANS()->LPOS({ Info.Real_X, 0.f , Info.Real_Y });
//	MeshActor->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
//	if (!Chess_player::Round)
//		return;
//
//	//Death_Check();
//	
//	/*if (Info.Hp <= 0)
//	{
//		ACTOR()->Death();
//		Info.Death = true;
//		MeshActor->Death();
//	}*/
//
//	if (Info.Position_Y != -2)
//		Info.Banch = false;
//	else
//		Info.Banch = true;
//
//	if (!Info.Banch)
//	{
//		Move();
//		Base_Update();
//	}
//
//	if(Info.Play_Skill)
//		Skill_Update();
//}

void SwordMaster::Skill_Init()
{
}

void SwordMaster::Skill_Update()
{
	bool a = NewPtr->Is_End();
	int b = NewPtr->Get_EndNum();
	int c = NewPtr->Get_Num();

	wcout << L"소드 마스터";
	wcout << b << L" " << c << L" ";
	if (a)
		wcout << L"true";
	else
		wcout << L"false";
	wcout << endl;

	//if (Info.State == Chess_State::Skill01 && NewPtr->Get_Num() >= 374 )
	if (Info.State == Chess_State::Skill01 && NewPtr->Is_End() )
	{
		Info.State = Chess_State::Skill02;
	}
	else if (Info.State == Chess_State::Skill02)
	{
		Info.SkileTime += Game_Time::DeltaTime();
		if (Info.SkileTime < 4.f)
		{
			if (Info.MyUnit)
			{
				for (auto iter : Chess_player::Piece_Enemy_ChessBoard)
				{
					if (iter->Info.Position_X <= Info.Position_X + 1 &&
						iter->Info.Position_X >= Info.Position_X - 1 &&
						iter->Info.Position_Y <= Info.Position_Y + 1 &&
						iter->Info.Position_Y >= Info.Position_Y - 1)
					{
						iter->Info.Hp -= (75 * Game_Time::DeltaTime());
					}
				}
			}
			else
			{
				for (auto iter : Chess_player::Piece_Board)
				{
					if (iter->Info.Position_X <= Info.Position_X + 1 &&
						iter->Info.Position_X >= Info.Position_X - 1 &&
						iter->Info.Position_Y <= Info.Position_Y + 1 &&
						iter->Info.Position_Y >= Info.Position_Y - 1)
					{
						iter->Info.Hp -= (75 * Game_Time::DeltaTime());
					}
				}
			}
		}
		else
		{
			Info.SkileTime = 0;
			Info.Mana = 0;
			Info.Play_Skill = false;
			Info.State = Chess_State::Idle;
		}
	}
}
