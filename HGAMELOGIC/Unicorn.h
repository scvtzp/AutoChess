#pragma once
#include "GAMELOGIC.h"
#include "Chess_Base.h"
#include "Game_2DAnimation.h"

class Unicorn : public Chess_Base
{
public:
	Unicorn();
	Unicorn(int Star);
	~Unicorn();

	void Set_Info(int Star);
	void Init() override;
	void Update() override;

	void Skill_Init() override;
	void Skill_Update() override;

	//Ãâ·Â
	Game_Ptr<Game_Actor> Unicorn_Effect_Actor;

	Game_Ptr<Game_2DAnimation> Unicorn_Effect;
};

