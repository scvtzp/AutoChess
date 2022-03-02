#include "HGAMEACTOR.h"

Game_Trans* Game_Actor::TRANS()
{
	return m_TRANS;
}

HGAMESCENE* Game_Actor::SCENE() 
{
	return m_SCENE;
}


void Game_Actor::UpdatePrev() {
	HUPDATELISTOBJMGR<HACTORCOM>::UpdatePrev();
}
void Game_Actor::Update() {
	HUPDATELISTOBJMGR<HACTORCOM>::Update();
}
void Game_Actor::UpdateNext() {
	HUPDATELISTOBJMGR<HACTORCOM>::UpdateNext();
}
void Game_Actor::RenderPrev() {
	HUPDATELISTOBJMGR<HACTORCOM>::RenderPrev();
}
void Game_Actor::RenderNext() {
	HUPDATELISTOBJMGR<HACTORCOM>::RenderNext();
}
void Game_Actor::CollisionPrev() {
	HUPDATELISTOBJMGR<HACTORCOM>::CollisionPrev();
}
void Game_Actor::CollisionNext() {
	HUPDATELISTOBJMGR<HACTORCOM>::CollisionNext();
}
void Game_Actor::SceneChangeEnd() {
	HUPDATELISTOBJMGR<HACTORCOM>::SceneChangeEnd();
}
void Game_Actor::SceneChangeStart() {
	HUPDATELISTOBJMGR<HACTORCOM>::SceneChangeStart();
}
void Game_Actor::Release() {
	HUPDATELISTOBJMGR<HACTORCOM>::Release();
}
