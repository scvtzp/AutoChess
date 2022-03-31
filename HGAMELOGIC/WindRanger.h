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

	void Set_Info(int Star); //영어로 생성자

	void Skile();

	bool UpCheck = false; //레벨업 처리했는가?

	void Fbx_Init();

	void Skill_Init() override;
	void Skill_Update() override;

	//출력용
	//Game_Ptr<Game_Actor> MeshActor;
	//<Game_BoneAnimationCom_Ex> NewPtr;

	//std::vector<Game_Ptr<HRENDERPLAYER>> RenderPlayer;
	//Game_Ptr<Game_Fbx_Ex> Fbx_Ex;
};

