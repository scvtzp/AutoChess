#pragma once
#include "GAMELOGIC.h"
#include "Chess_Base.h"

class SoulBreaker : public Chess_Base
{
public:
	SoulBreaker();
	SoulBreaker(int Star); //Star = 1 ����Ʈ
	~SoulBreaker();

	void Set_Info(int Star);
	void Init() override;
	//void Update() override;

	void Skill_Init() override;
	void Skill_Update() override;

	//��¿�
	//Game_Ptr<Game_Actor> MeshActor;
	//Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr;
};

