#include "Game_Trans.h"
#include "HGAMEACTOR.h"
#include "HGAMESCENE.h"

#include <HGAMEDEBUG.h>

bool Game_Trans::SPHERE2DCOLOBB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return _Left->SPHERE2D().Intersects(_Right->OBB2D());
}
bool Game_Trans::SPHERE2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) {
	return _Left->SPHERE2D().Intersects(_Right->AABB2D());;
}
bool Game_Trans::SPHERE2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) 
{
	{
		Game_Vector Pos1 = _Left->WPOS();
		Game_Vector Pos2 = _Right->WPOS();


		int a = 0;
	}


	return _Left->SPHERE2D().Intersects(_Right->SPHERE2D());;
}

bool Game_Trans::AABB2DCOLOBB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) {
	return _Left->AABB2D().Intersects(_Right->OBB2D());;
}
bool Game_Trans::AABB2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) {
	return _Left->AABB2D().Intersects(_Right->AABB2D());;
}
bool Game_Trans::AABB2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) {
	return _Left->AABB2D().Intersects(_Right->SPHERE2D());;
}

bool Game_Trans::OBB2DCOLOBB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) 
{
	return _Left->OBB2D().Intersects(_Right->OBB2D());;
}
bool Game_Trans::OBB2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) {
	return _Left->OBB2D().Intersects(_Right->AABB2D());;
}
bool Game_Trans::OBB2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return _Left->OBB2D().Intersects(_Right->SPHERE2D());;
}
