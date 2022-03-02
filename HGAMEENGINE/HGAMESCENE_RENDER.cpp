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

	// ����۰� ���� ���� �ȴ�.
	Game_Device::MAINOBJ()->RenderStart();

	for (auto& _LightGroup : m_AllLight)
	{
		for (auto& _Light : _LightGroup.second)
		{
			_Light->LightUpdate();
		}
	}

	

	// ��� ķŸ���� ����� �ܰ踦 ���⼭ ó��.
	//for (auto& _Cam : m_AllCam)
	//{
	//}

	// ķ�� ��� ������.
	for (auto& _Cam : m_AllCam)
	{
		{
			_Cam.second->ForwardCamTarget()->Clear(false);
			_Cam.second->ForwardCamTarget()->Setting();

			// ķ�� �ٶ󺸴� ���� �׷��� �ε����� ��� Ž���ϰ�
			// ���� ���� �ֵ��� �������� ����?
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

				// �� �ε����� �ش��ϴ� ���� �׷��� ã�´�.
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

			// ķ�� �ٶ󺸴� ���� �׷��� �ε����� ��� Ž���ϰ�
			for (int RenderOrder : _Cam.second->m_VIEWRENDERERS)
			{
				ObjectLightData* LightGroup = GetLightData(RenderOrder);

				for (size_t i = 0; i < LightGroup->LightCount[0]; i++)
				{
					// LightGroup->ArrLightData[i].CamPos = _Cam.second->m_VIEW.MulOne(_Cam.second->TRANS()->WPOS());
				}

				// �� �ε����� �ش��ϴ� ���� �׷��� ã�´�.
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

					// ���⼭ �׸��ڸ� �׷��ش�.
					for (auto& _Render : RenderGroup->second)
					{
						_Render->ShadowRender(LightGroup->ArrLightData[i]);
					}

					LightGroup->ArrLightData[i].m_CamViewInverse = _Cam.second->m_VIEW.InverseReturn();

					// �̳༮�� ����� �Ҷ� ����� �Ѵ�.
					// m_AllLight[RenderOrder][i]->ShadowTarget()

					// �̶� 

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

	// ����Ʈ ���μ��� ����.

	// 

	// ī�޶� �׷��� �༮�� �׸��� �ȴ�.
	for (auto& _Cam : m_AllCam)
	{
		_Cam.second->CamMergePrevEffect();
		_Cam.second->CamTargetMerge();
		_Cam.second->CamMergeNextEffect();

		// ������� 0��° �ؽ�ó���ٰ� 
		// _Cam.second->CamTarget()�� 0��° �ؽ�ó�� �����϶�.
		
		Game_Device::MAINOBJ()->BackBufferRenderTarget()->Merge(_Cam.second->CamTarget());
		
		// Game_Device::MAINOBJ()->BackBufferRenderTarget() ���⿡�ٰ� ������ �ϸ� �ȴ�.
	}

	{
		Game_3D_Debug::DebugRender();
	}

	Game_Device::MAINOBJ()->DataRenderTarget()->Clear(false);

	// ����Ʈ ���μ��� ����.

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