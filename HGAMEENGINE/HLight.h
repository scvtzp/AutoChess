#pragma once
#include "HGAMECOM.h"
#include <HGAMEDEBUG.h>
#include <set>
#include "HRenderingDataStructer.h"

// 0, 1, 2

// 라이트를 삭제하지 않는다는 가정하에 이렇게 만든 구조이다.
// HLight가 생긴곳에 방향을 알수있는 오브젝트도 만들어주세요.
class HLight : public HACTORCOM
{
	friend class Game_Trans;

private:
    std::list<LightData*> DataList;


	// 나는 1 3 4 5 
	std::set<int> m_VIEWRENDERERS;
	Game_Vector LightPower;

public:
	friend HGAMESCENE;

// 필요 없는데 자기 욕망을 위해서 뭔가를 구현한다.
// 언제나 끄는 기능

    // 랜더그룹
    // 내가 어느 랜더그룹을 비추는 빛이냐
public:
	// 그 뒤로 몇개가 들어올지
	// 가변인자이다.
	template<typename ... REST>
	void Init(int _Order, REST ... _Arg)
	{
		AddRenderGroup(_Arg...);
		InitSetting(_Order);
	}


	void Init(int _Order, int _Index = 0)
	{
		AddRenderGroup(_Index);
		InitSetting(_Order);
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
	void AddRenderGroup() {}

public:


	void InitSetting(int _Order);
	void LightUpdate();

	// 카메라랑 똑같습니다.

	// 빛도 카메라처럼 다른애들을 한번 그려줘야 한다.
private:
	float m_Near;
	float m_Far;
	Game_Vector ViewSize;
	float Qulity;
	HMATRIX m_VIEW;
	HMATRIX m_PROJ;

	Game_Ptr<Game_RenderTarget> ShadowDepthTarget;

public:
	Game_Ptr<Game_RenderTarget> ShadowTarget() 
	{
		return ShadowDepthTarget;
	}

};

