#pragma once
#include <GAMELOGIC.h>

class Game_Actor;
class HBoneAnimationCom;
class TestScene : public HSCENECOM
{
public:
	static TestScene* Inst;
	static HBoneAnimationCom* AniCom;
	Game_Ptr<class HOutLineFilter> m_LineFilter;
	Game_Ptr<class HBloomFilter> m_BloomFilter;

public:
	Game_Ptr<Game_Actor> CamActor;
	Game_Ptr<Game_Actor> CamActor_Ui;

public:
	Game_Ptr<Game_Actor> TestActor;
	Game_Ptr<Game_Actor> LightTest;
	//Game_Ptr<Game_Actor> Light_Ui;
	Game_Ptr<Game_Actor> PlayerActor;
	Game_Ptr<HLight> LightCom;

public:
	TestScene();
	~TestScene();
	void SceneChangeStart() override;
	void SceneChangeEnd() override;
	void Init() override;
	void Update() override;

	void Load_Functions();

private:

};

