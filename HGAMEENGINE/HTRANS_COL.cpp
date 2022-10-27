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

///////Ray////////
//RAY3D-OBB �浹
bool Game_Trans::RAY3D_COL_OBB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{

	return true;
}

//RAY3D-�ڽ� �浹
bool Game_Trans::RAY3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
	return false;
}

//RAY3D-�� �浹
bool Game_Trans::RAY3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right)
{
    Game_Ptr<HCAM> m_Camera = HGAMESCENE::m_CurScene->m_MainCam;
    bool m_beginCheck = false;
    int m_screenWidth = 1280;
    int m_screenHeight = 720;

    HMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
    DirectX::XMFLOAT3 direction, origin, rayOrigin, rayDirection;

    // ���콺 Ŀ�� ��ǥ�� -1���� +1 ������ �̵��մϴ�
    float pointX = ((2.0f * HGAMEINPUT::MousePos3D().x) / (float)m_screenWidth) - 1.0f;
    float pointY = (((2.0f * HGAMEINPUT::MousePos3D().y) / (float)m_screenHeight) - 1.0f) * -1.0f;

    // ����Ʈ�� ��Ⱦ�� ����Ͽ� ���� ����� ����Ͽ� ���� �����մϴ�
    //m_D3D->GetProjectionMatrix(projectionMatrix);
    projectionMatrix = m_Camera->Get_ProjectionMatrix();

    DirectX::XMFLOAT3X3 projectionMatrix4;
    XMStoreFloat3x3(&projectionMatrix4, projectionMatrix);

    pointX = pointX / projectionMatrix4._11;
    pointY = pointY / projectionMatrix4._22;

    // �� ����� ���Լ��� ���մϴ�.
    //m_Camera->GetViewMatrix(viewMatrix);
    viewMatrix = m_Camera->Get_ViewMatrix();
    inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);

    DirectX::XMFLOAT3X3 inverseViewMatrix4;
    XMStoreFloat3x3(&inverseViewMatrix4, inverseViewMatrix);

    // �� �������� ��ŷ ������ ������ ����մϴ�.
    direction.x = (pointX * inverseViewMatrix4._11) + (pointY * inverseViewMatrix4._21) + inverseViewMatrix4._31;
    direction.y = (pointX * inverseViewMatrix4._12) + (pointY * inverseViewMatrix4._22) + inverseViewMatrix4._32;
    direction.z = (pointX * inverseViewMatrix4._13) + (pointY * inverseViewMatrix4._23) + inverseViewMatrix4._33;

    // ī�޶��� ��ġ �� picking ray�� ������ �����ɴϴ�.
    origin = m_Camera->TRANS()->WPOS();

    // ���� ����� ������ ���� ��ġ�� ��ȯ�մϴ�.
    //m_D3D->GetWorldMatrix(worldMatrix);
    worldMatrix = m_Camera->TRANS()->TRANSDATA().WWORLD;
    translateMatrix = DirectX::XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
    worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, translateMatrix);

    // ���� ���� �� ����� ���Լ��� ���Ͻʽÿ�.
    inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrix);

    // ���� ���� ������ ���� ������ �� �������� ���� �������� ��ȯ�մϴ�.
    XMStoreFloat3(&rayOrigin, DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(origin.x, origin.y, origin.z, 0.0f), inverseWorldMatrix));
    XMStoreFloat3(&direction, DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f), inverseWorldMatrix));

    // ���� ������ ǥ��ȭ�մϴ�.
    XMStoreFloat3(&rayDirection, DirectX::XMVector3Normalize(DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f)));

    // ���� ���� �� ���� �׽�Ʈ�� �����Ͻʽÿ�.
    //XNA::Sphere Sphere;
    //Sphere.Center = { _Right->TRANS()->WPOS().x,_Right->TRANS()->WPOS().y,_Right->TRANS()->WPOS().z };
    //Sphere.Radius = _Right->TRANS()->WSCALE().x / 2;
    //float Dist;

   return false;
}

//BOOL IntersectRaySphere(FXMVECTOR Origin, FXMVECTOR Direction, const Sphere* pVolume, FLOAT* pDist)