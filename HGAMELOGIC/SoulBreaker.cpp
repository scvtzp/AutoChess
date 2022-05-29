#include "SoulBreaker.h"
#include "Chess_player.h"

/*
atk1 33
atk2 41
idle 41
dizzy 61
death 68
born 46
Run 17
skile 41
Jump 49
victory 93
*/

SoulBreaker::SoulBreaker()
{
	Set_Info(1);
}

SoulBreaker::SoulBreaker(int Star = 1)
{
	Set_Info(Star);
}

SoulBreaker::~SoulBreaker()
{
}

void SoulBreaker::Set_Info(int Star = 1)
{	
	Info.Name = "SoulBreaker";

	Info.RaceName1 = Chess_Race::Goblin;
	Info.RaceName2 = Chess_Race::End;
	Info.ClassName = Chess_Class::End;

	Info.Cost = 1; //가격
	Info.Star = Star; //등급

	Info.Hp = 550; //체력
	Info.Mana = 0; //마나

	Info.Atk = 55; //공
	Info.Atk_Rate = 1.1f; //공속
	Info.Range = 1.5f; //사거리
	Info.Hp_Regeneration = 0; //체젠
	Info.Armor = 5; //방어력
	Info.Magic_Resistance = 0; //마방

	Info.Damage_To_Player = 2; //플레이어에게 데미지.
	Info.Position_X = 0;
	Info.Position_Y = 0;

	Info.Real_X = Info.Position_X;
	Info.Real_Y = Info.Position_Y;
}

void SoulBreaker::Init()
{
	//출력
	{
		MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"006S1_Idle.fbx", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"006S1_Idle.fbx");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		NewPtr->CreateAni(L"006S1_Atk1.fbx", L"Attack01", 0, 33, 0);
		NewPtr->CreateAni(L"006S1_Atk2.fbx", L"Attack02", 0, 41, 0);
		NewPtr->CreateAni(L"006S1_Born.fbx", L"Born", 0, 17, 0);
		NewPtr->CreateAni(L"006S1_Death.fbx", L"Death", 0, 68, 0);
		NewPtr->CreateAni(L"006S1_Dizzy.fbx", L"Dizzy", 0, 61, 0);
		NewPtr->CreateAni(L"006S1_Idle.fbx", L"Idle", 0, 41, 0);
		NewPtr->CreateAni(L"006S1_Jump.fbx", L"Jump", 0, 49, 0);
		NewPtr->CreateAni(L"006S1_Run.fbx", L"Run", 0, 17, 0);
		NewPtr->CreateAni(L"006S1_Skile01.fbx", L"Skill01", 0, 41, 0);
		NewPtr->CreateAni(L"006S1_Victory.fbx", L"Victory", 0, 93, 0);

		NewPtr->ChangeAni(L"Idle");
		NewRender->ShadowOn();
	}

	Make_HpBar();
}

//void SoulBreaker::Update()
//{
//	MeshActor->TRANS()->LPOS({ Info.Real_X, 0.f , Info.Real_Y });
//	MeshActor->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
//	if (!Chess_player::Round)
//		return;	
//
//	Base_Update();
//
//	if (Info.Play_Skill)
//		Skill_Update();
//}

void SoulBreaker::Skill_Init()
{
	Info.State = Chess_State::Skill01;
	Info.Play_Skill = true;

	int Rand_Num = rand() % Chess_player::Piece_Enemy_ChessBoard.size();
	Game_Ptr<Chess_Base> Target;
	for (auto& itor : Chess_player::Piece_Enemy_ChessBoard)
	{
		if (Rand_Num == 0)
		{
			Target = itor;
			break;
		}
		else
			Rand_Num--;
	}

	Target.PTR->Info.Hp - 300;
}

void SoulBreaker::Skill_Update()
{
	Info.Mana = 0;

	//bool a = NewPtr->Is_End();
	//int b = NewPtr->Get_EndNum();
	//int c = NewPtr->Get_Num();
	//wcout << L"소울 브레이커";
	//wcout << b << L" " << c << L" ";
	//if (a)
	//	wcout << L"true";
	//else
	//	wcout << L"false";
	//wcout << endl;

	if (Info.State == Chess_State::Skill01 && NewPtr->Is_End())
	{
		Info.State = Chess_State::Idle;
		Info.Play_Skill = false;
	}
}
