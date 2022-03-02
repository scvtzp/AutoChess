#include "Game_SpriteRenderer.h"
#include "HGAMEACTOR.h"
#include "HGAMESCENE.h"
#include "Game_Device.h"
#include "HVBBUFFER.h"
#include "HIBBUFFER.h"
#include "HCBUFFER.h"
#include "HDEPTHSTENCIL.h"
#include "HBLEND.h"
#include "HRASTERIZER.h"
#include "HVTXSHADER.h"
#include "HPIXSHADER.h"
#include "HSAMPLER.h"
#include "HGAMESPRITE.h"
#include "HCAM.h"

void Game_SpriteRenderer::Init(int _Order) {

	 //부모의 함수 호출하는 방법.
	 Game_Renderer::Init(_Order);

	if (nullptr == HCBUFFER::Find(L"TRANSDATA"))
	{
		HCBUFFER::Create<HTRANSDATA>(L"TRANSDATA");
		HCBUFFER::Create<Game_Vector>(L"COLOR");
		HCBUFFER::Create<Game_Vector>(L"CUTDATA");
	}

	m_Color = Game_Vector::WHITE;
}
void Game_SpriteRenderer::Render(Game_Ptr<HCAM> _Cam)
{
	//Game_Ptr<HVTXSHADER> VTXS = HVTXSHADER::Find(L"VS_2DIMG");
	//Game_Ptr<HPIXSHADER> PIXS = HPIXSHADER::Find(L"PS_2DIMG");

	// auto& Arr = VTXS->m_ResData;

	// //텍스처를 넣어줘야 한다 정보
	// //텍스처를 넣어주면 되는데 => HTEXTURE

	//if (nullptr == m_SPRITE)
	//{
	//	AMSG(L"스프라이트가 세팅되지 않았습니다.");
	//}

	//// 부모인 트랜스폼의 기능
	//CamUpdate(_Cam);

	//Game_Ptr<HRASTERIZER> RS = HRASTERIZER::Find(L"NONE");
	//Game_Ptr<HDEPTHSTENCIL> DS = HDEPTHSTENCIL::Find(L"DEFDEPTH");
	//Game_Ptr<HBLEND> BLEND = HBLEND::Find(L"AlphaBlend");
	//
	//RS->Setting();
	//DS->Setting();
	//BLEND->Setting();

	//// Game_Ptr<HTEXTURE> TEX = HTEXTURE::Find(L"WALKLEFT.png");
	//Game_Ptr<HSAMPLER> SMP = HSAMPLER::Find(L"PMSMP");

	//m_SPRITE->Tex()->Setting(SHADERTYPE::SHADER_PS, 0);
	//// TEX->Setting(SHADERTYPE::SHADER_PS, 0);
	//SMP->Setting(SHADERTYPE::SHADER_PS, 0);


	////PIXS->Setting();

	//Game_Ptr<HCBUFFER> CBTRANS = HCBUFFER::Find(L"TRANSDATA");
	//Game_Ptr<HCBUFFER> CBCOLOR = HCBUFFER::Find(L"COLOR");
	//Game_Ptr<HCBUFFER> CUTDATA = HCBUFFER::Find(L"CUTDATA");

	//CutData = m_SPRITE->SpriteData(m_SpriteIndex);

	//CBTRANS->DataChange(m_TRANSDATA);
	//CBCOLOR->DataChange(m_Color);
	//CUTDATA->DataChange(CutData);

	//CBTRANS->Setting(SHADERTYPE::SHADER_VS, 0);
	//CUTDATA->Setting(SHADERTYPE::SHADER_VS, 1);
	//CBCOLOR->Setting(SHADERTYPE::SHADER_PS, 0);

	//VTXS->Setting();
	//PIXS->Setting();

	//Game_Ptr<HVBBUFFER> VB = HVBBUFFER::Find(L"2DCOLORRECT");
	//Game_Ptr<HIBBUFFER> IB = HIBBUFFER::Find(L"2DCOLORRECT");
	//VB->Setting();
	//IB->Setting();

	//Game_Device::MAINOBJ()->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Game_Device::MAINOBJ()->Context()->DrawIndexed(IB->Count(), 0, 0);
	
	if (nullptr == m_SPRITE)
	{
		AMSG(L"스프라이트가 세팅되지 않았습니다.");
	}

	// 부모인 트랜스폼의 기능
	CamUpdate(_Cam);

	Game_Ptr<HRASTERIZER> RS = HRASTERIZER::Find(L"BACK");
	Game_Ptr<HDEPTHSTENCIL> DS = HDEPTHSTENCIL::Find(L"DEFDEPTH");
	Game_Ptr<HBLEND> BLEND = HBLEND::Find(L"AlphaBlend");

	RS->Setting();
	DS->Setting();
	BLEND->Setting();

	// HPTR<HTEXTURE> TEX = HTEXTURE::Find(L"WALKLEFT.png");
	Game_Ptr<HSAMPLER> SMP = HSAMPLER::Find(L"PMSMP");

	m_SPRITE->Tex()->Setting(SHADERTYPE::SHADER_PS, 0);
	// TEX->Setting(SHADERTYPE::SHADER_PS, 0);
	SMP->Setting(SHADERTYPE::SHADER_PS, 0);


	Game_Ptr<HVTXSHADER> VTXS = HVTXSHADER::Find(L"VS_2DIMG");
	Game_Ptr<HPIXSHADER> ITXS = HPIXSHADER::Find(L"PS_2DIMG");

	Game_Ptr<HCBUFFER> CBTRANS = HCBUFFER::Find(L"TRANSDATA");
	Game_Ptr<HCBUFFER> CBCOLOR = HCBUFFER::Find(L"COLOR");
	Game_Ptr<HCBUFFER> CUTDATA = HCBUFFER::Find(L"CUTDATA");

	CutData = m_SPRITE->SpriteData(m_SpriteIndex);

	CBTRANS->DataChange(m_TRANSDATA);
	CBCOLOR->DataChange(m_Color);
	CUTDATA->DataChange(CutData);

	CBTRANS->Setting(SHADERTYPE::SHADER_VS, 0);
	CUTDATA->Setting(SHADERTYPE::SHADER_VS, 1);
	CBCOLOR->Setting(SHADERTYPE::SHADER_PS, 0);

	VTXS->Setting();
	ITXS->Setting();

	Game_Ptr<HVBBUFFER> VB = HVBBUFFER::Find(L"2DCOLORRECT");
	Game_Ptr<HIBBUFFER> IB = HIBBUFFER::Find(L"2DCOLORRECT");
	VB->Setting();
	IB->Setting();

	Game_Device::MAINOBJ()->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Game_Device::MAINOBJ()->Context()->DrawIndexed(IB->Count(), 0, 0);
}