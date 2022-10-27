#pragma once
#include <GAMELOGIC.h>

class MeshCreateData
{
public:
	Game_String m_MeshName;
	Game_String m_TakeName;
	Game_Vector m_Scale;
	Game_Vector m_Pos;
	Game_Vector m_Rot;
};

class Game_Actor;
class HBoneAnimationCom;
class HHeightMap;

class CL_TestScene : public HSCENECOM
{
	static CL_TestScene* Inst;
	static HBoneAnimationCom* AniCom;
	Game_Ptr<class HOutLineFilter> m_LineFilter;
	Game_Ptr<class HBloomFilter> m_BloomFilter;

	//Game_Ptr<class HOutLineFilter> m_LineFilter_Ui;
	//Game_Ptr<class HBloomFilter> m_BloomFilter_Ui;

public:
	Game_Ptr<Game_Actor> CamActor;
	Game_Ptr<Game_Actor> CamActor_Ui;

	Game_Ptr<Game_Actor> MouseActor;
	Game_Ptr<Game_Actor> RayActor;
	Game_Ptr<HCAM> TestScene_MainCam;
public:
	Game_Ptr<Game_Actor> TestActor;
	Game_Ptr<Game_Actor> LightTest;
	//Game_Ptr<Game_Actor> Light_Ui;
	Game_Ptr<Game_Actor> PlayerActor;
	Game_Ptr<HHeightMap> Map;
	Game_Ptr<HLight> LightCom;

public:
	void PlayerUpdate();

public:
	CL_TestScene();
	~CL_TestScene();

public:
	void SceneChangeStart() override;
	void SceneChangeEnd() override;
	void ColTest(Game_Collision* _This, Game_Collision* _Other);
	void Init() override;
	void Update() override;

	void LoadAni(Game_String _Name);

	void Load_Functions();

	bool Start = false;
	float StartTime = 0.f;
};
