#pragma once
#include <HPTR.h>
#include <HGAMEMATH.h>
#include <list>
#include "Game_Virtualprgoress.h"
// ���� ������ ���
// ������Ʈ�� �������� ��ɵ�
// ���� ������Ʈ�� ����
// �̳༮�� ��� �޾Ƽ�.

class HGAMECOM : public Game_Ref, public Game_Virtualprgoress
{
};


// �з��Ѵ�.
class HGAMESCENE;
class HSCENECOM : public HGAMECOM
{
	friend HGAMESCENE;
private:
	HGAMESCENE* m_Scene;

public:
	Game_Ptr<HGAMESCENE> SCENE();

private:
	// ������ ���ټ��� �ֳ��� ������?
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
	// ������ ���ټ��� �ֳ��� ������?
	void ACTOR(Game_Actor* _Ptr)
	{
		m_Actor = _Ptr;
	}
};

