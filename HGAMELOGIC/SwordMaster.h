#pragma once
#include  "GAMELOGIC.h"
#include "Chess_Base.h"

class SwordMaster : public Chess_Base
{
public:
	SwordMaster();
	SwordMaster(int Star);
	~SwordMaster();

	void Set_Info(int Star);
	void Init() override;
	//void Update() override;

	void Skill_Init() override;
	void Skill_Update() override;

	//Ãâ·Â¿ë
	//Game_Ptr<Game_Actor> MeshActor;
	//Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr;
};

