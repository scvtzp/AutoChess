#pragma once
#include "HGAMECOM.h"

enum class COLTYPE
{
	SPHERE2D, // API�� ��Ŭ�̴�.
	AABB2D,
	OBB2D,

	SPHERE3D, //��
	AABB3D, //	
	OBB3D,
	RAY3D,

	MAX,
};

struct RAY
{
	float4 Start;
	float4 Dir;
};


struct LightData;
class HCAM;
class Game_Trans : public HACTORCOM
{
private:
	class Game_ColStart
	{
	public:
		Game_ColStart();
	};
	friend Game_ColStart;
	static Game_ColStart Starter;

public:
	static bool __stdcall SPHERE2DCOLOBB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall SPHERE2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall SPHERE2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall AABB2DCOLOBB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall AABB2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall AABB2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall OBB2DCOLOBB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall OBB2DCOLAABB2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall OBB2DCOLSPHERE2D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall SPHERE3D_COL_SPHERE3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);
	static bool __stdcall AABB3D_COL_AABB3D(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);

	static bool (__stdcall *COLFUNC[(int)COLTYPE::MAX][(int)COLTYPE::MAX])(Game_Ptr<Game_Trans> _Left, Game_Ptr<Game_Trans> _Right);

protected:
	Game_Vector m_LPOS;
	Game_Vector m_LSCALE; // �������� ���� 0�̸� ��Ʈ���ڴ�. // ũ�Ⱑ 0�ΰ� �������� �ʴ´ٴ� ���̴ϱ�.
	Game_Vector m_LROT;
	Game_Vector m_WPOS;
	Game_Vector m_WSCALE; // �������� ���� 0�̸� ��Ʈ���ڴ�. // ũ�Ⱑ 0�ΰ� �������� �ʴ´ٴ� ���̴ϱ�.
	Game_Vector m_WROT;

	bool CALARRMATCHECK[(int)MATTYPE::MATTYPE_MAX];
	HTRANSDATA m_TRANSDATA;

	// Ʈ���� ���� �θ� ������ �ִ�.
	Game_Trans* m_PARENT;
	std::list<Game_Trans*> m_CHILDLIST;

public:
	HTRANSDATA TRANSDATA() {
		return m_TRANSDATA;
	}

	const HTRANSDATA& CRTRANSDATA() {
		return m_TRANSDATA;
	}

	HTRANSDATA* PTRANSDATA() {
		return &m_TRANSDATA;
	}


public:
	Game_Trans* PARENT()
	{
		return m_PARENT;
	}

	void PARENT(Game_Ptr<Game_Trans> _Parent);


public:
	Game_Trans();

public:
	Game_Vector LPOS() { return m_LPOS; }
	Game_Vector LSCALE() { return m_LSCALE; }
	Game_Vector LROT() { return m_LROT; }
	Game_Vector WPOS() { 
		return m_WPOS; 
	}

	const Game_Vector& CRWPOS() {
		return m_WPOS;
	}

	Game_Vector WPOS2D() 
	{
		return Game_Vector(m_WPOS.x, m_WPOS.y, 0.0f, m_WPOS.w);
	}
	Game_Vector WSCALE() { return m_WSCALE; }
	Game_Vector WROT() { return m_WROT; }


private:
	// m_LPOS�� ������ �־�� ��� ����.
	void CALWPOS() 
	{
		DEBUGCHECK;

		// ������ �߸��� �ڵ�. �θ���� WROT������ ��ġ�� ������ �����ǿ� ������ ��ġ�°�?
		// WROT()�� ������ ����� �༮�ε� �̳༮�� ���� �����ǿ� ������ ��ġ�� �־���.
		// m_WPOS = m_PARENT->WPOS() + ((m_LPOS * m_PARENT->WSCALE()).RotDegReturn(WROT()));

		// m_WPOS = m_PARENT->WPOS() + ((m_LPOS * m_PARENT->WSCALE()).RotDegReturn(m_PARENT->WROT()));
		// �������� �����ϸ� �ȉ´�.

		// �ڵ带 ������ �־�� �Ѵ�.
		// ��ƴ���� �����Ҽ� ������ �ڱ���̴�. 
		// �̰� ���ϴϱ� ũ�� -1�� ������ �ȵȴ�.
		// ȸ���� �ϰ� �������� ���ؾ� �Ѵ�.
		// �̰� ���ϰ� ����?

		// ���� ������ ���ϴµ� ���� �������� 
		// �̰� ���� �������Դϴ�.
		// ���� �������� * m_PARENT->WSCALE() ũ�⸦ Ű����. ũ�⸦ �����ϴϱ�
		// ���� �������� (m_LPOS.RotDegReturn(m_PARENT->WROT()) ȸ����Ų��.
		// -1�����Ϸ� ������ ��ġ�� ������ ���� �ʴ´�.

		// �κ� ��ť �� ���α׷��� �����ߴ�.
		m_WPOS = m_PARENT->WPOS() + ((m_LPOS.RotDegReturn(m_PARENT->WROT()) * m_PARENT->WSCALE()));
	}

	void CALCHILDPOS() {
		for (auto& _Child : m_CHILDLIST)
		{
			_Child->CALWPOS();
		}
	}

	void CALWROT()
	{
		m_WROT = m_PARENT->WROT() + m_LROT;
	}

	void CALCHILDROT() {
		for (auto& _Child : m_CHILDLIST)
		{
			_Child->CALWROT();
		}
	}

	void CALWSCALE()
	{
		m_WSCALE = m_PARENT->WSCALE() * m_LSCALE;
	}

	void CALCHILDSCALE() {
		for (auto& _Child : m_CHILDLIST)
		{
			_Child->CALWSCALE();
		}
	}

public:
	// �̰͵� �� �ٲ��� �Ѵ�.
	void LPOS(const Game_Vector& _Value)
	{
		if (nullptr == m_PARENT)
		{
			m_LPOS = m_WPOS = _Value;
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS] = true;
			CALCHILDPOS();
			return;
		}

		m_LPOS = _Value;
		CALWPOS();
		CALCHILDPOS();
		CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS] = true;
	}
	void WPOS(const Game_Vector& _Value)
	{
		if (nullptr == m_PARENT)
		{
			m_LPOS = m_WPOS = _Value;
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS] = true;
			CALCHILDPOS();
			return;
		}

		// ���� �������
		// ������ �³�?
		// �����е��� �ణ�� ������ ����� �ִٴ°��� ����ض�.
		m_LPOS = ((_Value - m_PARENT->WPOS()) / m_PARENT->WSCALE()).RotDegReturn(-WROT());
		CALWPOS();
		CALCHILDPOS();


		CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS] = true;
	}

	void WPOS2D(const Game_Vector& _Value, float _Z = 0.0F)
	{
		Game_Vector Vec2D = _Value;

		Vec2D.z = _Z;

		if (nullptr == m_PARENT)
		{
			m_LPOS = m_WPOS = Vec2D;
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS] = true;
			CALCHILDPOS();
			return;
		}

		// ���� �������
		// ������ �³�?
		// �����е��� �ణ�� ������ ����� �ִٴ°��� ����ض�.
		m_LPOS = ((Vec2D - m_PARENT->WPOS()) / m_PARENT->WSCALE()).RotDegReturn(-m_PARENT->WROT());
		CALWPOS();
		CALCHILDPOS();


		CALARRMATCHECK[(int)MATTYPE::MATTYPE_POS] = true;
	}

	void LROT(const Game_Vector& _Value)
	{
		if (nullptr == m_PARENT)
		{
			m_LROT = m_WROT = _Value;
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_ROT] = true;
			CALCHILDROT();
			CALCHILDPOS();
			return;
		}

		m_LROT = _Value;
		CALWROT();
		CALCHILDROT();
		CALCHILDPOS();

		CALARRMATCHECK[(int)MATTYPE::MATTYPE_ROT] = true;

	}

	void WROT(const Game_Vector& _Value)
	{
		if (nullptr == m_PARENT)
		{
			m_LROT = m_WROT = _Value;
			CALCHILDROT();
			CALCHILDPOS();
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_ROT] = true;
			return;
		}

		// �̿����� �����Ϸ���
		// ������ ���� �����ؼ�
		// �װɷ� ����� �غ��� �Ѵ�.

		//0 0 0  0 0 90 - 0 0 90
		m_LROT = _Value - m_PARENT->WROT();
		CALWROT();
		CALCHILDROT();
		CALCHILDPOS();

		CALARRMATCHECK[(int)MATTYPE::MATTYPE_ROT] = true;
	}


	void LSCALE(const Game_Vector& _Value)
	{
		if (nullptr == m_PARENT)
		{
			m_LSCALE = m_WSCALE = _Value;
			CALCHILDSCALE();
			CALCHILDPOS();
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_SCALE] = true;
			return;
		}

		m_LSCALE = _Value;
		CALWSCALE();
		CALCHILDSCALE();
		CALCHILDPOS();
		CALARRMATCHECK[(int)MATTYPE::MATTYPE_SCALE] = true;

	}
	void WSCALE(const Game_Vector& _Value)
	{
		if (nullptr == m_PARENT)
		{
			m_LSCALE = m_WSCALE = _Value;
			CALCHILDSCALE();
			CALCHILDPOS();
			CALARRMATCHECK[(int)MATTYPE::MATTYPE_SCALE] = true;
			return;
		}

		// �������� � �Ӽ��� 0�ΰ� ������� �ʴ´�.

		m_LSCALE = _Value / m_PARENT->WSCALE();
		CALWSCALE();
		CALCHILDSCALE();
		CALCHILDPOS();

		CALARRMATCHECK[(int)MATTYPE::MATTYPE_SCALE] = true;
	}

	void LMOVE(const Game_Vector& _Value) { LPOS(LPOS() + _Value); }
	void WMOVE(const Game_Vector& _Value) { WPOS(WPOS() + _Value); }

	void LROTADDX(float _Value) { m_LROT.x += _Value;		LROT(m_LROT); }
	void LROTADDY(float _Value) { 
		m_LROT.y += _Value;		
		LROT(m_LROT); 
	}
	void LROTADDZ(float _Value) { m_LROT.z += _Value;		LROT(m_LROT); }
	void WROTADDX(float _Value) {
		m_WROT.x += _Value;		
		WROT(m_WROT);
	}
	void WROTADDY(float _Value) {
		m_WROT.y += _Value;		
		WROT(m_WROT);
	}
	void WROTADDZ(float _Value) { 
		m_WROT.z += _Value;		
		WROT(m_WROT); 
	}


	// �׸��� ��������? ���̰� ����?
	Game_Vector LFORWARD() { return m_TRANSDATA.ROT.ArrV[2]; }
	Game_Vector LBACK() { return -m_TRANSDATA.ROT.ArrV[2]; }
	Game_Vector LRIGHT() { return m_TRANSDATA.ROT.ArrV[0]; }
	Game_Vector LLEFT() { return -m_TRANSDATA.ROT.ArrV[0]; }
	Game_Vector LUP() { return m_TRANSDATA.ROT.ArrV[1]; }
	Game_Vector LDOWN() { return m_TRANSDATA.ROT.ArrV[1]; }

	Game_Vector WFORWARD() { return m_TRANSDATA.WWORLD.ArrV[2].NORMAL3DTORETURN(); }
	Game_Vector WBACK() { return -m_TRANSDATA.WWORLD.ArrV[2].NORMAL3DTORETURN(); }
	Game_Vector WRIGHT() { return m_TRANSDATA.WWORLD.ArrV[0].NORMAL3DTORETURN(); }
	Game_Vector WLEFT() { return -m_TRANSDATA.WWORLD.ArrV[0].NORMAL3DTORETURN(); }
	Game_Vector WUP() { return m_TRANSDATA.WWORLD.ArrV[1].NORMAL3DTORETURN(); }
	Game_Vector WDOWN() { return -m_TRANSDATA.WWORLD.ArrV[1].NORMAL3DTORETURN(); }

public:
	//      ���� _ThisType��    ��� Ʈ������        ���� _OtherType��
	bool Col(COLTYPE _ThisType, Game_Ptr<Game_Trans> _Trans, COLTYPE _OtherType);

public:
	DirectX::BoundingBox AABB2D();
	DirectX::BoundingSphere SPHERE2D();
	DirectX::BoundingOrientedBox OBB2D();
	DirectX::BoundingOrientedBox RAY3D();

public:
	void Init() override;
	void TransUpdate();
	void CamUpdate(Game_Ptr<HCAM> _Cam);
	void LightUpdate(const LightData& _Light);
};