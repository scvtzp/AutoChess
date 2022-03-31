#pragma once
#include  "GAMELOGIC.h"
#include "Chess_Base.h"

class WindRanger : public Chess_Base
{
public:
	WindRanger();
	WindRanger(int Star);
	~WindRanger();

	void Init() override;
	void Update() override;

	void Set_Info(int Star); //����� ������

	void Skile();

	bool UpCheck = false; //������ ó���ߴ°�?

	void Fbx_Init();

	void Skill_Init() override;
	void Skill_Update() override;

	//��¿�
	//Game_Ptr<Game_Actor> MeshActor;
	//<Game_BoneAnimationCom_Ex> NewPtr;

	//std::vector<Game_Ptr<HRENDERPLAYER>> RenderPlayer;
	//Game_Ptr<Game_Fbx_Ex> Fbx_Ex;
};

