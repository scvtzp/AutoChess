#include "HGAMECOM.h"
#include "Game_Trans.h"
#include "HGAMEACTOR.h"

Game_Ptr<HGAMESCENE> HSCENECOM::SCENE()
{
	return m_Scene;
}

Game_Ptr<Game_Actor> HACTORCOM::ACTOR()
{
	return m_Actor;
}

Game_Ptr<HGAMESCENE> HACTORCOM::SCENE() {
	return m_Actor->SCENE();
}

Game_Ptr<Game_Trans> HACTORCOM::TRANS() 
{
	return m_Actor->TRANS();
}

bool HACTORCOM::IsUpdate()
{
	// ������ bool���� true�� ���� bool���� true���� ���� ���� �����Ҽ� �ִ�. 
	return true == Game_Virtualprgoress::IsUpdate() && true == m_Actor->IsUpdate();
}

bool HACTORCOM::IsDeath()
{
	// ������ bool���� true�� ���� bool���� true���� ���� ���� �����Ҽ� �ִ�. 
	return true == Game_Virtualprgoress::IsDeath() || true == m_Actor->IsDeath();
}
