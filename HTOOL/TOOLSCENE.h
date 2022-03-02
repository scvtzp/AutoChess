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
class HTOOLMAPSCENE : public HSCENECOM
{
public:
	static HTOOLMAPSCENE* Inst;
	static HBoneAnimationCom* AniCom;
	Game_Ptr<class HOutLineFilter> m_LineFilter;
	Game_Ptr<class HBloomFilter> m_BloomFilter;

	//Game_Ptr<class HOutLineFilter> m_LineFilter_Ui;
	//Game_Ptr<class HBloomFilter> m_BloomFilter_Ui;

	// ķ
public:
	Game_Ptr<Game_Actor> CamActor;
	Game_Ptr<Game_Actor> CamActor_Ui;

public:
	Game_Ptr<Game_Actor> TestActor;
	Game_Ptr<Game_Actor> LightTest;
	//Game_Ptr<Game_Actor> Light_Ui;
	Game_Ptr<Game_Actor> PlayerActor;
	Game_Ptr<HHeightMap> Map;
	Game_Ptr<HLight> LightCom;

public:
	void CreateFBXObject(MeshCreateData _FbxObjectData);
	void PlayerUpdate();

public:
	HTOOLMAPSCENE();
	~HTOOLMAPSCENE();

public:
	void SceneChangeStart() override;
	void SceneChangeEnd() override;
	void Init() override;
	void Update() override;

	void LoadAni(Game_String _Name);

	void Load_Functions();

	bool Start = false;
	float StartTime = 0.f;
};