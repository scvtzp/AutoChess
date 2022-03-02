#pragma once
#include <HGAMEDEBUG.h>
#include "HGAMECOM.h"
#include <set>
#include "HRenderingDataStructer.h"
#include <HLISTOBJMGR.h>
#include <HPTR.h>

enum class CAMMODE
{
	ORTH, // 직교투영
	PERS, // 원근투영
};

enum class RenderType
{
	Default = 0, //기본
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
	// 버텍스를 이 두가지 행렬로 변환시켜서 자신의 시야안에서 어떻게 표현한다.
	// 누가 이걸 또 해야하느냐?
	// 빛이 이걸 해야 한다.
	HMATRIX m_VIEW;
	HMATRIX m_PROJ;
	Game_Vector m_CAMSIZE;
	float m_CAMNEAR;
	float m_CAMFAR;
	float m_CAMFOV;

	// 내가 바라보는 랜더러들의 그룹을 카메라가 알고 있어야 한다.
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
	// 그 뒤로 몇개가 들어올지
	// 가변인자이다.
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
			AMSG(L"카메라가 같은 그룹을 두번 보려고 했습니다.");
		}

		m_VIEWRENDERERS.insert(_GroupIndex);
		AddRenderGroup(_Arg...);
	}

	void AddRenderGroup(int _GroupIndex)
	{
		if (m_VIEWRENDERERS.end() != m_VIEWRENDERERS.find(_GroupIndex))
		{
			AMSG(L"카메라가 같은 그룹을 두번 보려고 했습니다.");
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