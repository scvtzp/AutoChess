#pragma once
#include "HCAM.h"


struct OUTLINEDATA
{
	float4 PixelUv[4];
	float4 LineColor;
};

class HOutLineFilter : public HFilter
{
public:
	Game_Ptr<HRENDERPLAYER> LinePlayer;
	OUTLINEDATA Data;

public:
	void Init(int _Len);

private:
	void DefferdMergePrevEffect() override;
	void CamMergeNextEffect() override;
};

