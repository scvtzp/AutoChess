#pragma once
#include <GAMELOGIC.h>

class Game_Actor;
class HBoneAnimationCom;
class CL_LobbyScene : public HSCENECOM
{
public:
	Game_Ptr<class HOutLineFilter> m_LineFilter;
	Game_Ptr<class HBloomFilter> m_BloomFilter;

	//¸¶¿ì½º
	Game_Ptr<Game_Collision> Mouse_Col;
	Game_Ptr<Game_Renderer> Mouse_ColRenderer;

public:
	Game_Ptr<Game_Actor> CamActor;
	Game_Ptr<Game_Actor> CamActor_Ui;

public:
	Game_Ptr<Game_Actor> TestActor;
	Game_Ptr<Game_Actor> LightTest;
	Game_Ptr<Game_Actor> Mouse_Actor;
	//Game_Ptr<Game_Actor> Light_Ui;
	Game_Ptr<Game_Actor> PlayerActor;
	Game_Ptr<HLight> LightCom;

	CL_LobbyScene();
	~CL_LobbyScene();
	void SceneChangeStart() override;
	void SceneChangeEnd() override;
	void Init() override;
	void Update() override;

	void Load_Functions();

	Game_Ptr<Game_BoneAnimationCom_Ex>	NewPtr;

private:

};

