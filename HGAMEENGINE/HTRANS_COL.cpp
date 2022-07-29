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
bool Game_Trans::OBB2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) 
{
	return _Left->OBB2D().Intersects(_Right->AABB2D());;
}
bool Game_Trans::OBB2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return _Left->OBB2D().Intersects(_Right->SPHERE2D());;
}

///////////3D/////////////
/////��/////
//��-obb
bool Game_Trans::SPHERE3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}
//��-�ڽ� �浹

bool Game_Trans::SPHERE3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//��-�� �浹
bool Game_Trans::SPHERE3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	//�� �� ������ �Ÿ� ����.
	float distance = sqrt((_Left->WPOS().x - _Right->WPOS().x) * (_Left->WPOS().x - _Right->WPOS().x) +
		(_Left->WPOS().y - _Right->WPOS().y) * (_Left->WPOS().y - _Right->WPOS().y) +
		(_Left->WPOS().z - _Right->WPOS().z) * (_Left->WPOS().z - _Right->WPOS().z));

	//�� �̱� ������ scale x=y=z.
	//������ ���ϱ� ���� Scale/2 �� ������
	return distance < (_Left->WSCALE().x / 2) + (_Right->WSCALE().x / 2);
}

//��-ray �浹
bool Game_Trans::SPHERE3D_COL_RAY3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

/////////AABB///////////
//�ڽ�-OBB �浹
bool Game_Trans::AABB3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//�ڽ�-�ڽ� �浹
bool Game_Trans::AABB3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	Game_Vector Min1, Min2, Max1, Max2;
	Min1 = { _Left->WPOS().x - _Left->WSCALE().x/2,  _Left->WPOS().y - _Left->WSCALE().y / 2, _Left->WPOS().z - _Left->WSCALE().z / 2 };
	Max1 = { _Left->WPOS().x + _Left->WSCALE().x/2,  _Left->WPOS().y + _Left->WSCALE().y / 2, _Left->WPOS().z + _Left->WSCALE().z / 2 };
	Min2 = { _Right->WPOS().x - _Right->WSCALE().x/2,  _Right->WPOS().y - _Right->WSCALE().y / 2, _Right->WPOS().z - _Right->WSCALE().z / 2 };
	Max2 = { _Right->WPOS().x + _Right->WSCALE().x/2,  _Right->WPOS().y + _Right->WSCALE().y / 2, _Right->WPOS().z + _Right->WSCALE().z / 2 };


	if (Min1.x <= Max2.x && Max1.x >= Min2.x &&
		Min1.y <= Max2.y && Max1.y >= Min2.y &&
		Min1.z <= Max2.z && Max1.z >= Min2.z)
		return true;

	return false;
}

//�ڽ�-�� �浹
bool Game_Trans::AABB3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//�ڽ�-ray �浹
bool Game_Trans::AABB3D_COL_RAY3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

///////OBB////////
//OBB-OBB �浹
bool Game_Trans::OBB3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//OBB-�ڽ� �浹
bool Game_Trans::OBB3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return false;
}

//OBB-�� �浹
bool Game_Trans::OBB3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//OBB-ray �浹
bool Game_Trans::OBB3D_COL_RAY3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}