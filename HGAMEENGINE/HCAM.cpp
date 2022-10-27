#include "HCAM.h"
#include <HGAMEDEBUG.h>
#include <HGAMEWINDOW.h>
#include "HGAMEACTOR.h"
#include "Game_Trans.h"
#include "HGAMESCENE.h"
#include "HRenderTarget.h"
#include "HTEXTURE.h"


void HCAM::StartSetting(int _Order)
{
	//Game_Vector A = Game_Vector{ 1, 0 , 0 };
	//Game_Vector B = Game_Vector{ 1, 0 , 0 };

	//A.NORMAL3D();
	//B.NORMAL3D();

	//float Light = A.Dot3D(B);


	// Ʈ�������� ���� ���Ϳ� ����.
	if (nullptr == ACTOR()->TRANS())
	{
		AMSG(L"Ʈ�������� ���� ���Ϳ� ķ�� �־����ϴ�.");
	}
	Order(_Order);

	m_CAMFOV = 60.0F;
	m_CAMFAR = 1000.0F;
	m_CAMNEAR = 0.1F;
	m_MODE = CAMMODE::ORTH;
	m_CAMSIZE = HGAMEWINDOW::MAINOBJ()->Size();

	ACTOR()->SCENE()->PushCam(this);

	m_CamTarget = new Game_RenderTarget();
	m_CamTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE);
	m_CamTarget->Depth(Game_Device::MAINOBJ()->BackBufferRenderTarget()->Depth());

	m_ForwardCamTarget = new Game_RenderTarget();
	m_ForwardCamTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(1), Game_Vector::NONE, false);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(2), Game_Vector::NONE, false);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(3), Game_Vector::NONE, false);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(4), Game_Vector::NONE, false);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(5), Game_Vector::NONE, false);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(6), Game_Vector::NONE, false);
	m_ForwardCamTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(7), Game_Vector::NONE, false);

	m_ForwardCamTarget->Depth(Game_Device::MAINOBJ()->BackBufferRenderTarget()->Depth());

	m_DefferdCamTarget = new Game_RenderTarget();
	m_DefferdCamTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE);
	m_DefferdCamTarget->Depth(Game_Device::MAINOBJ()->BackBufferRenderTarget()->Depth());


	m_GbufferTarget = new Game_RenderTarget();
	m_GbufferTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // ��ǻ��
	m_GbufferTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // ������
	m_GbufferTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // �븻
	m_GbufferTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // ����
	m_GbufferTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(4), Game_Vector::NONE, false);
	m_GbufferTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(5), Game_Vector::NONE, false);
	m_GbufferTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(6), Game_Vector::NONE, false);
	m_GbufferTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(7), Game_Vector::NONE, false);
	m_GbufferTarget->Depth(Game_Device::MAINOBJ()->BackBufferRenderTarget()->Depth());


	DefferdLightPlayer = new HRENDERPLAYER();
	DefferdLightPlayer->MESH(L"TargetFullRect");
	DefferdLightPlayer->MATERIAL(L"DefferdLight");
	DefferdLightPlayer->SAMPLER(L"SAMPLER", L"LWSMP");
	DefferdLightPlayer->SAMPLER(L"SHADOWSAMPLER", L"LCSMP");
	DefferdLightPlayer->TEXTURE(L"PosTex", m_GbufferTarget->Texture(1));
	DefferdLightPlayer->TEXTURE(L"NorTex", m_GbufferTarget->Texture(2));


	m_DefferdLightTarget = new Game_RenderTarget();
	m_DefferdLightTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // ��ǻ��
	m_DefferdLightTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // ����ŧ��
	m_DefferdLightTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // �����Ʈ
	m_DefferdLightTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // �׳� �������� Ÿ��
	m_DefferdLightTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(4), Game_Vector::NONE, false);
	m_DefferdLightTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(5), Game_Vector::NONE, false);
	m_DefferdLightTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(6), Game_Vector::NONE, false);
	m_DefferdLightTarget->CreateTarget(Game_Device::MAINOBJ()->DataRenderTarget()->Texture(7), Game_Vector::NONE, false);
	m_DefferdLightTarget->Depth(Game_Device::MAINOBJ()->BackBufferRenderTarget()->Depth());


	DefferdMergePlayer = new HRENDERPLAYER();
	DefferdMergePlayer->MESH(L"TargetFullRect");
	DefferdMergePlayer->MATERIAL(L"DefferdMerge");
	DefferdMergePlayer->SAMPLER(L"SAMPLER", L"LWSMP");
	DefferdMergePlayer->TEXTURE(L"DifColor", m_GbufferTarget->Texture(0));
	DefferdMergePlayer->TEXTURE(L"LightColor", m_DefferdLightTarget->Texture(3));
}

void HCAM::RenderPrev() 
{
	//CBTRANSDATA.V.VIEWLH(CamPos, Game_Vector::FORWARD, Game_Vector::UP);
	//CBTRANSDATA.P.ORTHLH(CamScale, 0.1f, 100.0f);
	// ī�޶� ��� �ִ� ������ ��ġ�� �� ī�޶� �����ִ� ��ġ�̴�

	// �̳༮�� ������� ����� ���.
	// ȸ����Ŀ��� ���ü� �ִ�.

	Game_Vector Pos = ACTOR()->TRANS()->WPOS();
	Game_Vector F = ACTOR()->TRANS()->WFORWARD();
	Game_Vector U = ACTOR()->TRANS()->WUP();

	m_VIEW.VIEWLH(ACTOR()->TRANS()->WPOS(), ACTOR()->TRANS()->WFORWARD(), ACTOR()->TRANS()->WUP());

	switch (m_MODE)
	{
	case CAMMODE::ORTH:
		m_PROJ.ORTHLH(m_CAMSIZE, m_CAMNEAR, m_CAMFAR);
		break;
	case CAMMODE::PERS:
		m_PROJ.PERSDEG(m_CAMFOV, m_CAMSIZE, m_CAMNEAR, m_CAMFAR);
		break;
	default:
		AMSG(L"ķ ��� ������ �߸��ƽ��ϴ�.");
		break;
	}

}


Game_Vector HCAM::CamOrthMousePos2D()
{
	Game_Vector ScreenMousePos = HGAMEWINDOW::MAINOBJ()->MousePos();
	HMATRIX ScreenViewPort = HGAMEWINDOW::MAINOBJ()->ViewPort();
	ScreenViewPort.Inverse();
	ScreenMousePos *= ScreenViewPort;
	ScreenMousePos *= CAMSIZE().HalfVector();
	return ScreenMousePos;
}

Game_Vector HCAM::OrthWorldMousePos2D()  
{
	return TRANS()->WPOS2D() + CamOrthMousePos2D();
}

void HCAM::CamTargetMerge() 
{
	m_CamTarget->Clear(false);
	m_CamTarget->Merge(m_ForwardCamTarget);
	m_CamTarget->Merge(m_DefferdCamTarget);
}

Game_Ptr<Game_RenderTarget> HCAM::CamTarget() 
{
	return m_CamTarget;
}

Game_Ptr<Game_RenderTarget> HCAM::ForwardCamTarget() 
{ 
	return m_ForwardCamTarget;
}
Game_Ptr<Game_RenderTarget> HCAM::DefferdCamTarget() 
{
	return m_DefferdCamTarget;
}

Game_Ptr<Game_RenderTarget> HCAM::DefferdLightTarget() 
{
	return m_DefferdLightTarget;
}
Game_Ptr<Game_RenderTarget> HCAM::GbufferTarget() 
{
	return m_GbufferTarget;
}

void HCAM::CalDefferdLight(const LightData& _Data, Game_Ptr<HTEXTURE> _Tex) 
{
	m_DefferdLightTarget->Setting();
	DefferdLightPlayer->CBUFFER(L"ONELIGHTDATA", (void*)&_Data, CBUFFERMODE::CB_LINK);
	DefferdLightPlayer->TEXTURE(L"ShadowTex", _Tex);
	DefferdLightPlayer->Render();
	DefferdLightPlayer->Reset();
}

void HCAM::DefferdMerge() 
{
	m_DefferdCamTarget->Clear(false);
	m_DefferdCamTarget->Setting();
	DefferdMergePlayer->Render();
	DefferdMergePlayer->Reset();
}




void HFilter::StartTargetCreate()
{
	OutTarget = new Game_RenderTarget();
	OutTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size(), Game_Vector::NONE); // ��ǻ��
}


void HCAM::CamMergeNextEffect() 
{
	for (auto& _Filter : HLISTOBJMGR<HFilter>::m_ObjList)
	{
		_Filter.Obj->CamMergeNextEffect();
	}
}

void HCAM::CamMergePrevEffect()
{
	for (auto& _Filter : HLISTOBJMGR<HFilter>::m_ObjList)
	{
		_Filter.Obj->CamMergePrevEffect();
	}
}

void HCAM::DefferdMergeNextEffect()
{
	for (auto& _Filter : HLISTOBJMGR<HFilter>::m_ObjList)
	{
		_Filter.Obj->DefferdMergeNextEffect();
	}
}

void HCAM::DefferdMergePrevEffect()
{
	for (auto& _Filter : HLISTOBJMGR<HFilter>::m_ObjList)
	{
		_Filter.Obj->DefferdMergePrevEffect();
	}
}