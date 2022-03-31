#include "Warewolf.h"
#include "Chess_player.h"
#include "MiniWolf.h"
#include "Chess_player.h"

/*
Attack01		[0,33]
Attack02		[34,67]
Born		[68,113]
Death		[114,185]
Dizzy		[186,278]
Idle		[279,329]
Jump		[330,370]
Run		[371,392]
Skill01		[393,433]
Victory		[434,531]

본체 변신
Attack01		[0,40]
Attack02		[41,81]
Born		[82,127]
Death		[128,181]
Dizzy		[182,242]
Idle		[243,273]
Jump		[274,349]
Run		[350,368]
Victory		[369,476]

*/

WareWolf::WareWolf()
{
	Set_Info(1);
}

WareWolf::WareWolf(int Star = 1)
{
	Set_Info(Star);
}

WareWolf::~WareWolf()
{
}

void WareWolf::Set_Info(int Star = 1)
{
	Info.Name = "WareWolf";

	Info.RaceName1 = Chess_Race::Human;
	Info.RaceName2 = Chess_Race::End;
	Info.ClassName = Chess_Class::End;

	Info.Cost = 3; //가격
	Info.Star = Star; //등급

	Info.Hp = 750; //체력
	//Info.Mana = 0; //마나
	//Info.MaxMana = 200; //최대마나

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

void WareWolf::Init()
{
	//출력
	{
		MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"WareWolfS1.FBX", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"WareWolfS1.FBX");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");		

		NewPtr->CreateAni(L"WareWolfS1_Death.fbx", L"Death", 0, 71, 0);
		NewPtr->CreateAni(L"WareWolfS1.FBX", L"Attack01", 0, 33, 0);
		NewPtr->CreateAni(L"WareWolfS1_Run.fbx", L"Run", 0, 21, 0);
		NewPtr->CreateAni(L"WareWolfS1_Idle.fbx", L"Idle", 0, 50, 0);
		NewPtr->CreateAni(L"WareWolfS1_Born.fbx", L"Born", 0, 45, 0);
		NewPtr->CreateAni(L"WareWolfS1_Dizzy.fbx", L"Dizzy", 0, 92, 0);
		NewPtr->CreateAni(L"WareWolfS1_Skill01.fbx", L"Skill01", 0, 40, 0);
		NewPtr->CreateAni(L"WareWolfS1_Victory.fbx", L"Victory", 0, 97, 0);

		NewPtr->ChangeAni(L"Attack01");

		NewRender->ShadowOn();
	}
}

void WareWolf::Update()
{
	MeshActor->TRANS()->LPOS({ Info.Real_X, 0.f , Info.Real_Y });
	MeshActor->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
	if (!Chess_player::Round)
		return;	
	
	Death_Check();
	//if (Info.Hp <= 0)
	//{
	//	ACTOR()->Death();
	//	Info.Death = true;
	//	MeshActor->Death();
	//}

	if (!Info.Banch)
	{
		Move();
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

void WareWolf::Skile()
{
	if (Form_change)
	{

	}
	else if (Info.Mana >= Info.MaxMana)
	{
		Info.Mana = 0;
		Form_change = true;

		switch (Info.Star)
		{
		case 1:
			Info.Hp += 200.f;
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
		}

		{
			bool Left = true;
			bool Right = true;
			if (!Info.MyUnit) //적군이라면
			{
				for (auto iter : Chess_player::Piece_Board)
				{
					if (iter->Info.Position_Y == Info.Position_Y)
					{
						if (iter->Info.Position_X == Info.Position_X + 1)
						{
							Right = false;
						}
						if (iter->Info.Position_X == Info.Position_X - 1)
						{
							Left = false;
						}

					}
				}

			}
			else
			{
				for (auto iter : Chess_player::Piece_Enemy_ChessBoard)
				{
					if (iter->Info.Position_Y == Info.Position_Y)
					{
						if (iter->Info.Position_X == Info.Position_X + 1)
						{
							Right = false;
						}
						if (iter->Info.Position_X == Info.Position_X - 1)
						{
							Left = false;
						}

					}
				}
			}

			if (Left)
			{
				Game_Ptr<Game_Actor> PTR2 = SCENE()->CreateActor();
				Game_Ptr<MiniWolf> TestEnemy3 = PTR2->CreateCom<MiniWolf>();
				TestEnemy3->WolfStar(Info.Star);
				TestEnemy3->Info.Position_X = Info.Position_X-1;
				TestEnemy3->Info.Position_Y = Info.Position_Y-1;
				TestEnemy3->Info.Real_X = TestEnemy3->Info.Position_X;
				TestEnemy3->Info.Real_Y = TestEnemy3->Info.Position_Y;
				if (!Info.MyUnit) //적군이라면
				{
					TestEnemy3->Info.MyUnit = false;
					Chess_player::Piece_Enemy_ChessBoard.emplace_back(TestEnemy3);
				}
				else
				{
					TestEnemy3->Info.MyUnit = true;
					Chess_player::Piece_Board.emplace_back(TestEnemy3);
				}

			}
			if (Right)
			{
				Game_Ptr<Game_Actor> PTR2 = SCENE()->CreateActor();
				Game_Ptr<MiniWolf> TestEnemy3 = PTR2->CreateCom<MiniWolf>();
				TestEnemy3->WolfStar(Info.Star);
				TestEnemy3->Info.Position_X = Info.Position_X+1;
				TestEnemy3->Info.Position_Y = Info.Position_Y+1;
				TestEnemy3->Info.Real_X = TestEnemy3->Info.Position_X;
				TestEnemy3->Info.Real_Y = TestEnemy3->Info.Position_Y;
				if (!Info.MyUnit) //적군이라면
				{
					TestEnemy3->Info.MyUnit = false;
					Chess_player::Piece_Enemy_ChessBoard.emplace_back(TestEnemy3);
				}
				else
				{
					TestEnemy3->Info.MyUnit = true;
					Chess_player::Piece_Board.emplace_back(TestEnemy3);
				}

			}
		}
	}
}

void WareWolf::Skill_Init()
{
}

void WareWolf::Skill_Update()
{
}
