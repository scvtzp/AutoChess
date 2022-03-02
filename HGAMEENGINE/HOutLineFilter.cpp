#include "HOutLineFilter.h"
#include "HRenderTarget.h"

void HOutLineFilter::Init(int _Len)
{
	StartTargetCreate();
	Data.LineColor = float4::RED;

	Data.PixelUv[0].x = 1.0f / Game_Device::MAINOBJ()->BackBufferRenderTarget()->Texture(0)->Size().x * _Len;
	Data.PixelUv[0].y = 0.0f;

	Data.PixelUv[1].x = -(1.0f / Game_Device::MAINOBJ()->BackBufferRenderTarget()->Texture(0)->Size().x) * _Len;
	Data.PixelUv[1].y = 0.0f;

	Data.PixelUv[2].x = 0.0f;
	Data.PixelUv[2].y = -(1.0f / Game_Device::MAINOBJ()->BackBufferRenderTarget()->Texture(0)->Size().y) * _Len;

	Data.PixelUv[3].x = 0.0f;
	Data.PixelUv[3].y = 1.0f / Game_Device::MAINOBJ()->BackBufferRenderTarget()->Texture(0)->Size().y * _Len;

	LinePlayer = new HRENDERPLAYER();

	LinePlayer->MESH(L"TargetFullRect");
	LinePlayer->MATERIAL(L"OUTLINEDRAW");

	LinePlayer->CBUFFER(L"OUTLINEDATA", &Data, CBUFFERMODE::CB_LINK);
	LinePlayer->TEXTURE(L"Tex", Game_Device::MAINOBJ()->DataRenderTarget()->Texture(7));
	LinePlayer->SAMPLER(L"Smp", L"LCSMP");
}

void HOutLineFilter::DefferdMergePrevEffect()
{
	OutTarget->Clear(false);
	OutTarget->Setting();
	LinePlayer->Render();
	LinePlayer->Reset();
}

void HOutLineFilter::CamMergeNextEffect()
{
	m_Parent->CamTarget()->Merge(OutTarget);
}