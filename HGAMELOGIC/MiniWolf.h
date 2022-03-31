#pragma once
#include "Chess_Base.h"

class MiniWolf : public Chess_Base
{
public:
	MiniWolf();
	~MiniWolf();

	void WolfStar(int num)
	{
		switch (num)
		{
		case 1:
			Info.Hp = 300;
			Info.Atk = 60;
			break;
		case 2:
			Info.Hp = 500;
			Info.Atk = 100;
			break;
		case 3:
			Info.Hp = 700;
			Info.Atk = 140;
			break;
		default:
			break;
		}
	}

	void Init() override;
	void Update() override;

	void Skill_Init() override;
	void Skill_Update() override;

	//Ãâ·Â¿ë
	//Game_Ptr<Game_Actor> MeshActor;
	//Game_Ptr<Game_BoneAnimationCom_Ex> NewPtr;
};

