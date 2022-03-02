#include "HGAMESCENE.h"
#include "HGAMEACTOR.h"
#include "Game_Device.h"
#include "Game_Trans.h"
#include "HCAM.h"
#include "HRENDERER.h"
#include "HCOL.h"
#include "Game_3D_Debug.h"
#include <HGAMEINPUT.h>
#include <HGAMETIME.h>
#include <HGAMEMOVIE.h>
#include "HRenderTarget.h"
#include "HLight.h"

Game_Ptr<HGAMESCENE> HGAMESCENE::m_CurScene = nullptr;
Game_Ptr<HGAMESCENE> HGAMESCENE::m_NextScene = nullptr;

HGAMESCENE::HGAMESCENE() {

}
HGAMESCENE::~HGAMESCENE() {

}

void HGAMESCENE::ChangeScene(const Game_String& _Name) 
{
	m_NextScene = Find(_Name);

	if (nullptr == m_NextScene)
	{
		AMSG(L"존재하지 않는 씬을 체인지 하려고 했습니다.");
	}

}

Game_Ptr<Game_Actor> HGAMESCENE::CreateActor(const Game_String& _Name) 
{
	// HUPDATELISTOBJMGR<Game_Actor>::m_ObjList

	Game_Ptr<Game_Actor> Actor = HUPDATELISTOBJMGR<Game_Actor>::CreateObj<Game_Actor>(_Name);
	// 이걸 집어넣어주니까 액터는 위치가 존재하게 된다.
	// 컴포넌트 구조에서는 위치라는 것도 기능(컴포넌트)이다.
	// 씬을 알게해줘야 그걸 가지고 여러가지 일을 할수 있다.
	Actor->m_SCENE = this;
	Actor->CreateCom<Game_Trans>();
	return Actor;
}

void HGAMESCENE::Progress() 
{
	if (nullptr != m_NextScene)
	{
		// 최초
		if (nullptr == m_CurScene)
		{
			m_CurScene = m_NextScene;
			m_CurScene->SceneChangeStart();
			m_NextScene = nullptr;
			Game_Time::TimeReset();
		}
		else 
		{
			m_CurScene->SceneChangeEnd();
			m_CurScene = m_NextScene;
			m_CurScene->SceneChangeStart();
			m_NextScene = nullptr;
			Game_Time::TimeReset();
		}
	}

	HGAMEINPUT::Update();
	Game_Time::Update();

	m_CurScene->UpdatePrev();
	m_CurScene->Update();
	m_CurScene->UpdateNext();

	m_CurScene->RenderPrev();

	m_CurScene->Render();
	m_CurScene->RenderNext();


	m_CurScene->CollisionPrev();
	m_CurScene->Collision();
	// 콜리전
	m_CurScene->CollisionNext();

	m_CurScene->Release();
}

bool HGAMESCENE::ZSORT(Game_Ptr<Game_Renderer>& _Left, Game_Ptr<Game_Renderer>& _Right)
{
	Game_Vector LeftZ = _Left->WPOS();
	Game_Vector RightZ = _Right->WPOS();

	// 랜더러가 트랜스폼을 상속받은 이유
	return _Left->WPOS().z > _Right->WPOS().z;
}


void HGAMESCENE::Collision() 
{
	// 누구누구들이 충돌해야 하는가?
	// std::map<int, std::set<int>> m_ColLink;
	// 그 그룹은 이거다.
	// std::map<int, std::list<Game_Ptr<Game_Collision>>> m_AllCol;
	// std::list<Game_Ptr<Game_Collision>>

	for (auto& LeftIndex : m_ColLink)
	{
		// std::map<int, std::set<int>> m_ColLink;
		// std::map<int == 플레이어그룹
		// ColIndex.first

		// std::set<int> == 몬스터 그룹과, 골드그룹
		// ColIndex.second

		// 플레이어 그룹
		std::map<int, std::list<Game_Ptr<Game_Collision>>>::iterator LeftGroup = m_AllCol.find(LeftIndex.first);
		if (LeftGroup == m_AllCol.end())
		{
			continue;
		}

		// 자료형 std::list<Game_Ptr<Game_Collision>>
		if (true == LeftGroup->second.empty())
		{
			continue;
		}

		for (int RightIndex : LeftIndex.second)
		{
			std::map<int, std::list<Game_Ptr<Game_Collision>>>::iterator RightGroup = m_AllCol.find(RightIndex);

			if (RightGroup == m_AllCol.end())
			{
				continue;
			}

			if (true == RightGroup->second.empty())
			{
				continue;
			}


			if (LeftIndex.first == RightIndex)
			{
				std::list<Game_Ptr<Game_Collision>>::iterator LeftStart = LeftGroup->second.begin();

				if (false == (*LeftStart)->IsUpdate())
				{
					continue;
				}

				std::list<Game_Ptr<Game_Collision>>::iterator LeftEnd = RightGroup->second.end();
				--LeftEnd;


				// 0 1 2 
				for (; LeftStart != LeftEnd; ++LeftStart)
				{
					std::list<Game_Ptr<Game_Collision>>::iterator RightStart = LeftStart;
					std::list<Game_Ptr<Game_Collision>>::iterator RightEnd = RightGroup->second.end();
					++RightStart;

					for (; RightStart != RightEnd; ++RightStart)
					{
						if (false == (*RightStart)->IsUpdate())
						{
							continue;
						}

						(*LeftStart)->ColCheck(*RightStart);
					}
				}


			}
			else 
			{


				// 자기 그룹과의 충돌은 생각 안한겁니다.
				for (auto& LeftCol : LeftGroup->second)
				{

					if (false == LeftCol->IsUpdate())
					{
						continue;
					}

					for (auto& RightCol : RightGroup->second)
					{
						if (false == RightCol->IsUpdate())
						{
							continue;
						}

						LeftCol->ColCheck(RightCol);
					}
				}
			}


		}

		// 이젠 이 그룹은 충돌할수 있죠.
	}


}

void HGAMESCENE::UpdatePrev() 			  
{
	HUPDATELISTOBJMGR<HSCENECOM>::UpdatePrev();
	HUPDATELISTOBJMGR<Game_Actor>::UpdatePrev();
}
void HGAMESCENE::Update() 				  
{
	HUPDATELISTOBJMGR<HSCENECOM>::Update();
	HUPDATELISTOBJMGR<Game_Actor>::Update();
}
void HGAMESCENE::UpdateNext() 			  
{
	HUPDATELISTOBJMGR<HSCENECOM>::UpdateNext();
	HUPDATELISTOBJMGR<Game_Actor>::UpdateNext();

}
void HGAMESCENE::RenderPrev() 			  
{
	HUPDATELISTOBJMGR<HSCENECOM>::RenderPrev();
	HUPDATELISTOBJMGR<Game_Actor>::RenderPrev();

	// 부모가 없는 가장 최상의 트랜스폼을 모아놓은 자료구조인것이다.
	for (auto& _TransForm : m_AllTrans)
	{
		_TransForm->TransUpdate();
	}
}
void HGAMESCENE::RenderNext() 			  
{
	HUPDATELISTOBJMGR<HSCENECOM>::RenderNext();
	HUPDATELISTOBJMGR<Game_Actor>::RenderNext();
}
void HGAMESCENE::CollisionPrev()		  
{
	HUPDATELISTOBJMGR<HSCENECOM>::CollisionPrev();
	HUPDATELISTOBJMGR<Game_Actor>::CollisionPrev();
}
void HGAMESCENE::CollisionNext()		  
{
	HUPDATELISTOBJMGR<HSCENECOM>::CollisionNext();
	HUPDATELISTOBJMGR<Game_Actor>::CollisionNext();
}
void HGAMESCENE::SceneChangeEnd() 		  
{
	HUPDATELISTOBJMGR<HSCENECOM>::SceneChangeEnd();
	HUPDATELISTOBJMGR<Game_Actor>::SceneChangeEnd();
}
void HGAMESCENE::SceneChangeStart() 	  
{
	HUPDATELISTOBJMGR<HSCENECOM>::SceneChangeStart();
	HUPDATELISTOBJMGR<Game_Actor>::SceneChangeStart();
}

typedef std::list<HUPDATELISTOBJMGR<HSCENECOM>::LISTDATA>::iterator SceneComIter;
typedef std::list<HUPDATELISTOBJMGR<Game_Actor>::LISTDATA>::iterator ActorIter;


void HGAMESCENE::Release()
{
	// SceneComRelease
	{
		SceneComIter Start = HUPDATELISTOBJMGR<HSCENECOM>::m_ObjList.begin();
		SceneComIter End = HUPDATELISTOBJMGR<HSCENECOM>::m_ObjList.end();

		for (; Start != End; )
		{
			if (true == Start->Obj->IsDeath())
			{
				Start = HUPDATELISTOBJMGR<HSCENECOM>::m_ObjList.erase(Start);
			}
			else
			{
				++Start;
			}
		}
	}

	// Actor
	{
		// 가장 상위에 있는 녀석이 가장 마지막에 지워지는게 좋다.
		// 랜더러를 지운다.

		// Trans
		{
			std::list<Game_Ptr<Game_Trans>>::iterator Start = m_AllTrans.begin();
			std::list<Game_Ptr<Game_Trans>>::iterator End = m_AllTrans.end();

			for (; Start != End; )
			{
				if (true == (*Start)->IsDeath())
				{
					Start = m_AllTrans.erase(Start);
				}
				else
				{
					++Start;
				}
			}
		}

		// Col
		{
			std::map<int, std::list<Game_Ptr<Game_Collision>>>::iterator Start = m_AllCol.begin();
			std::map<int, std::list<Game_Ptr<Game_Collision>>>::iterator End = m_AllCol.end();

			// std::map<int, std::list<Game_Ptr<Game_Renderer>>>:: Iterator
			// first == int, second == std::list<Game_Ptr<Game_Renderer>>

			for (; Start != End; ++Start)
			{
				std::list<Game_Ptr<Game_Collision>>::iterator StartCol = Start->second.begin();
				std::list<Game_Ptr<Game_Collision>>::iterator EndCol = Start->second.end();

				for (; StartCol != EndCol; )
				{
					if (true == (*StartCol)->IsDeath())
					{
						StartCol = Start->second.erase(StartCol);
					}
					else
					{
						++StartCol;
					}

				}

			}
		}
		// Cam
		{
			std::map<int, Game_Ptr<HCAM>>::iterator Start = m_AllCam.begin();
			std::map<int, Game_Ptr<HCAM>>::iterator End = m_AllCam.end();

			for (; Start != End; )
			{
				if (true == Start->second->IsDeath())
				{
					if (m_MainCam == Start->second)
					{
						m_MainCam = nullptr;
					}
					Start = m_AllCam.erase(Start);
				}
				else
				{
					++Start;
				}
			}
		}
		// Renderer
		{
			std::map<int, std::list<Game_Ptr<Game_Renderer>>>::iterator Start = m_AllRender.begin();
			std::map<int, std::list<Game_Ptr<Game_Renderer>>>::iterator End = m_AllRender.end();

			// std::map<int, std::list<Game_Ptr<Game_Renderer>>>:: Iterator
			// first == int, second == std::list<Game_Ptr<Game_Renderer>>

			for (; Start != End; ++Start)
			{
				std::list<Game_Ptr<Game_Renderer>>::iterator StartRender = Start->second.begin();
				std::list<Game_Ptr<Game_Renderer>>::iterator EndRender = Start->second.end();

				for (; StartRender != EndRender; )
				{
					if (true == (*StartRender)->IsDeath())
					{
						StartRender = Start->second.erase(StartRender);
					}
					else
					{
						++StartRender;
					}

				}

			}
		}

		// Light
		{
			bool LightReCheck = false;

			std::map<int, std::vector<Game_Ptr<HLight>>>::iterator Start = m_AllLight.begin();
			std::map<int, std::vector<Game_Ptr<HLight>>>::iterator End = m_AllLight.end();

			// std::map<int, std::list<Game_Ptr<Game_Renderer>>>:: Iterator
			// first == int, second == std::list<Game_Ptr<Game_Renderer>>

			for (; Start != End; ++Start)
			{
				std::vector<Game_Ptr<HLight>>::iterator StartRender = Start->second.begin();
				std::vector<Game_Ptr<HLight>>::iterator EndRender = Start->second.end();

				for (; StartRender != EndRender; )
				{
					if (true == (*StartRender)->IsDeath())
					{
						LightReCheck = true;
						// 라이트는 라이트 구조가 있기 때문에 이게 삭제된 순간.
						// 데이터에 대해서 재정리를 해줘야 한다.
						StartRender = Start->second.erase(StartRender);
					}
					else
					{
						++StartRender;
					}

				}

			}

			if (true == LightReCheck)
			{
				LightCheck();
			}

			// LightReCheck = true;
		}


		{
			ActorIter Start = HUPDATELISTOBJMGR<Game_Actor>::m_ObjList.begin();
			ActorIter End = HUPDATELISTOBJMGR<Game_Actor>::m_ObjList.end();

			for (; Start != End; )
			{
				if (true == Start->Obj->IsDeath())
				{
					Start->Obj->Release();
					Start = HUPDATELISTOBJMGR<Game_Actor>::m_ObjList.erase(Start);
				}
				else
				{
					++Start;
				}
			}
		}
	}
}

void HGAMESCENE::PushCam(Game_Ptr<HCAM> _Cam) 
{
	if (m_AllCam.end() != m_AllCam.find(_Cam->Order()))
	{
		AMSG(L"카메라의 오더가 겹쳤습니다.");
	}

	for (int Value : _Cam->m_VIEWRENDERERS)
	{
		CreateRenderGroupLight(Value);
	}

	if (nullptr == m_MainCam)
	{
		m_MainCam = _Cam;
	}

	m_AllCam[_Cam->Order()] = _Cam;
}

ObjectLightData* HGAMESCENE::GetLightData(int _Index)
{
	return &(m_AllLightData[_Index]);
}

void HGAMESCENE::CreateRenderGroupLight(int _Index) 
{
	if (m_AllRender.end() != m_AllRender.find(_Index))
	{
		return;
	}
	// 라이트도 생깁니다.
	m_AllRender.insert(
		std::map<int, std::list<Game_Ptr<Game_Renderer>>>::value_type(_Index,
			std::list<Game_Ptr<Game_Renderer>>()));

	m_AllLight.insert(
		std::map<int, std::vector<Game_Ptr<HLight>>>::value_type(_Index,
			std::vector<Game_Ptr<HLight>>()));

	m_AllLightData.insert( std::map<int, ObjectLightData>::value_type(_Index, ObjectLightData()));
}

void HGAMESCENE::PushLight(Game_Ptr<HLight> _Light) 
{
	// 혹시나 그룹이 없으면 만들고.
	for (int Value : _Light->m_VIEWRENDERERS)
	{
		CreateRenderGroupLight(Value);
	}

	for (int Value : _Light->m_VIEWRENDERERS)
	{
		m_AllLight[Value].push_back(_Light);
		LightData& LightData = m_AllLightData[Value].ArrLightData[m_AllLightData[Value].LightCount[0]];
		_Light->DataList.push_back(&LightData);

		++m_AllLightData[Value].LightCount[0];
	}
}

void HGAMESCENE::PushRender(Game_Ptr<Game_Renderer> _Render)
{
	if (m_AllRender.end() == m_AllRender.find(_Render->Order()))
	{
		CreateRenderGroupLight(_Render->Order());
	}

	m_AllRender[_Render->Order()].push_back(_Render);
}

void HGAMESCENE::PushCol(Game_Ptr<Game_Collision> _Col) 
{
	if (m_AllCol.end() == m_AllCol.find(_Col->Order()))
	{
		m_AllCol.insert(
			std::map<int, std::list<Game_Ptr<Game_Collision>>>::value_type(_Col->Order(),
				std::list<Game_Ptr<Game_Collision>>()));
	}

	m_AllCol[_Col->Order()].push_back(_Col);
}



void HGAMESCENE::ActorClear() 
{
	m_AllCol.clear();
	m_AllCam.clear();
	m_AllRender.clear();
	m_AllTrans.clear();
	HUPDATELISTOBJMGR<Game_Actor>::m_ObjList.clear();
	m_MainCam = nullptr;
}

Game_Ptr<HCAM> HGAMESCENE::MainCam() {
	return m_MainCam;
}