#pragma once
#include "HGAMECOM.h"

enum class COLTYPE
{
	SPHERE2D, // API의 서클이다.
	AABB2D,
	OBB2D,

	SPHERE3D, //구
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
	Game_Vector m_LSCALE; // 스케일이 어디라도 0이면 터트리겠다. // 크기가 0인건 존재하지 않는다는 말이니까.
	Game_Vector m_LROT;
	Game_Vector m_WPOS;
	Game_Vector m_WSCALE; // 스케일이 어디라도 0이면 터트리겠다. // 크기가 0인건 존재하지 않는다는 말이니까.
	Game_Vector m_WROT;

	bool CALARRMATCHECK[(int)MATTYPE::MATTYPE_MAX];
	HTRANSDATA m_TRANSDATA;

	// 트랜스 폼은 부모를 가질수 있다.
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
	// m_LPOS가 결정나 있어야 계산 가능.
	void CALWPOS() 
	{
		DEBUGCHECK;

		// 기존의 잘못된 코드. 부모님의 WROT영향을 미치지 자전이 포지션에 영향을 미치는가?
		// WROT()는 자전도 적용된 녀석인데 이녀석이 나의 포지션에 영향을 미치고 있었다.
		// m_WPOS = m_PARENT->WPOS() + ((m_LPOS * m_PARENT->WSCALE()).RotDegReturn(WROT()));

		// m_WPOS = m_PARENT->WPOS() + ((m_LPOS * m_PARENT->WSCALE()).RotDegReturn(m_PARENT->WROT()));
		// 스케일을 먼저하면 안됀다.

		// 코드를 읽을수 있어야 한다.
		// 빈틈없이 설명할수 있으면 자기것이다. 
		// 이걸 안하니까 크기 -1이 적용이 안된다.
		// 회전을 하고 스케일을 곱해야 한다.
		// 이걸 왜하고 있죠?

		// 월드 포스를 구하는데 로컬 포지션을 
		// 이게 월드 포지션입니다.
		// 로컬 포지션을 * m_PARENT->WSCALE() 크기를 키우죠. 크기를 먼저하니까
		// 로컬 포지션을 (m_LPOS.RotDegReturn(m_PARENT->WROT()) 회전시킨다.
		// -1스케일로 반전된 위치가 적용이 되지 않는다.

		// 두복 땡큐 내 프로그램은 발전했다.
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
	// 이것들 다 바뀌어야 한다.
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

		// 나온 결과물이
		// 순서가 맞나?
		// 여러분들이 약간의 오차가 생길수 있다는것을 기억해라.
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

		// 나온 결과물이
		// 순서가 맞나?
		// 여러분들이 약간의 오차가 생길수 있다는것을 기억해라.
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

		// 이연산을 이해하려면
		// 가상의 수를 연상해서
		// 그걸로 계산을 해봐야 한다.

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

		// 스케일의 어떤 속성이 0인걸 허용하지 않는다.

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


	// 그리고 방향이죠? 길이가 얼마죠?
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
	//      내가 _ThisType고    상대 트랜스폼        상대는 _OtherType고
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