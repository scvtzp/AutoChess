#pragma once
#include <HGAMECOM.h>

class HCLIENTSCENECOM : public HSCENECOM
{
private:
	//static HBoneAnimationCom* AniCom;
	//Game_Ptr<class HOutLineFilter> m_LineFilter;
	//Game_Ptr<class HBloomFilter> m_BloomFilter;
	Game_Ptr<Game_Actor> CamActor;
private:


public:

	void Init() override;
	void Update();

public:
	HCLIENTSCENECOM();
	~HCLIENTSCENECOM();
};

