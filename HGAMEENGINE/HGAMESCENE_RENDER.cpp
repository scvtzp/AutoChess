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

void HGAMESCENE::Render()
{
	if (true == HGAMEWINDOW::MAINOBJ()->GetMoviePlayer()->IsPlay())
	{
		// HGAMEWINDOW::MAINOBJ()->GetMoviePlayer()->PrintMovie();
		// InvalidateRect(HGAMEWINDOW::MAINOBJ()->WINHWND(), nullptr, false);
		return;
	}

	// 백버퍼가 먼저 세팅 된다.
	Game_Device::MAINOBJ()->RenderStart();

	for (auto& _LightGroup : m_AllLight)
	{
		for (auto& _Light : _LightGroup.second)
		{
			_Light->LightUpdate();
		}
	}

	

	// 모든 캠타겟을 지우는 단계를 여기서 처리.
	//for (auto& _Cam : m_AllCam)
	//{
	//}

	// 캠을 모두 돌린다.
	for (auto& _Cam : m_AllCam)
	{
		{
			_Cam.second->ForwardCamTarget()->Clear(false);
			_Cam.second->ForwardCamTarget()->Setting();

			// 캠이 바라보는 랜더 그룹의 인덱스를 모두 탐색하고
			// 내가 보는 애들이 누가누가 있죠?
			for (int RenderOrder : _Cam.second->m_VIEWRENDERERS)
			{
				ObjectLightData* LightGroup = GetLightData(RenderOrder);

				for (size_t i = 0; i < LightGroup->LightCount[0]; i++)
				{
					//LightGroup->ArrLightData->LightDirInv = _Cam.second->m_VIEW.MulZero(LightGroup->ArrLightData->LightDirInv);
					//LightGroup->ArrLightData->LightDirInv.NORMAL3D();
					//LightGroup->ArrLightData->CamPos = _Cam.second->m_VIEW.MulOne(_Cam.second->TRANS()->WPOS());

					LightGroup->ArrLightData[i].LightDirInv = _Cam.second->m_VIEW.MulZero(LightGroup->ArrLightData[i].LightDirInv);
					LightGroup->ArrLightData[i].LightDirInv.NORMAL3D();
					LightGroup->ArrLightData[i].CamPos = _Cam.second->m_VIEW.MulOne(_Cam.second->TRANS()->WPOS());
				}

				// 그 인덱스에 해당하는 랜더 그룹을 찾는다.
				std::map<int, std::list<Game_Ptr<Game_Renderer>>>::iterator RenderGroup = m_AllRender.find(RenderOrder);

				if (m_AllRender.end() == RenderGroup)
				{
					continue;
				}

				// std::list<Game_Ptr<Game_Renderer>>
				for (auto& _Render : RenderGroup->second)
				{
					if (false == _Render->IsUpdate())
					{
						continue;
					}

					_Render->ForwardRender(_Cam.second);
				}
			}

			_Cam.second->GbufferTarget()->Clear(false);
			_Cam.second->GbufferTarget()->Setting();
			_Cam.second->DefferdLightTarget()->Clear(false);

			// 캠이 바라보는 랜더 그룹의 인덱스를 모두 탐색하고
			for (int RenderOrder : _Cam.second->m_VIEWRENDERERS)
			{
				ObjectLightData* LightGroup = GetLightData(RenderOrder);

				for (size_t i = 0; i < LightGroup->LightCount[0]; i++)
				{
					// LightGroup->ArrLightData[i].CamPos = _Cam.second->m_VIEW.MulOne(_Cam.second->TRANS()->WPOS());
				}

				// 그 인덱스에 해당하는 랜더 그룹을 찾는다.
				std::map<int, std::list<Game_Ptr<Game_Renderer>>>::iterator RenderGroup = m_AllRender.find(RenderOrder);

				if (m_AllRender.end() == RenderGroup)
				{
					continue;
				}

				// std::list<Game_Ptr<Game_Renderer>>
				for (auto& _Render : RenderGroup->second)
				{
					if (false == _Render->IsUpdate())
					{
						continue;
					}

					_Render->DefferdRender(_Cam.second);
				}

				for (size_t i = 0; i < LightGroup->LightCount[0]; i++)
				{

					m_AllLight[RenderOrder][i]->ShadowTarget()->Setting();

					// 여기서 그림자를 그려준다.
					for (auto& _Render : RenderGroup->second)
					{
						_Render->ShadowRender(LightGroup->ArrLightData[i]);
					}

					LightGroup->ArrLightData[i].m_CamViewInverse = _Cam.second->m_VIEW.InverseReturn();

					// 이녀석이 빛계산 할때 들어가줘야 한다.
					// m_AllLight[RenderOrder][i]->ShadowTarget()

					// 이때 

					_Cam.second->CalDefferdLight(LightGroup->ArrLightData[i], m_AllLight[RenderOrder][i]->ShadowTarget()->Texture(0));
				}
			}
		}	
	}


	for (auto& _Cam : m_AllCam)
	{
		_Cam.second->DefferdMergePrevEffect();
	}

	for (auto& _Cam : m_AllCam)
	{
		_Cam.second->DefferdMerge();
	}

	for (auto& _Cam : m_AllCam)
	{
		_Cam.second->DefferdMergeNextEffect();
	}

	Game_Device::MAINOBJ()->BackBufferRenderTarget()->Clear(true);
	// Game_Device::MAINOBJ()->BackBufferRenderTarget()->Setting();

	// 포스트 프로세싱 구조.

	// 

	// 카메라에 그려진 녀석을 그리면 된다.
	for (auto& _Cam : m_AllCam)
	{
		_Cam.second->CamMergePrevEffect();
		_Cam.second->CamTargetMerge();
		_Cam.second->CamMergeNextEffect();

		// 백버퍼의 0번째 텍스처에다가 
		// _Cam.second->CamTarget()의 0번째 텍스처를 머지하라.
		
		Game_Device::MAINOBJ()->BackBufferRenderTarget()->Merge(_Cam.second->CamTarget());
		
		// Game_Device::MAINOBJ()->BackBufferRenderTarget() 여기에다가 랜더링 하면 된다.
	}

	{
		Game_3D_Debug::DebugRender();
	}

	Game_Device::MAINOBJ()->DataRenderTarget()->Clear(false);

	// 포스트 프로세싱 구조.

	Game_Device::MAINOBJ()->RenderEnd();

}

void HGAMESCENE::LightCheck() 
{
	for (auto& LightDataGroup : m_AllLightData)
	{
		LightDataGroup.second.LightCount[0] = 0;
	}

	for (auto& _LightGroup : m_AllLight)
	{
		for (auto& _Light : _LightGroup.second)
		{
			for (int Value : _Light->m_VIEWRENDERERS)
			{
				LightData& LightData = m_AllLightData[Value].ArrLightData[m_AllLightData[Value].LightCount[0]];
				_Light->DataList.push_back(&LightData);
				++m_AllLightData[Value].LightCount[0];
			}
		}
	}
}