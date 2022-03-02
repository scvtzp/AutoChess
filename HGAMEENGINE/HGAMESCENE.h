#pragma once
#include <HMGR.h>
#include <HGAMESTRING.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <HLISTOBJMGR.h>
#include "Game_Virtualprgoress.h"
#include "HGAMECOM.h"
#include "HRenderingDataStructer.h"


class HCAM;
class Game_Collision;
class Game_Trans;
class HLight;
class Game_Renderer;
class HSCENECOM;
class Game_Actor;
class HGAMESCENE : public HMGR<HGAMESCENE>
	, public Game_Virtualprgoress
	, HUPDATELISTOBJMGR<Game_Actor>
	, HUPDATELISTOBJMGR<HSCENECOM>
{
public:
	static Game_Ptr<HGAMESCENE> m_CurScene;
	static Game_Ptr<HGAMESCENE> m_NextScene;
	static void Progress();

	static Game_Ptr<HGAMESCENE> Create(const Game_String& _Name)
	{
		Game_Ptr<HGAMESCENE> Res = CreateToInsert(_Name);
		return Res;
	}

	template<typename STARTCOM>
	static Game_Ptr<HGAMESCENE> Create(const Game_String& _Name)
	{
		Game_Ptr<HGAMESCENE> Res = CreateToInsert(_Name);
		Res->CreateCom<STARTCOM>();
		return Res;
	}
	static void ChangeScene(const Game_String& _Name);

public:
	Game_Ptr<Game_Actor> CreateActor(const Game_String& _Name = L"");

	template<typename COMTYPE>
	Game_Ptr<COMTYPE> CreateCom(const Game_String& _Name = L"")
	{
		Game_Ptr<COMTYPE> NewCom = HLISTOBJMGR<HSCENECOM>::CreateObj<COMTYPE>(_Name);
		NewCom->Scene(this);
		NewCom->Init();
		return NewCom;
	}

	template<typename COMTYPE>
	Game_Ptr<COMTYPE> FindCom(const Game_String& _Name = L"")
	{
		return HLISTOBJMGR<HSCENECOM>::FindObjType<COMTYPE>();
	}


public:
	void UpdatePrev() 	override;
	void Update() 	override;
	void UpdateNext() 	override;

	void RenderPrev() 	override;
	void Render();
	void RenderNext() 	override;

	void CollisionPrev() override;
	void Collision();
	void CollisionNext() override;

	void SceneChangeEnd() 	override;
	void SceneChangeStart() override;
	void Release() override;


	void ActorClear();
	//////////////////////////////////////////////// trans 관리
private:
	friend Game_Trans;
	// 이렇게 따로 모아 놓겠다.
	std::list<Game_Ptr<Game_Trans>> m_AllTrans;

	//////////////////////////////////////////////// Renderer 관리

/// <summary>
/// /////////////////////////
/// </summary>
	/// 최상위에는 그룹이 있고
	// int로 한다. 1번 
	// 각 그룹은 랜더러, 라이트

	// 랜더링 자체의 주체는 카메라.

	// 그 그룹을 보는 카메라
	// 
private:
	friend HCAM;
	friend Game_Renderer;
	friend HLight;
	Game_Ptr<HCAM> m_MainCam;
	std::map<int, Game_Ptr<HCAM>> m_AllCam;

	//   Camera1 01   Camera2 12
	// 0  List  Camera1
	// 1  List  Camera1 Camera2
	// 2  List  Camera2

	std::map<int, std::vector<Game_Ptr<HLight>>> m_AllLight;
	std::map<int, ObjectLightData> m_AllLightData;

	std::map<int, std::list<Game_Ptr<Game_Renderer>>> m_AllRender;

	void PushCam(Game_Ptr<HCAM> _Cam);
	void PushLight(Game_Ptr<HLight> _Light);
	void PushRender(Game_Ptr<Game_Renderer> _Render);
	void LightCheck();

	static bool ZSORT(Game_Ptr<Game_Renderer>& _Left, Game_Ptr<Game_Renderer>& _Right);

public:
	Game_Ptr<HCAM> MainCam();
	void CreateRenderGroupLight(int _Index);
	ObjectLightData* GetLightData(int _Index);
////////////////////////////////////////// COL

private:
	friend Game_Collision;
	//        플레이어    
	std::map<int, std::set<int>> m_ColLink;
	std::map<int, std::list<Game_Ptr<Game_Collision>>> m_AllCol;

public:
	void PushCol(Game_Ptr<Game_Collision> _Col);

public:
	template<typename ... REST>
	void ColLink(int _Order, REST ... _Arg)
	{
		if (m_ColLink.end() == m_ColLink.find(_Order))
		{
			// 찾아봐.
			// 정석
			// m_ColIndex.insert(std::map<int, std::set<int>>::value_type(_Order, std::set<int>()));
			// m_ColIndex.insert(std::make_pair(_Order, std::set<int>()));
			// m_ColIndex[_Order] = std::set<int>();
			m_ColLink[_Order];
		}

		// m_ColIndex : std::map<int, std::set<int>> ;
		// m_ColIndex[_Order] : std::set<int> ;

		ColLinkPush(m_ColLink[_Order], _Arg...);
	}

	template<typename ... REST>
	void ColLinkPush(std::set<int>& _Value, int _PushIndex, REST ... _Arg)
	{
		if (_Value.end() != _Value.find(_PushIndex))
		{
			AMSG(L"같은 그룹과 두번 충돌하려고 했습니다.");
		}

		_Value.insert(_PushIndex);
		ColLinkPush(_Value, _Arg...);
	}

	void ColLinkPush(std::set<int>& _Value, int _PushIndex)
	{
		if (_Value.end() != _Value.find(_PushIndex))
		{
			AMSG(L"같은 그룹과 두번 충돌하려고 했습니다.");
		}

		_Value.insert(_PushIndex);
	}


public:
	HGAMESCENE();
	~HGAMESCENE();
};



