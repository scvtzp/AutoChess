#pragma once
#include <HPTR.h>
#include <HGAMEMATH.h>
#include <list>
#include "Game_Virtualprgoress.h"
// 가장 말단의 기능
// 오브젝트가 가져야할 기능들
// 내가 오브젝트로 몬스터
// 이녀석을 상속 받아서.

class HGAMECOM : public Game_Ref, public Game_Virtualprgoress
{
};


// 분류한다.
class HGAMESCENE;
class HSCENECOM : public HGAMECOM
{
	friend HGAMESCENE;
private:
	HGAMESCENE* m_Scene;

public:
	Game_Ptr<HGAMESCENE> SCENE();

private:
	// 세팅을 해줄수가 있나요 없나요?
	void Scene(HGAMESCENE* _Ptr)
	{
		m_Scene = _Ptr;
	}

};

class Game_Trans;
class Game_Actor;
class HACTORCOM : public HGAMECOM
{
private:
	friend Game_Actor;

private:
	Game_Actor* m_Actor;

public:
	Game_Ptr<Game_Actor> ACTOR();
	Game_Ptr<HGAMESCENE> SCENE();
	Game_Ptr<Game_Trans> TRANS();

	bool IsUpdate() override;
	bool IsDeath() override;


private:
	// 세팅을 해줄수가 있나요 없나요?
	void ACTOR(Game_Actor* _Ptr)
	{
		m_Actor = _Ptr;
	}
};

