#include "Game_Trans.h"
#include "HGAMEACTOR.h"
#include "HGAMESCENE.h"
#include "HCAM.h"
#include <HGAMEDEBUG.h>
#include <HGAMEINPUT.h>
#include <DirectXCollision.h>
#include "HTRANS_COL.h"

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
/////구/////
//구-obb
bool Game_Trans::SPHERE3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}
//구-박스 충돌

bool Game_Trans::SPHERE3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//구-구 충돌
bool Game_Trans::SPHERE3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	//두 점 사이의 거리 구함.
	float distance = sqrt((_Left->WPOS().x - _Right->WPOS().x) * (_Left->WPOS().x - _Right->WPOS().x) +
		(_Left->WPOS().y - _Right->WPOS().y) * (_Left->WPOS().y - _Right->WPOS().y) +
		(_Left->WPOS().z - _Right->WPOS().z) * (_Left->WPOS().z - _Right->WPOS().z));

	//구 이기 때문에 scale x=y=z.
	//반지름 구하기 위해 Scale/2 씩 더해줌
	return distance < (_Left->WSCALE().x / 2) + (_Right->WSCALE().x / 2);
}

//구-ray 충돌
bool Game_Trans::SPHERE3D_COL_RAY3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

/////////AABB///////////
//박스-OBB 충돌
bool Game_Trans::AABB3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//박스-박스 충돌
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

//박스-구 충돌
bool Game_Trans::AABB3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//박스-ray 충돌
bool Game_Trans::AABB3D_COL_RAY3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

///////OBB////////
//OBB-OBB 충돌
bool Game_Trans::OBB3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{

	return true;
}

//OBB-박스 충돌
bool Game_Trans::OBB3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return false;
}

//OBB-구 충돌
bool Game_Trans::OBB3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

//OBB-ray 충돌
bool Game_Trans::OBB3D_COL_RAY3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return true;
}

///////Ray////////
//RAY3D-OBB 충돌
bool Game_Trans::RAY3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{

	return true;
}

//RAY3D-박스 충돌
bool Game_Trans::RAY3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return false;
}

//RAY3D-구 충돌
bool Game_Trans::RAY3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
    Game_Ptr<HCAM> m_Camera = HGAMESCENE::m_CurScene->m_MainCam;
    bool m_beginCheck = false;
    int m_screenWidth = 1280;
    int m_screenHeight = 720;

    HMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
    DirectX::XMFLOAT3 direction, origin, rayOrigin, rayDirection;

    // 마우스 커서 좌표를 -1에서 +1 범위로 이동합니다
    float pointX = ((2.0f * HGAMEINPUT::MousePos3D().x) / (float)m_screenWidth) - 1.0f;
    float pointY = (((2.0f * HGAMEINPUT::MousePos3D().y) / (float)m_screenHeight) - 1.0f) * -1.0f;

    // 뷰포트의 종횡비를 고려하여 투영 행렬을 사용하여 점을 조정합니다
    //m_D3D->GetProjectionMatrix(projectionMatrix);
    projectionMatrix = m_Camera->Get_ProjectionMatrix();

    DirectX::XMFLOAT3X3 projectionMatrix4;
    XMStoreFloat3x3(&projectionMatrix4, projectionMatrix);

    pointX = pointX / projectionMatrix4._11;
    pointY = pointY / projectionMatrix4._22;

    // 뷰 행렬의 역함수를 구합니다.
    //m_Camera->GetViewMatrix(viewMatrix);
    viewMatrix = m_Camera->Get_ViewMatrix();
    inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);

    DirectX::XMFLOAT3X3 inverseViewMatrix4;
    XMStoreFloat3x3(&inverseViewMatrix4, inverseViewMatrix);

    // 뷰 공간에서 피킹 레이의 방향을 계산합니다.
    direction.x = (pointX * inverseViewMatrix4._11) + (pointY * inverseViewMatrix4._21) + inverseViewMatrix4._31;
    direction.y = (pointX * inverseViewMatrix4._12) + (pointY * inverseViewMatrix4._22) + inverseViewMatrix4._32;
    direction.z = (pointX * inverseViewMatrix4._13) + (pointY * inverseViewMatrix4._23) + inverseViewMatrix4._33;

    // 카메라의 위치 인 picking ray의 원점을 가져옵니다.
    origin = m_Camera->TRANS()->WPOS();

    // 세계 행렬을 가져와 구의 위치로 변환합니다.
    //m_D3D->GetWorldMatrix(worldMatrix);
    worldMatrix = m_Camera->TRANS()->TRANSDATA().WWORLD;
    translateMatrix = DirectX::XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
    worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, translateMatrix);

    // 이제 번역 된 행렬의 역함수를 구하십시오.
    inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrix);

    // 이제 광선 원점과 광선 방향을 뷰 공간에서 월드 공간으로 변환합니다.
    XMStoreFloat3(&rayOrigin, DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(origin.x, origin.y, origin.z, 0.0f), inverseWorldMatrix));
    XMStoreFloat3(&direction, DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f), inverseWorldMatrix));

    // 광선 방향을 표준화합니다.
    XMStoreFloat3(&rayDirection, DirectX::XMVector3Normalize(DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f)));

    // 이제 광선 구 교차 테스트를 수행하십시오.
    //XNA::Sphere Sphere;
    //Sphere.Center = { _Right->TRANS()->WPOS().x,_Right->TRANS()->WPOS().y,_Right->TRANS()->WPOS().z };
    //Sphere.Radius = _Right->TRANS()->WSCALE().x / 2;
    //float Dist;

   return false;
}

//BOOL IntersectRaySphere(FXMVECTOR Origin, FXMVECTOR Direction, const Sphere* pVolume, FLOAT* pDist)