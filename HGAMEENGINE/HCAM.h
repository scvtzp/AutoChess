#pragma once
#include <HGAMEDEBUG.h>
#include "HGAMECOM.h"
#include <set>
#include "HRenderingDataStructer.h"
#include <HLISTOBJMGR.h>
#include <HPTR.h>

enum class CAMMODE
{
	ORTH, // ��������
	PERS, // ��������
};

enum class RenderType
{
	Default = 0, //�⺻
	Ui = 999,
	Front = 1234,

};

class HTEXTRUE;
class HCAM;
class Game_RenderTarget;
class HFilter : public Game_Ref 
{
	friend HCAM;
public:
	HCAM* m_Parent;
	Game_Ptr<Game_RenderTarget> OutTarget;

public:
	void Init() {}
	void StartTargetCreate();

private:
	virtual void CamMergePrevEffect() {}
	virtual void CamMergeNextEffect() {}
	virtual void DefferdMergePrevEffect() {}
	virtual void DefferdMergeNextEffect() {}
};

class HTEXTURE;
class Game_Renderer;
class HRENDERPLAYER;
class Game_RenderTarget;
class HCAM : public HACTORCOM, public HLISTOBJMGR<HFilter>
{
public:
	friend Game_Trans;
	friend HGAMESCENE;

private:
	CAMMODE m_MODE;
	// ���ؽ��� �� �ΰ��� ��ķ� ��ȯ���Ѽ� �ڽ��� �þ߾ȿ��� ��� ǥ���Ѵ�.
	// ���� �̰� �� �ؾ��ϴ���?
	// ���� �̰� �ؾ� �Ѵ�.
	HMATRIX m_VIEW;
	HMATRIX m_PROJ;
	Game_Vector m_CAMSIZE;
	float m_CAMNEAR;
	float m_CAMFAR;
	float m_CAMFOV;

	// ���� �ٶ󺸴� ���������� �׷��� ī�޶� �˰� �־�� �Ѵ�.
	std::set<int> m_VIEWRENDERERS;

	Game_Ptr<HRENDERPLAYER> DefferdLightPlayer;
	Game_Ptr<HRENDERPLAYER> DefferdMergePlayer;

public:
	Game_Vector CAMSIZE() { return m_CAMSIZE; }
	void CAMSIZE(const Game_Vector _Value) { m_CAMSIZE = _Value; }

	float CAMFOV() { return m_CAMFOV; }
	float CAMNEAR() { return m_CAMNEAR; }
	float CAMFAR() { return m_CAMFAR; }

	void CAMFOV(float _Value) { m_CAMFAR = _Value; }
	void CAMNEAR(float _Value) { m_CAMNEAR = _Value; }
	void CAMFAR(float _Value) { m_CAMFAR = _Value; }

	//CBTRANSDATA.V.VIEWLH(CamPos, Game_Vector::FORWARD, Game_Vector::UP);
	//CBTRANSDATA.P.ORTHLH(CamScale, 0.1f, 100.0f);

private:
	void StartSetting(int _Order);

public:
	CAMMODE MODE() { return m_MODE; }
	void MODE(CAMMODE _MODE) { m_MODE = _MODE; }

public:
	// �� �ڷ� ��� ������
	// ���������̴�.
	template<typename ... REST>
	void Init(int _Order, REST ... _Arg) 
	{
		StartSetting(_Order);
		AddRenderGroup(_Arg...);
	}

	template<typename ... REST>
	void AddRenderGroup(int _GroupIndex, REST ... _Arg)
	{
		if (m_VIEWRENDERERS.end() != m_VIEWRENDERERS.find(_GroupIndex))
		{
			AMSG(L"ī�޶� ���� �׷��� �ι� ������ �߽��ϴ�.");
		}

		m_VIEWRENDERERS.insert(_GroupIndex);
		AddRenderGroup(_Arg...);
	}

	void AddRenderGroup(int _GroupIndex)
	{
		if (m_VIEWRENDERERS.end() != m_VIEWRENDERERS.find(_GroupIndex))
		{
			AMSG(L"ī�޶� ���� �׷��� �ι� ������ �߽��ϴ�.");
		}

		m_VIEWRENDERERS.insert(_GroupIndex);
	}

	void AddRenderGroup()	{}


	void RenderPrev() override;

	Game_Vector CamOrthMousePos2D();
	Game_Vector OrthWorldMousePos2D();

//////////////////////////////////////////
private:
	Game_Ptr<Game_RenderTarget> m_CamTarget;
	Game_Ptr<Game_RenderTarget> m_ForwardCamTarget;
	Game_Ptr<Game_RenderTarget> m_GbufferTarget;
	Game_Ptr<Game_RenderTarget> m_DefferdLightTarget;
	Game_Ptr<Game_RenderTarget> m_DefferdCamTarget;

public:
	void CamTargetMerge();
	void CalDefferdLight(const LightData& _Data, Game_Ptr<HTEXTURE> _Tex);
	void DefferdMerge();

	Game_Ptr<Game_RenderTarget> CamTarget();
	Game_Ptr<Game_RenderTarget> ForwardCamTarget();
	Game_Ptr<Game_RenderTarget> DefferdCamTarget();
	Game_Ptr<Game_RenderTarget> GbufferTarget();
	Game_Ptr<Game_RenderTarget> DefferdLightTarget();


public:
	template<typename FILTERTYPE, typename ... REST>
	Game_Ptr<FILTERTYPE> AddFilter(REST ... _ARG)
	{
		Game_Ptr<FILTERTYPE> NewCom = HLISTOBJMGR<HFilter>::CreateObj<FILTERTYPE>();
		NewCom->m_Parent = this;
		NewCom->Init(_ARG...);
		return NewCom;
	}


	void DefferdMergePrevEffect();
	void DefferdMergeNextEffect();

	void CamMergePrevEffect();
	void CamMergeNextEffect();
};