#include "HBloomFilter.h"
#include "HRenderTarget.h"

void HBloomFilter::Init(int _Len)
{
	float SizeRatio = 0.25f;

	StartTargetCreate();
	BloomCheckPlayer = new HRENDERPLAYER();
	BloomCheckPlayer->MESH(L"TargetFullRect");
	BloomCheckPlayer->MATERIAL(L"EmissionBloomCheck");
	BloomCheckPlayer->TEXTURE(L"EmissionTex", Game_Device::MAINOBJ()->DataRenderTarget()->Texture(6));
	BloomCheckPlayer->TEXTURE(L"LightTex", m_Parent->DefferdLightTarget()->Texture(3));
	BloomCheckPlayer->SAMPLER(L"Smp", L"LCSMP");

	SmallTarget = new Game_RenderTarget();
	SmallTarget->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size() * SizeRatio, Game_Vector::NONE);

	SmallTarget2 = new Game_RenderTarget();
	SmallTarget2->CreateTarget(HGAMEWINDOW::MAINOBJ()->Size() * SizeRatio, Game_Vector::NONE);


	SmallUvSize.x = 1 / (HGAMEWINDOW::MAINOBJ()->Size().x * SizeRatio);
	SmallUvSize.y = 1 / (HGAMEWINDOW::MAINOBJ()->Size().y * SizeRatio);

	BloomPlayer = new HRENDERPLAYER();
	BloomPlayer->MESH(L"TargetFullRect");
	BloomPlayer->MATERIAL(L"EmissionBloom");
	BloomPlayer->CBUFFER(L"TargetUvSize", &SmallUvSize, CBUFFERMODE::CB_LINK);
	BloomPlayer->TEXTURE(L"SmallTex", SmallTarget->Texture(0));
	BloomPlayer->SAMPLER(L"Smp", L"LCSMP");


	


}

void HBloomFilter::DefferdMergePrevEffect()
{
	SmallTarget->Clear(false);
	SmallTarget->Setting();
	BloomCheckPlayer->Render();
	BloomCheckPlayer->Reset();

	SmallTarget2->Clear(false);
	SmallTarget2->Setting();
	BloomPlayer->Render();
	BloomPlayer->Reset();

	m_Parent->DefferdLightTarget()->Plus(SmallTarget2, 3, 0, true);
}

void HBloomFilter::CamMergeNextEffect()
{
	// m_Parent->CamTarget()->Merge(OutTarget);
}