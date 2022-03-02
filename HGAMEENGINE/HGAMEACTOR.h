#pragma once
#include <HLISTOBJMGR.h>
#include "Game_Virtualprgoress.h"

// 이녀석은 기능을 관리하는 기능만 존재한다.
// 이 엑터는 주인공이 될꺼야.
class Game_Trans;
class HACTORCOM;
class HGAMESCENE;
class Game_Actor : public Game_Ref, public Game_Virtualprgoress, public HUPDATELISTOBJMGR<HACTORCOM>
{
public:
	template< typename _Type > 
	friend class HUPDATELISTOBJMGR;

	friend HGAMESCENE;
	friend Game_Trans;

public:
	template<typename COMTYPE, typename ... REST>
	Game_Ptr<COMTYPE> CreateComName(const Game_String& _Name = L"", REST ... _ARG)
	{
		Game_Ptr<COMTYPE> NewCom = HLISTOBJMGR<HACTORCOM>::CreateObj<COMTYPE>(_Name);
		NewCom->ACTOR(this);
		NewCom->Init(_ARG...);
		return NewCom;
	}

	template<typename COMTYPE, typename ... REST>
	Game_Ptr<COMTYPE> CreateCom(REST ... _ARG)
	{
		Game_Ptr<COMTYPE> NewCom = HLISTOBJMGR<HACTORCOM>::CreateObj<COMTYPE>();
		NewCom->ACTOR(this);
		NewCom->Init(_ARG...);
		return NewCom;
	}

private:
	HGAMESCENE* m_SCENE;
	Game_Trans* m_TRANS;

public:
	Game_Trans* TRANS();
	HGAMESCENE* SCENE();

public:
	template<typename FINDTYPE>
	FINDTYPE* FindComName(std::wstring _Name)
	{
		return FindObjName<FINDTYPE>(_Name);
	}


	template<typename FINDTYPE>
	Game_Ptr<FINDTYPE> FindComType()
	{
		return FindObjType<FINDTYPE>();
	}

	template<typename FINDTYPE>
	std::list<Game_Ptr<FINDTYPE>> FindComTypeAll()
	{
		return FindObjTypeAll<FINDTYPE>();
	}

private:
	void UpdatePrev() 	override;
	void Update() 	override;
	void UpdateNext() 	override;
	void RenderPrev() 	override;
	void RenderNext() 	override;
	void CollisionPrev() override;
	void CollisionNext() override;
	void SceneChangeEnd() 	override;
	void SceneChangeStart() override;
	void Release() override;


};

