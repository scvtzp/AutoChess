#pragma once
#include "Chess_Base.h"
class WareWolf : public Chess_Base
{
public:
	WareWolf();
	WareWolf(int Star);
	~WareWolf();

	void Set_Info(int Star);
	void Init() override;
	//void Update() override;
	void Skile();

	bool Form_change = false; //true = ����

	void Skill_Init() override;
	void Skill_Update() override;

	//��¿�
	//Game_Ptr<Game_Actor> MeshActor;
	//Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr;
};

