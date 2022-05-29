#include "MiniWolf.h"
#include "Chess_player.h"

/*
잡몹
Attack01		[0,34]
Attack02		[35,69]
Born		[70,110]
Death		[111,161]
Dizzy		[162,196]
Idle		[197,233]
Jump		[234,274]
Run		[275,291]
Victory		[292,368]
*/

MiniWolf::MiniWolf()
{
	Info.Name = "MiniWolf";

	Info.RaceName1 = Chess_Race::End;
	Info.RaceName2 = Chess_Race::End;
	Info.ClassName = Chess_Class::End;

	Info.Cost = 3; //가격
	Info.Star = 1; //등급

	Info.Mana = 0; //마나

	Info.Atk_Rate = 1.2f; //공속
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

MiniWolf::~MiniWolf()
{
}

void MiniWolf::Init()
{
	MeshActor = SCENE()->CreateActor();
	MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
	MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
	MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

	Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
	NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
	std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"SWareWolfS1_Idle.fbx", L"3DANIDefferdTexture", (int)RenderType::Default);

	Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"SWareWolfS1_Idle.fbx");
	DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
	Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
	Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

	NewPtr->CreateAni(L"SWareWolfS1_Attack01.fbx", L"Attack01", 0, 34, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Attack02.fbx", L"Attack02", 0, 34, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Born.fbx", L"Born", 0, 33, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Death.fbx", L"Death", 0, 33, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Dizzy.fbx", L"Dizzy", 0, 33, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Idle.fbx", L"Idle", 0, 33, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Run.fbx", L"Run", 0, 15, 0);
	NewPtr->CreateAni(L"SWareWolfS1_Victory.fbx", L"Victory", 0, 75, 0);
	NewPtr->ChangeAni(L"Attack01");

	NewRender->ShadowOn();
	Make_HpBar();

}

//void MiniWolf::Update()
//{
//	MeshActor->TRANS()->LPOS({ Info.Real_X, 0.f , Info.Real_Y });
//	MeshActor->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
//	if (!Chess_player::Round)
//		return;
//	Info.Mana = 0; //스킬 사용못하게 막아버림.
//
//	Death_Check();
//	//if (Info.Hp <= 0)
//	//{
//	//	ACTOR()->Death();
//	//	Info.Death = true;
//	//	MeshActor->Death();
//	//}
//
//	//이동 (밴칭에 있으면 패스)
//	if (!Info.Banch)
//	{
//		Move();
//	}
//
//
//	switch (Info.State)
//	{
//	case Chess_State::Attack_1:
//		NewPtr->ChangeAni(L"Attack01");
//		break;
//	case Chess_State::Attack_2:
//		NewPtr->ChangeAni(L"Attack02");
//		break;
//	case Chess_State::Born:
//		NewPtr->ChangeAni(L"Born");
//		break;
//	case Chess_State::Death:
//		NewPtr->ChangeAni(L"Death");
//		break;
//	case Chess_State::Dizzy:
//		NewPtr->ChangeAni(L"Dizzy");
//		break;
//	case Chess_State::Idle:
//		NewPtr->ChangeAni(L"Idle");
//		break;
//	case Chess_State::Jump:
//		NewPtr->ChangeAni(L"Jump");
//		break;
//	case Chess_State::Run:
//		NewPtr->ChangeAni(L"Run");
//		break;
//	case Chess_State::Skill01:
//		NewPtr->ChangeAni(L"Skill01");
//		break;
//	case Chess_State::Victory:
//		NewPtr->ChangeAni(L"Victory");
//		break;
//	default:
//		NewPtr->ChangeAni(L"Idle");
//		break;
//	}
//}

void MiniWolf::Skill_Init()
{
}

void MiniWolf::Skill_Update()
{
}
