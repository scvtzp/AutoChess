#pragma once
#include "HGAMECOM.h"
#include <HGAMEDEBUG.h>
#include <set>
#include "HRenderingDataStructer.h"

// 0, 1, 2

// ����Ʈ�� �������� �ʴ´ٴ� �����Ͽ� �̷��� ���� �����̴�.
// HLight�� ������� ������ �˼��ִ� ������Ʈ�� ������ּ���.
class HLight : public HACTORCOM
{
	friend class Game_Trans;

private:
    std::list<LightData*> DataList;


	// ���� 1 3 4 5 
	std::set<int> m_VIEWRENDERERS;
	Game_Vector LightPower;

public:
	friend HGAMESCENE;

// �ʿ� ���µ� �ڱ� ����� ���ؼ� ������ �����Ѵ�.
// ������ ���� ���

    // �����׷�
    // ���� ��� �����׷��� ���ߴ� ���̳�
public:
	// �� �ڷ� ��� ������
	// ���������̴�.
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
	void AddRenderGroup() {}

public:


	void InitSetting(int _Order);
	void LightUpdate();

	// ī�޶�� �Ȱ����ϴ�.

	// ���� ī�޶�ó�� �ٸ��ֵ��� �ѹ� �׷���� �Ѵ�.
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

