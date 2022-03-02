#include "Game_Trans.h"
#include "HGAMEACTOR.h"
#include "HGAMESCENE.h"
#include "HCAM.h"
#include "HLight.h"

#include <HGAMEDEBUG.h>

bool (*Game_Trans::COLFUNC[(int)COLTYPE::MAX][(int)COLTYPE::MAX])(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right) = { nullptr,};

Game_Trans::Game_ColStart Game_Trans::Starter;

Game_Trans::Game_ColStart::Game_ColStart() 
{
	Game_Trans::COLFUNC[(int)COLTYPE::AABB2D][(int)COLTYPE::AABB2D] =	AABB2DCOLAABB2D;
	Game_Trans::COLFUNC[(int)COLTYPE::AABB2D][(int)COLTYPE::SPHERE2D] = AABB2DCOLSPHERE2D;
	Game_Trans::COLFUNC[(int)COLTYPE::AABB2D][(int)COLTYPE::OBB2D] =	AABB2DCOLOBB2D;

	Game_Trans::COLFUNC[(int)COLTYPE::OBB2D][(int)COLTYPE::AABB2D] =	OBB2DCOLAABB2D;
	Game_Trans::COLFUNC[(int)COLTYPE::OBB2D][(int)COLTYPE::SPHERE2D] =	OBB2DCOLSPHERE2D;
	Game_Trans::COLFUNC[(int)COLTYPE::OBB2D][(int)COLTYPE::OBB2D] =		OBB2DCOLOBB2D;

	Game_Trans::COLFUNC[(int)COLTYPE::SPHERE2D][(int)COLTYPE::AABB2D] =		SPHERE2DCOLAABB2D;
	Game_Trans::COLFUNC[(int)COLTYPE::SPHERE2D][(int)COLTYPE::SPHERE2D] =	SPHERE2DCOLSPHERE2D;
	Game_Trans::COLFUNC[(int)COLTYPE::SPHERE2D][(int)COLTYPE::OBB2D] =		SPHERE2DCOLOBB2D;
}

Game_Trans::Game_Trans() 
	: m_LPOS(Game_Vector::ZERO), m_LSCALE(Game_Vector::ONE), m_LROT(Game_Vector::ZERO)
	, m_WPOS(Game_Vector::ZERO), m_WSCALE(Game_Vector::ONE), m_WROT(Game_Vector::ZERO)
	, m_TRANSDATA(), CALARRMATCHECK{ true, }, m_PARENT(nullptr)
{

	//Game_Vector PlayerPos;
	//PlayerPos.y += 100.0f;

	//// �÷��̾��� ������
	//Game_Vector RayPos = PlayerPos;
	//Game_Vector RayDir;
	//Game_Vector Tri0;
	//Game_Vector Tri1;
	//Game_Vector Tri2;
	//float Result = -1.0f;
	//DirectX::TriangleTests::Intersects(RayPos, RayDir, Tri0, Tri1, Tri2, Result);

	//if (0 != Result)
	//{
	//	// �浹�ߴ�.
	//	return Result;
	//}

	//Game_Vector Tri3;
	//Game_Vector Tri4;
	//Game_Vector Tri5;
	//DirectX::TriangleTests::Intersects(RayPos, RayDir, Tri3, Tri4, Tri5, Result);

	memset(&CALARRMATCHECK, 1, sizeof(CALARRMATCHECK));
	m_TRANSDATA.CALIDEN();
}

// ������ �� �θ� �Ǿ��شٴ°���?
void Game_Trans::PARENT(Game_Ptr<Game_Trans> _Parent)
{
	// ������ �θ� ����� ���� Ʈ����������Ʈ���� �����°Ŵ�.
	ACTOR()->SCENE()->m_AllTrans.remove(this);

	if (nullptr != _Parent)
	{
		// ���ڵ尡 �־�� �Ѵ�.
		// ��������� ���� ���� �θ� ���ٴ°��� ������ �ϰ� �ִ� �ڵ尡.
		if (nullptr != m_PARENT)
		{
			m_PARENT->m_CHILDLIST.remove(this);
		}


		// �θ����????? �θ��� �ڽ����� ����.
		_Parent->m_CHILDLIST.push_back(this);

		// �θ���� �������� �ѹ��� ���� ����.
		CALARRMATCHECK[(int)MATTYPE::MATTYPE_PARENT] = true;
		// ���� ��������� �ڽ��� �θ���Ŀ� �Է����ְ�
		m_TRANSDATA.PARENT = _Parent->m_TRANSDATA.WWORLD;
		// �ָ��л��� �˾Ƴ� ���� �Ǽ���.
		// ���� ���� �θ�?
		m_PARENT = _Parent;

		LSCALE(WSCALE());
		LROT(WROT());
		LPOS(WPOS());
	}
	else 
	{
		if (nullptr != m_PARENT)
		{
			m_PARENT->m_CHILDLIST.remove(this);
		}
		ACTOR()->SCENE()->m_AllTrans.push_back(this);
		
		CALWPOS();
		CALWSCALE();
		CALWROT();

		m_PARENT = nullptr;
		m_TRANSDATA.PARENT.IDEN();
		LPOS(WPOS());
		LSCALE(WSCALE());
		LROT(WROT());
	}

	// ȯ���� �������Ƿ� ����� �� �ѹ� ���� ����.
	TransUpdate();
}

void Game_Trans::Init()
{
	if (nullptr != ACTOR()->m_TRANS)
	{
		AMSG(L"�� ���Ϳ� Ʈ�������� 2���� ������ �����ϴ�.");
	}

	ACTOR()->m_TRANS = this;
	ACTOR()->SCENE()->m_AllTrans.push_back(this);
}

void Game_Trans::TransUpdate()
{
	// ũ�Ⱑ ����...
	if (true == CALARRMATCHECK[(int)MATTYPE::MATTYPE_SCALE])
	{
		m_TRANSDATA.SCALE.Scale3D(m_LSCALE);
		CALARRMATCHECK[(int)MATTYPE::MATTYPE_LWORLD] = true;
	}

	// ȸ���� ����...
	if (true == CALARRMATCHECK[(int)MATTYPE::MATTYPE_ROT])
	{
		m_TRANSDATA.ROT.RotDeg(m_LROT);

		// 3D�� ���� ������ �ٽ� �غ��ô�.
		// HMATRIX RotX;
		// HMATRIX RotY;
		// HMATRIX RotZ;
		// m_LROT.X = 90.0F;
		// m_LROT.Y = 90.0F;
		// RotX.RotXDeg(m_LROT.X);
		// RotY.RotYDeg(m_LROT.Y);
		// RotZ.RotZDeg(m_LROT.Z);
		// m_TRANSDATA.ROT = RotX * RotY * RotZ;

		CALARRMATCHECK[(int)MATTYPE::MATTYPE_LWORLD] = true;
	}

	if (true == CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS])
	{
		m_TRANSDATA.POS.Pos3D(m_LPOS);
		CALARRMATCHECK[(int)MATTYPE::MATTYPE_LWORLD] = true;
	}

	if (true == CALARRMATCHECK[(int)MATTYPE::MATTYPE_LWORLD] ||
		true == CALARRMATCHECK[(int)MATTYPE::MATTYPE_PARENT])
	{
		// CALWORLD�� �Ѵٰ� �ؼ� Wpos�� ����ǳ���?
		m_TRANSDATA.CALLWORLD();
		m_TRANSDATA.CALWWORLD();
		// ���� WWORLD�� ����Ǿ��ٴ� �ǹ̷� �̰� TRUE�� ����
		CALARRMATCHECK[(int)MATTYPE::MATTYPE_WWORLD] = true;
		if (nullptr != m_PARENT)
		{
			CALWSCALE();
			CALWROT();
			CALWPOS();
		}
	}


	// �ڽĵ�
	for (auto& _Child : m_CHILDLIST)
	{
		// �̹��������� ����� ������ �־��ٴ� �̾߱���?
		if (true == CALARRMATCHECK[(int)MATTYPE::MATTYPE_WWORLD])
		{
			_Child->CALARRMATCHECK[(int)MATTYPE::MATTYPE_PARENT] = true;
			// ���� ��������� �ڽ��� �θ���Ŀ� �Է����ְ�
			// �ڽĿ��� ��������� �Է����ְ�
			_Child->m_TRANSDATA.PARENT = m_TRANSDATA.WWORLD;
		}

		// ���� ���ؼ� �ڽŸ��� ��������� �����.

		_Child->TransUpdate();
	}


	memset(&CALARRMATCHECK, 0, sizeof(CALARRMATCHECK));
}

void Game_Trans::CamUpdate(Game_Ptr<HCAM> _Cam)
{
	m_TRANSDATA.PROJ = _Cam->m_PROJ;
	m_TRANSDATA.VIEW = _Cam->m_VIEW;
	m_TRANSDATA.CALWVP();
}

void Game_Trans::LightUpdate(const LightData& _Light)
{
	m_TRANSDATA.PROJ = _Light.m_Proj;
	m_TRANSDATA.VIEW = _Light.m_View;
	m_TRANSDATA.CALWVP();
}

bool Game_Trans::Col(COLTYPE _ThisType, Game_Ptr<Game_Trans> _Trans, COLTYPE _OtherType)
{
	if (nullptr == COLFUNC[(int)(_ThisType)][(int)(_OtherType)])
	{
		AMSG(L"�浹 �Լ��� �������� ���� �浹�� �Ϸ��� �߽��ϴ�.");
	}

	return COLFUNC[(int)(_ThisType)][(int)(_OtherType)](this, _Trans);

	//// AABB ȸ������ ���� �ڽ�
	//DirectX::BoundingOrientedBox ColThis;
	//DirectX::BoundingBox ColOther;

	//// XMFLOAT3   Game_Vector
	//ColThis.Center = WPOS();
	//// ����ٰ� �׳� ������ �־��ָ� ȸ���� �ν����� ���Ѵ�.
	//// X Y Z W�� �˲����ִ� ������� �־����
	//// �װɷ� �ѹ濡 ȸ������� ���� ȸ���� �νĵȴ�.
	//// WROT(); ���Ϸ����� �־������ �ƹ��� �ǹ̵� ����.
	//// Orientation ������� �־�޶�� �ϴ°Ŵϱ�.
	//// ������� ���ٴ� ���� 
	//// �츮�� �����ϱ� ����� ���Ҽ� ��� ����� ���յ� ����� ����ϰ� �ִ� ��.
	//// ���� �ӷ� �ʿ����.
	//// �������ΰ� ����. 
	//// ȸ������ ��׸��� �ѹ� �������� �ٲ�� �װ� �ٽ� ���ʹϿ��� �ؼ� ȸ���ϴµ�.
	//// WROT() ��ü�� �׳� ��׸��� ���̴�.
	//ColThis.Orientation = WROT().DegToQ();
	//// DirectX::XMQuaternionRotationRollPitchYaw()
	//ColThis.Extents = WSCALE().HalfVector();

	//ColOther.Center = _Trans->WPOS();
	//ColOther.Extents = _Trans->WSCALE().HalfVector();

	//if ((int)_ThisType < (int)COLTYPE::SPHERE3D)
	//{
	//	ColThis.Center.z = 0.0f;
	//}

	//if ((int)_OtherType < (int)COLTYPE::SPHERE3D)
	//{
	//	ColOther.Center.z = 0.0f;
	//}

	// �� �Լ��� �˾Ƽ� ���ݴϴ�.
	// return ColThis.Intersects(ColOther);

}

DirectX::BoundingBox Game_Trans::AABB2D() 
{
	return DirectX::BoundingBox(WPOS2D(), WSCALE().HalfVector().AbsVector());
}
DirectX::BoundingSphere Game_Trans::SPHERE2D() 
{
	return DirectX::BoundingSphere(WPOS2D(), abs(WSCALE().HalfVector().x));
}
DirectX::BoundingOrientedBox Game_Trans::OBB2D() 
{
	//Game_Vector Pos = WPOS2D();
	//Game_Vector Scale = WSCALE();
	//Game_Vector Rot = WROT();

	return DirectX::BoundingOrientedBox(WPOS2D(), WSCALE().HalfVector().AbsVector(), WROT().DegToQ());
}

DirectX::BoundingOrientedBox Game_Trans::RAY3D()
{
	//Game_Vector Pos = WPOS2D();
	//Game_Vector Scale = WSCALE();
	//Game_Vector Rot = WROT();

	return DirectX::BoundingOrientedBox(WPOS2D(), WSCALE().HalfVector().AbsVector(), WROT().DegToQ());
}

