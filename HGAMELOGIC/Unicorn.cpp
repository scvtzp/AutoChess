#include "Unicorn.h"
#include <Game_3D_Debug.h>
#include "Chess_player.h"
#include <math.h>

Unicorn::Unicorn()
{
	Set_Info(1);
}

Unicorn::Unicorn(int Star)
{
	Set_Info(Star);
}

Unicorn::~Unicorn()
{
}

void Unicorn::Set_Info(int Star)
{
	Info.Name = "Unicorn";

	Info.RaceName1 = Chess_Race::Beast;
	Info.RaceName2 = Chess_Race::End;
	Info.ClassName = Chess_Class::End;

	Info.Cost = 1; //가격
	Info.Star = Star; //등급

	Info.Hp = 600; //체력

	Info.Atk = 55*Star; //공
	Info.Atk_Rate = 1.5f; //공속
	Info.Range = 3.5f; //사거리
	Info.Hp_Regeneration = 0; //체젠
	Info.Armor = 5; //방어력
	Info.Magic_Resistance = 0; //마방

	Info.Damage_To_Player = 1; //플레이어에게 데미지.
	Info.Position_X = 0;
	Info.Position_Y = 0;

	Info.Real_X = Info.Position_X;
	Info.Real_Y = Info.Position_Y;
}

void Unicorn::Init()
{
	//출력
	{
		MeshActor = SCENE()->CreateActor();
		MeshActor->TRANS()->WSCALE({ 0.01f, 0.01f , 0.01f });
		MeshActor->TRANS()->LPOS({ 0.00f, 0.00f , 0.0f });
		MeshActor->TRANS()->LROT({ -90.0f, 0.0f , 0.0f });

		Game_Ptr<Game_Renderer> NewRender = MeshActor->CreateCom<Game_Renderer>((int)RenderType::Default);
		NewPtr = MeshActor->CreateCom<Game_BoneAnimationCom_Ex>();
		std::vector<Game_Ptr<HRENDERPLAYER>> Ptr = NewPtr->MainFbx(L"002S1_Attack01.fbx", L"3DANIDefferdTexture", (int)RenderType::Default);

		Game_Ptr<Game_Fbx_Ex> Monster = Game_Fbx_Ex::Find(L"002S1_Attack01.fbx");
		DRAWSET* DrawSet1 = Monster->GetDrawSet(0);
		Ptr[0]->TEXTURE(L"Tex", DrawSet1->m_MatialData[0][0].DifTexture);
		Ptr[0]->SAMPLER(L"Smp", L"LWSMP");

		NewPtr->CreateAni(L"002S1_Attack01.fbx", L"Attack01", 0, 45, 0);
		NewPtr->CreateAni(L"002S1_Born.fbx", L"Born", 0, 45, 0);
		NewPtr->CreateAni(L"002S1_Death.fbx", L"Death", 0, 54, 0);
		NewPtr->CreateAni(L"002S1_Dizzy.fbx", L"Dizzy", 0, 60, 0);
		NewPtr->CreateAni(L"002S1_Idle.fbx", L"Idle", 0, 60, 0);
		NewPtr->CreateAni(L"002S1_Run.fbx", L"Run", 0, 18, 0);
		NewPtr->CreateAni(L"002S1_Skill01.fbx", L"Skill01", 0, 46, 0);
		NewPtr->CreateAni(L"002S1_Victory.fbx", L"Victory", 0, 65, 0);

		NewPtr->ChangeAni(L"Idle");
		NewRender->ShadowOn();
	}

	//스킬 이펙트 미리 로드
	{
		EffectActor = SCENE()->CreateActor();
		EffectActor->TRANS()->WSCALE({ 3.f, 3.f , 1.f });
		EffectActor->TRANS()->WPOS({ 0.00f, 0.00f , 0.0f });
		EffectActor->TRANS()->LROT({ 90.0f, 0.0f , 0.0f });

		float4 DRAWCOLOR = { 1,1,1,1 };
		float4 CUTDATA = { 0,0,1,1 };

		Game_Ptr<Game_Renderer> Unicorn_Effect_Renderer = EffectActor->CreateCom<Game_Renderer>(L"2DCOLORRECT", L"2DIMAGE", (int)RenderType::Default);
		Unicorn_Effect = EffectActor->CreateCom<Game_2DAnimation>(Unicorn_Effect_Renderer);
		Unicorn_Effect->CreateAni(L"Unicorn_Effect", L"Unicorn_Circle.png", 0, 15, 0.1f, true);
		Unicorn_Effect->ChangeAni(L"Unicorn_Effect");
		Unicorn_Effect_Renderer->CBUFFER(L"CUTDATA", &CUTDATA, CBUFFERMODE::CB_NEW);
		Unicorn_Effect_Renderer->CBUFFER(L"DRAWCOLOR", &DRAWCOLOR, CBUFFERMODE::CB_NEW);
		Unicorn_Effect_Renderer->TEXTURE(L"Tex", L"Unicorn_Circle.png");
		Unicorn_Effect_Renderer->SAMPLER(L"Smp", "LWSMP");
		Unicorn_Effect->Off();
		EffectActor->Off();
	}
	Make_HpBar();

}

//void Unicorn::Update()
//{
//	if (MeshActor == nullptr || EffectActor == nullptr)
//		return;
//	MeshActor->TRANS()->LPOS({ Info.Real_X, 0.0f , Info.Real_Y });
//	EffectActor->TRANS()->LPOS({ Info.Real_X, 0.02f, Info.Real_Y });
//	MeshActor->TRANS()->LROT({ -90.f , TRANS()->LROT().y,  TRANS()->LROT().z });
//
//	Base_Update();
//
//
//	if (!Chess_player::Round)
//		return;
//
//
//	if (Info.Play_Skill)
//		Skill_Update();
//
//}

void Unicorn::Skill_Init()
{
	//사실 스킬init에는 이것만
	Unicorn_Effect->On();
	EffectActor->On();
}

void Unicorn::Skill_Update()
{
	//매 1.5초마다 3칸 이내의 무작위 아군 유닛의 생명력을 25치유.
	//무작위는 안했음
	Info.SkileTime += Game_Time::DeltaTime();
	int Count = 1;
	if (Info.SkileTime >= 1.5f * Count)
	{
		Count++;
		if (Info.MyUnit)
		{
			for (auto iter : Chess_player::Piece_Board)
			{
				if (sqrt((iter->Info.Position_X - Info.Position_X)* (iter->Info.Position_X - Info.Position_X)
					+ (iter->Info.Position_Y - Info.Position_Y)* (iter->Info.Position_Y - Info.Position_Y)) < 3.f)
				{
					iter->Info.Hp += 25;
				}
			}
		}
		else
		{
			for (auto iter : Chess_player::Piece_Enemy_ChessBoard)
			{
				if (sqrt((iter->Info.Position_X - Info.Position_X) * (iter->Info.Position_X - Info.Position_X)
					+ (iter->Info.Position_Y - Info.Position_Y) * (iter->Info.Position_Y - Info.Position_Y)) < 3.f)
				{
					iter->Info.Hp += 25;
				}
			}
		}

	}
	if (Info.SkileTime >= 11.f)
	{
		Info.SkileTime = 0;
		Info.Mana = 0;
		Info.Play_Skill = false;
		Info.State = Chess_State::Idle;
		Unicorn_Effect->Off();
		EffectActor->Off();
	}
}
